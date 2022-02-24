#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    // notice that n is a 64-bit unsigned integer
    // however, we can only use 32-bit unsigned integers in the implementation
    // so we need to convert n to a 32-bit unsigned integer
    return isn + static_cast<uint32_t>(n);
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    const constexpr uint64_t UINT32_RANGE = 0x100000000;
    // uint64_t absolute_seqno = static_cast<uint64_t>(n - isn);
    // I don't know why the above line doesn't work
    // simply use uint32_t instead
    uint32_t absolute_seqno = n - isn;
    if (checkpoint <= absolute_seqno) {
        // no need to round
        return static_cast<uint64_t>(absolute_seqno);
    } else {
        // rounding
        uint64_t real_checkpoint = (checkpoint - absolute_seqno) + (UINT32_RANGE >> 1);
        uint64_t real_absolute_seqno = absolute_seqno + (real_checkpoint / UINT32_RANGE) * UINT32_RANGE;
        return real_absolute_seqno;
    }
}
