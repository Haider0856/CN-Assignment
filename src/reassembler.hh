#pragma once

#include "byte_stream.hh"
#include <map>
#include <string>
#include <cstdint>

class Reassembler
{
public:
  explicit Reassembler( ByteStream&& output );

  void insert( uint64_t first_index, std::string data, bool is_last_substring );

 
  uint64_t count_bytes_pending() const;


  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }
  const Writer& writer() const { return output_.writer(); }

private:
  ByteStream output_;                              // underlying byte stream
  std::map<uint64_t, std::string> unassembled_;    // stored out-of-order substrings
  uint64_t next_index_ { 0 };                      // next expected byte index
  bool eof_received_ { false };                    // true if last-substring flag seen
  uint64_t eof_index_ { 0 };                       // index where EOF occurs (first_index + length)
};
