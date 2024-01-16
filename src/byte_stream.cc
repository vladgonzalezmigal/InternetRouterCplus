#include "byte_stream.hh"
#include <stdexcept>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  return closed;
}

void Writer::push( string data )
{
  if (closed) return;
  if (data.length() <= available_capacity()){
    cumPushed += data.length();
    buffer += data;
  } else {
    cumPushed += available_capacity();
    buffer += data.substr(0,available_capacity());
  }
  return;
}

void Writer::close()
{
  closed = true;  
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - buffer.length();
}

uint64_t Writer::bytes_pushed() const
{
  return cumPushed;
}

bool Reader::is_finished() const
{
  // Need check for edge case where capacity = 0?
  return closed && (buffer.length() == 0);
}

uint64_t Reader::bytes_popped() const
{
  return cumPopped;
}

string_view Reader::peek() const
{
  return string_view(buffer);
}

void Reader::pop( uint64_t len )
{
  // need exception here 
  if (len <= bytes_buffered()){
    buffer.erase(buffer.begin(), buffer.begin() + len);
    cumPopped += len;
  } else {
    set_error();
    throw std::invalid_argument("trying to pop more than buffer length");
    has_error();
  }
}

uint64_t Reader::bytes_buffered() const
{
  return buffer.length();
}
