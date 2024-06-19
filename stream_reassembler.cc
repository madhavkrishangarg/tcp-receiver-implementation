
#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), ackIndex(0), buffer(), capacity(capacity), waitingIndex()
{
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof)
{
    if (index > ackIndex + _output.remaining_capacity())
    {
        return;
    }

    if (index + data.size() <= ackIndex)
    {
        // when segment is already written
        if (eof)
        {
            _output.end_input();
        }
        return;
    }

    if (index <= ackIndex && index + data.size() > ackIndex)
    { // when segment is partially written
        string tmp = data.substr(ackIndex - index, data.size() - (ackIndex - index));
        _output.write(tmp);
        ackIndex = _output.bytes_written();

        if (eof)
        {
            _output.end_input();
        }

        while (!waitingIndex.empty() && waitingIndex.top().first.first <= ackIndex)
        {
            if (waitingIndex.top().first.first + waitingIndex.top().first.second.size() > ackIndex)
            {

                string tmp = waitingIndex.top().first.second.substr(ackIndex - waitingIndex.top().first.first);
                _output.write(tmp);
                ackIndex = _output.bytes_written();
            }
            if (waitingIndex.top().second == true && waitingIndex.top().first.first + waitingIndex.top().first.second.size() <= capacity)
            {
                _output.end_input();
                break;
            }
            waitingIndex.pop();
        }
        capacity = _output.remaining_capacity();
        return;
    }

    if (index > ackIndex)
    { // if received segment is out of order

        // for(auto x = buffer.begin(); x!=buffer.end(); x++)
        // {
        //     if(x->first<=index && x->first+x->second.size()>index)
        //     {
        // //if we receive data=="bcde" and index=1 and we already have data="ab", index=0 and data="cd" and index=2 in buffer
        // //we need to update buffer[2] to "cde"
        //      if (x->first + x->second.size() < index + data.size() && buffer.find(x->first + x->second.size()) != buffer.end())
        //         {
        //             string tmp = data.substr(x->first + x->second.size() - index);
        //             if(tmp.size()>buffer[x->first + x->second.size()].size())
        //             {
        //                 buffer[x->first + x->second.size()] = tmp;
        //             }
        //             break;
        //         }

        //         if(x->first + x->second.size()< index+data.size() && buffer.find(x->first + x->second.size()) == buffer.end())
        //         {
        //             string tmp = data.substr(x->first + x->second.size() - index);
        //             buffer[x->first + x->second.size()] = tmp;
        //             break;
        //         }

        //     }
        // }

        if (index + data.size() > ackIndex + _output.remaining_capacity())
        { // out of range
            string tmp = data.substr(0, ackIndex + _output.remaining_capacity() - index);
            waitingIndex.push(make_pair(make_pair(index, tmp), eof));
            // unassembledBytes += tmp.size();
            return;
        }
        else
        {
            waitingIndex.push(make_pair(make_pair(index, data), eof));
        }
    }
}

size_t StreamReassembler::unassembled_bytes() const
{
    size_t unassembledBytes = 0;
    int tmp = ackIndex;

    priority_queue<pair<pair<int, string>, bool>, vector<pair<pair<int, string>, bool>>, greater<pair<pair<int, string>, bool>>> waitingIndexCopy = waitingIndex;

    while (!waitingIndexCopy.empty())
    {
        if (waitingIndexCopy.top().first.first <= tmp && waitingIndexCopy.top().first.first + waitingIndexCopy.top().first.second.size() > tmp)
        {
            unassembledBytes += waitingIndexCopy.top().first.second.size() - (tmp - waitingIndexCopy.top().first.first);
            tmp = waitingIndexCopy.top().first.first + waitingIndexCopy.top().first.second.size();
        }
        if (waitingIndexCopy.top().first.first > tmp)
        {
            unassembledBytes += waitingIndexCopy.top().first.second.size();
            tmp = waitingIndexCopy.top().first.first + waitingIndexCopy.top().first.second.size();
        }
        waitingIndexCopy.pop();
    }

    // for (auto &x : buffer)
    // {
    //     if (x.first <= tmp && x.first + x.second.size() > tmp)
    //     {
    //         unassembledBytes += x.second.size() - (tmp - x.first);
    //         tmp = x.first + x.second.size();
    //     }
    //     if (x.first > tmp)
    //     {
    //         unassembledBytes += x.second.size();
    //         tmp = x.first + x.second.size();
    //     }
    // }

    // while(!)
    return unassembledBytes;
}

bool StreamReassembler::empty() const
{
    return unassembled_bytes() == 0;
}

size_t StreamReassembler::ack_index() const
{
    return ackIndex;
}