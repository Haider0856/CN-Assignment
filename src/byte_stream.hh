#pragma once

#include <cstdint>
#include <string>
#include <string_view>

class Reader;
class Writer;

class ByteStream
{
public:
  explicit ByteStream(uint64_t capacity);

  // Helper functions (provided) to access the ByteStream's Reader and Writer interfaces
  Reader& reader();
  const Reader& reader() const;
  Writer& writer();
  const Writer& writer() const;

  void set_error() { error_ = true; }       // Signal that the stream suffered an error.
  bool has_error() const { return error_; } // Has the stream had an error?

protected:
  // Put all actual stream state here (Reader/Writer must be size-compatible with ByteStream).
  uint64_t capacity_;
  bool error_ {};
  std::string buffer_ {};       // stored bytes that have been pushed but not yet popped
  uint64_t total_pushed_ = 0;   // cumulative bytes pushed
  uint64_t total_popped_ = 0;   // cumulative bytes popped
  bool input_ended_ = false;    // writer closed the stream
};

// Reader and Writer are *views* into ByteStream (no additional data members).
class Writer : public ByteStream
{
public:
  // Push data to stream, but only as much as available capacity allows.
  void push(std::string data);

  // Signal that the stream has reached its ending. Nothing more will be written.
  void close();

  // Has the stream been closed?
  bool is_closed() const;

  // How many bytes can be pushed to the stream right now?
  uint64_t available_capacity() const;

  // Total number of bytes cumulatively pushed to the stream
  uint64_t bytes_pushed() const;
};

class Reader : public ByteStream
{
public:
  // Peek at the next bytes in the buffer -- ideally as many as possible.
  std::string_view peek() const;

  // Remove `len` bytes from the buffer.
  void pop(uint64_t len);

  // Is the stream finished (closed and fully popped)?
  bool is_finished() const;

  // Number of bytes currently buffered (pushed and not popped)
  uint64_t bytes_buffered() const;

  // Total number of bytes cumulatively popped from stream
  uint64_t bytes_popped() const;
};

/*
 * read: A (provided) helper function that peeks and pops up to `max_len` bytes
 * from a ByteStream Reader into a string;
 */
void read(Reader& reader, uint64_t max_len, std::string& out);
