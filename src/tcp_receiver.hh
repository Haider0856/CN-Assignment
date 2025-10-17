#pragma once

#include "byte_stream.hh"
#include "reassembler.hh"
#include "tcp_sender_message.hh"
#include "tcp_receiver_message.hh"
#include "wrapping_integers.hh"

class TCPReceiver {
  private:
    Reassembler reassembler_;
    bool syn_received_{false};
    Wrap32 isn_{0};
    uint64_t checkpoint_{0};

  public:
    // ✅ Constructor that accepts a Reassembler
    explicit TCPReceiver(Reassembler reassembler = Reassembler(ByteStream(65535)))
        : reassembler_(std::move(reassembler)) {}

    // ✅ Accessor methods for the tests
    Reassembler &reassembler() { return reassembler_; }
    const Reassembler &reassembler() const { return reassembler_; }

    Reader &reader() { return reassembler_.reader(); }
    const Reader &reader() const { return reassembler_.reader(); }

    // ✅ Required methods
    void receive(TCPSenderMessage message);
    TCPReceiverMessage send() const;

  private:
    TCPReceiverMessage make_send() const;
};
