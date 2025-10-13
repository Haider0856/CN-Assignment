#include "byte_stream.hh"
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  uint64_t space_available = available_capacity();
  uint64_t bytes_to_write = min( static_cast<uint64_t>( data.size() ), space_available );
  
  buffer_.append( data.substr( 0, bytes_to_write ) );
  total_pushed_ += bytes_to_write;
}

void Writer::close()
{
  is_closed_ = true;
}

bool Writer::is_closed() const
{
  return is_closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  return total_pushed_;
}

string_view Reader::peek() const
{
  return string_view( buffer_ );
}

void Reader::pop( uint64_t len )
{
  uint64_t bytes_to_remove = min( len, static_cast<uint64_t>( buffer_.size() ) );
  buffer_.erase( 0, bytes_to_remove );
  total_popped_ += bytes_to_remove;
}

bool Reader::is_finished() const
{
  return is_closed_ && buffer_.empty();
}

uint64_t Reader::bytes_buffered() const
{
  return buffer_.size();
}

uint64_t Reader::bytes_popped() const
{
  return total_popped_;
}