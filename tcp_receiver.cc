#include "tcp_receiver.hh"

#include <algorithm>
#include <string>

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();

    std::string payload_copy=seg.payload().copy();

    if(_synReceived == false && head.syn == false)
        return;

    if(head.fin == true)
            _finReceived = true;

    if(_synReceived == false && head.syn == true) {
        _synReceived = true;
        _isn = head.seqno;
        _reassembler.push_substring(seg.payload().copy(), 0, head.fin);
        return;
    }
        // convert the seqno into absolute seqno
    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;
    
    _reassembler.push_substring(seg.payload().copy(), stream_idx, head.fin);

    return;

}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(_synReceived)
        if(_reassembler.empty() && _finReceived == true)
            return wrap(_reassembler.ack_index() + 2, _isn);        //send ack for fin
        else
            return wrap(_reassembler.ack_index() + 1, _isn);        //send ack for stream
    else
        return {};
}

size_t TCPReceiver::window_size() const { 
    if(_capacity == _reassembler.stream_out().buffer_size())
        return 0; 
    size_t window_size = _capacity - _reassembler.stream_out().buffer_size();
    return window_size;
 }
