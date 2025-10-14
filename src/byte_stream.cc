#include "byte_stream.hh"
#include "debug.hh"
#include <algorithm>

using namespace std;

// ============================
// Writer implementation
// ============================

Writer::Writer( const size_t capacity )
  : _capacity( capacity ), _buffer(), _bytes_pushed( 0 ), _closed( false ) {}

void Writer::push( string data )
{
  // Write only as much as fits in the remaining capacity
  size_t space_left = available_capacity();
  size_t to_write = min( space_left, data.size() );

  _buffer.append( data.substr( 0, to_write ) );
  _bytes_pushed += to_write;
}

void Writer::close() { _closed = true; }

bool Writer::is_closed() const { return _closed; }

uint64_t Writer::available_capacity() const
{
  return _capacity - _buffer.size();
}

uint64_t Writer::bytes_pushed() const { return _bytes_pushed; }

const string &Writer::peek_buffer() const { return _buffer; }

bool Writer::is_finished() const { return _closed && _buffer.empty(); }

// ============================
// Reader implementation
// ============================

Reader::Reader( const size_t capacity )
  : _writer( capacity ), _bytes_popped( 0 ) {}

string_view Reader::peek() const { return _writer.peek_buffer(); }

void Reader::pop( uint64_t len )
{
  len = min( len, _writer._buffer.size() );
  _writer._buffer.erase( 0, len );
  _bytes_popped += len;
}

bool Reader::is_finished() const { return _writer.is_finished(); }

uint64_t Reader::bytes_buffered() const { return _writer._buffer.size(); }

uint64_t Reader::bytes_popped() const { return _bytes_popped; }
