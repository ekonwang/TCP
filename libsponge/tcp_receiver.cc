#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    auto header = seg.header();
    if (!_syn_flag) {
        // currently in LISTEN state
        if (!header.syn) {
            // not a SYN packet
            // simply drop it
            return;
        }
        // a SYN packet
        // enter SYN_RECV state
        _syn_flag = true;
        // record the initial sequence number
        _isn = header.seqno;
    }
    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;
    uint64_t cur_abs_seqno = unwrap(header.seqno, _isn, abs_ackno);

    uint64_t stream_index = cur_abs_seqno - 1 + header.syn;
    _reassembler.push_substring(seg.payload().copy(), stream_index, header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    // if in LISTEN state, return an empty optional
    if (!_syn_flag) {
        return nullopt;
    }
    // if not in LISTEN state, add one bit for the SYN flag
    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;
    // if in FIN_RECV state, add one bit for the FIN flag
    if (_reassembler.stream_out().input_ended()) {
        abs_ackno++;
    }
    // abs_ackno + _isn represent the ackno that should be sent to the peer
    return _isn + abs_ackno;

}

size_t TCPReceiver::window_size() const { 
    return _capacity - _reassembler.stream_out().buffer_size();
 }
