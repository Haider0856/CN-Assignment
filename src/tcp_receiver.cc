// src/tcp_receiver.cc
#include "tcp_receiver.hh"

#include <algorithm>
#include <cstdint>
#include <limits>

using namespace std;

void TCPReceiver::receive(TCPSenderMessage message) {
    // Handle RST: mark the stream as errored.
    if (message.RST) {
        reassembler_.writer().set_error();
        return;
    }

    // If SYN present and not recorded yet, set it.
    if (message.SYN && !syn_received_) {
        syn_received_ = true;
        isn_ = message.seqno;
    }

    // If SYN not received yet, ignore everything else.
    if (!syn_received_)
        return;

    // Unwrap seqno relative to ISN and checkpoint.
    uint64_t abs_seqno = message.seqno.unwrap(isn_, checkpoint_);
    checkpoint_ = abs_seqno;

    // If this segment arrives *before* the SYN (invalid index), ignore.
    // The first valid absolute sequence number is ISN (SYN) → next data is ISN + 1.
    if (abs_seqno == 0 && !message.SYN)
        return;

    // Compute stream index (position in logical data stream)
    uint64_t stream_index = abs_seqno - 1;
    if (message.SYN)
        stream_index = 0; // Data after SYN starts at index 0

    // Insert the payload; FIN flag marks stream end
    reassembler_.insert(stream_index, message.payload, message.FIN);
}

TCPReceiverMessage TCPReceiver::make_send() const {
    TCPReceiverMessage out{};

    // Advertise available window space (limited to uint16_t)
    out.window_size = static_cast<uint16_t>(
        min<uint64_t>(reassembler_.writer().available_capacity(), static_cast<uint64_t>(UINT16_MAX)));

    // No SYN yet → no ackno
    if (!syn_received_) {
        out.ackno = nullopt;
        out.RST = reassembler_.writer().has_error();
        return out;
    }

    // Base ACK = bytes_pushed() + 1 (SYN consumes one sequence number)
    uint64_t ack_abs = reassembler_.writer().bytes_pushed() + 1;

    // If FIN has been fully written to output, add +1 more
    if (reassembler_.writer().is_closed())
        ack_abs += 1;

    // Wrap into 32-bit
    out.ackno = Wrap32::wrap(ack_abs, isn_);
    out.RST = reassembler_.writer().has_error();

    return out;
}

TCPReceiverMessage TCPReceiver::send() const {
    return make_send();
}
