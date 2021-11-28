#include "byte_stream.hh"
#include <iostream>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : _buffer(), _capacity(capacity), _written_size(0), _read_size(0), _end_input(false), _error(false)
    {  }

size_t ByteStream::write(const string &data) {
    if (_end_input) {
        _error = true;
        return 0;
    }
    size_t write_size = min(data.length(), _capacity - buffer_size());
    // get data to write
    string write_data = data.substr(0, write_size);
    // insert data into buffer
    _buffer.insert(_buffer.end(), write_data.begin(), write_data.end());
    // update written size
    this->_written_size += write_size;
    return write_size;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t peek_size = min(len, buffer_size());
    string peek_data = string(_buffer.begin(), _buffer.begin() + peek_size);
    return peek_data;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t pop_size = min(len, buffer_size());
    _read_size += pop_size;
    _buffer.erase(_buffer.begin(), _buffer.begin() + pop_size);
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string read_data = peek_output(len);
    pop_output(len);
    return read_data;
}

void ByteStream::end_input() { _end_input = true; }

bool ByteStream::input_ended() const { return _end_input; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return (_end_input && buffer_empty()); }

size_t ByteStream::bytes_written() const { return _written_size; }

size_t ByteStream::bytes_read() const { return _read_size; }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }
