#pragma once
#include <string>
#include <string_view>

class Writer
{
private:
  size_t _capacity;
  std::string _buffer;
  uint64_t _bytes_pushed;
  bool _closed;

public:
  Writer( const size_t capacity );

  void push( std::string data );
  void close();
  bool is_closed() const;
  uint64_t available_capacity() const;
  uint64_t bytes_pushed() const;
  const std::string &peek_buffer() const;
  bool is_finished() const;

  friend class Reader;
};

class Reader
{
private:
  Writer _writer;
  uint64_t _bytes_popped;

public:
  Reader( const size_t capacity );

  std::string_view peek() const;
  void pop( uint64_t len );
  bool is_finished() const;
  uint64_t bytes_buffered() const;
  uint64_t bytes_popped() const;
};
