#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <string>
#include <set>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.
    
    // Our final goal is to assemble a seires of contiguous substrings from the input stream which may be out of order.
    // The key point is to merge the substrings into some larger contiguous strings.
    // Since we will continuously update the structure of the substrings, 
    // we should use a effective data structure to store the substrings 
    // which should support efficient insertion and deletion, should be in order, and should be easy to find a substring.
    // Thus, we can choose heap or RB Tree to implement the data structure.
    // In this lab, we will use STL set which uses RB Tree to implement the data structure.

    struct SubString {
      size_t start_index = 0;
      size_t length = 0;
      std::string data = "";
      bool operator<(const SubString &other) const {
        return start_index < other.start_index;
      }
    };
    std::set<SubString> _substrings = {};
    size_t _unassembled_bytes = 0;
    size_t _head_index = 0;                 // the index of the first byte in the stream window
    bool _eof_flag = false;
    int _merge_substring(SubString &s1, const SubString &s2);
    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
