#include "wrapping_integers.hh"

#include <iostream>

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn)
{
    uint64_t isn64 = static_cast<uint64_t>(isn.raw_value());
    uint64_t sum = n + isn64;
    uint32_t seqNo = static_cast<uint32_t>(isn64 + n % (static_cast<uint64_t>(1) << 32));
    return WrappingInt32(seqNo);
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

uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint)
{
    WrappingInt32 checkpointWrap = wrap(checkpoint, isn);
    uint32_t diff = n - checkpointWrap;
    uint32_t isn32 = isn.raw_value();
    uint64_t absSeq = static_cast<uint64_t>(diff) + checkpoint;
    uint32_t absSeq32 = static_cast<uint32_t>(absSeq);
    if (diff >= (1ul << 31) && absSeq >= (1ul << 32))
    {
        absSeq32 -= 1ul << 32;
        absSeq -= 1ul << 32;
        //the absolute sequence number is closest to checkpoint
    }
    if(absSeq32 < isn32)
        absSeq32 += 1ul << 32;
    
    return absSeq;
}
