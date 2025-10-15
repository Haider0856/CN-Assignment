#include "byte_stream.hh"
#include "debug.hh"
#include <algorithm>

using namespace std;

// ByteStream constructor
ByteStream::ByteStream(uint64_t capacity)
    : capacity_(capacity),
      error_(false),
      buffer_(),
      total_pushed_(0),
      total_popped_(0),
      input_ended_(false) {}

// ---------------- Writer implementation ----------------

void Writer::push(std::string data) {
  // write only as much as fits in the remaining capacity
  uint64_t space_left = available_capacity();
  uint64_t to_write = std::min<uint64_t>(space_left, static_cast<uint64_t>(data.size()));
  if (to_write == 0) {
    // nothing fits right now; just return (writer should obey capacity)
    return;
  }
  buffer_.append(data.data(), to_write);
  total_pushed_ += to_write;
}

void Writer::close() {
  input_ended_ = true;
}

bool Writer::is_closed() const {
  return input_ended_;
}

uint64_t Writer::available_capacity() const {
  // capacity_ minus currently buffered bytes
  if (buffer_.size() >= capacity_) return 0;
  return capacity_ - static_cast<uint64_t>(buffer_.size());
}

uint64_t Writer::bytes_pushed() const {
  return total_pushed_;
}

// ---------------- Reader implementation ----------------

std::string_view Reader::peek() const {
  // Return a view over the buffered bytes (possibly empty)
  return std::string_view(buffer_);
}

void Reader::pop(uint64_t len) {
  // remove up to len bytes from the front of the buffer
  uint64_t have = static_cast<uint64_t>(buffer_.size());
  uint64_t to_remove = std::min(len, have);
  if (to_remove == 0) return;
  // Erase from the beginning
  buffer_.erase(0, to_remove);
  total_popped_ += to_remove;
}

bool Reader::is_finished() const {
  // finished when writer closed and buffer empty
  return input_ended_ && buffer_.empty();
}

uint64_t Reader::bytes_buffered() const {
  return static_cast<uint64_t>(buffer_.size());
}

uint64_t Reader::bytes_popped() const {
  return total_popped_;
}


