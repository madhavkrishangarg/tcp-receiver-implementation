#include "byte_stream.hh"

#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh`

/* Replace all the dummy definitions inside the methods in this file. */

using namespace std;

ByteStream::ByteStream(const size_t capa) : endInput(false), bytesRead(0), bytesWritten(0), capacity(capa)
{
}

size_t ByteStream::write(const string &data)
{
  if (endInput || error())
  {
    return 0;
  }
  size_t bytes_input = 0;
  size_t s = stream.size();
  for (char x : data)
  {
    if (s < capacity)
    {
      stream.push_back(x);
      bytes_input += 1;
      bytesWritten += 1;
      s = stream.size();
    }
    else if (s >= capacity)
    {
      break;
    }
    else
    {
      break;
    }
  }
  return bytes_input;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const
{
  if (error())
  {
    return "Error"; // error string
  }

  string output;
  // size_t count = 0;
  for (char x : stream)
  {
    if (output.size() < len)
    {
      output += x;
      // count++;
    }
    else
    {
      break;
    }
  }

  return output;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len)
{
  if (error())
  {
    return; // error string
  }
  size_t c = 0;
  if (stream.size() < len)
  {
    set_error();
    return;
  }
  while (!stream.empty() && c < len)
  {

    c += 1;
    stream.pop_front();
    bytesRead += 1;
  }
  return;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len)
{
  if (stream.size() < len)
  {
    set_error();
  }
  if (error())
  {
    return "Error";
  }

  string output;
  size_t c = 0;

  while (!stream.empty() && c < len)
  {
    char x = stream.front();
    output += x;
    stream.pop_front();
    c += 1;
    bytesRead += 1;
  }

  return output;
}

void ByteStream::end_input()
{
  endInput = true;
}

bool ByteStream::input_ended() const { return endInput; }

size_t ByteStream::buffer_size() const
{
  return stream.size();
}

size_t ByteStream::bytes_written() const { return bytesWritten; }

size_t ByteStream::bytes_read() const { return bytesRead; }

size_t ByteStream::remaining_capacity() const
{

  return capacity - stream.size();
}

bool ByteStream::buffer_empty() const { return stream.empty(); }

bool ByteStream::eof() const { return endInput && stream.empty(); }
