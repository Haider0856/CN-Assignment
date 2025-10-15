#include "reassembler.hh"
#include "debug.hh"
#include <algorithm>

using namespace std;

Reassembler::Reassembler( ByteStream&& output )
  : output_( std::move( output ) ),
    unassembled_(),
    next_index_( 0 ),
    eof_received_( false ),
    eof_index_( 0 )
{}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  debug( "Reassembler::insert({}, {}, {}) called", first_index, data, is_last_substring );

  
  if ( is_last_substring ) {
    eof_received_ = true;
    eof_index_ = first_index + static_cast<uint64_t>(data.size());
  }

  
  if ( data.empty() && !is_last_substring ) {
    return;
  }

  uint64_t end_index = first_index + static_cast<uint64_t>(data.size());

  
  if ( end_index <= next_index_ ) {
    if ( eof_received_ && next_index_ >= eof_index_ )
      output_.writer().close();
    return;
  }

  // Trim already assembled prefix
  if ( first_index < next_index_ ) {
    data.erase( 0, static_cast<size_t>( next_index_ - first_index ) );
    first_index = next_index_;
  }

  // Respect available capacity
  uint64_t store_limit = next_index_ + output_.writer().available_capacity();
  if ( first_index >= store_limit ) {
    if ( eof_received_ && next_index_ >= eof_index_ )
      output_.writer().close();
    return;
  }

  // Trim data if exceeds capacity
  if ( end_index > store_limit ) {
    data.resize( static_cast<size_t>( store_limit - first_index ) );
  }

  // Store or merge with existing data
  if ( !data.empty() ) {
    auto it = unassembled_.find( first_index );
    if ( it == unassembled_.end() ) {
      unassembled_.emplace( first_index, std::move( data ) );
    } else {
      std::string& existing = it->second;
      if ( data.size() > existing.size() )
        existing = std::move( data );
    }
  }

  
  if ( !unassembled_.empty() ) {
    std::map<uint64_t, std::string> merged;
    for ( auto &kv : unassembled_ ) {
      uint64_t idx = kv.first;
      std::string seg = std::move(kv.second);
      if ( merged.empty() ) {
        merged.emplace( idx, std::move(seg) );
        continue;
      }
      auto &last_pair = *merged.rbegin();
      uint64_t last_idx = last_pair.first;
      std::string &last_seg = last_pair.second;
      uint64_t last_end = last_idx + static_cast<uint64_t>(last_seg.size());

      if ( idx <= last_end ) {
        if ( idx + static_cast<uint64_t>(seg.size()) > last_end )
          last_seg.append( seg.substr( static_cast<size_t>( last_end - idx ) ) );
      } else {
        merged.emplace( idx, std::move(seg) );
      }
    }
    unassembled_.swap( merged );
  }

  
  while ( !unassembled_.empty() ) {
    auto it = unassembled_.begin();
    uint64_t seg_idx = it->first;

    if ( seg_idx > next_index_ )
      break;

    std::string &seg = it->second;

    if ( seg_idx < next_index_ ) {
      seg.erase( 0, static_cast<size_t>( next_index_ - seg_idx ) );
      first_index = next_index_;
    }

    uint64_t can_push = min<uint64_t>( seg.size(), output_.writer().available_capacity() );
    if ( can_push == 0 )
      break;

    output_.writer().push( seg.substr( 0, static_cast<size_t>( can_push ) ) );
    next_index_ += can_push;

    if ( can_push == seg.size() ) {
      unassembled_.erase( it );
    } else {
      std::string remaining = seg.substr( static_cast<size_t>( can_push ) );
      unassembled_.erase( it );
      unassembled_.emplace( next_index_, std::move( remaining ) );
      if ( output_.writer().available_capacity() == 0 )
        break;
    }
  }

  // Close when EOF reached
  if ( eof_received_ && next_index_ >= eof_index_ )
    output_.writer().close();
}

uint64_t Reassembler::count_bytes_pending() const
{
  uint64_t total = 0;
  for ( const auto &kv : unassembled_ )
    total += static_cast<uint64_t>( kv.second.size() );
  return total;
}
