#include "stream_reassembler.hh"
#include <algorithm>
#include <iostream>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

int StreamReassembler::_merge_substring(SubString& s1, const SubString& s2) {
    SubString element1, element2;
    if (s1.start_index < s2.start_index) {
        element1 = s1;
        element2 = s2;
    } else {
        element1 = s2;
        element2 = s1;
    }
    if (element1.start_index + element1.length < element2.start_index) {
        // no intersection, can not merge
        return -1;
    }
    else if (element1.start_index + element1.length >= element2.start_index + element2.length) {
        // element2 is contained in element1, merge element2 into element1
        s1 = element1;
        return element2.length;
    }
    else {
        // overlap
        s1.start_index = element1.start_index;
        s1.data = element1.data + element2.data.substr(element1.start_index + element1.length - element2.start_index);
        s1.length = s1.data.length();
        return element1.start_index + element1.length - element2.start_index;
    }
}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (index >= this->_head_index + this->_capacity) {
        // out of range
        return;
    }
    SubString sub;
    if (index + data.length() <= this->_head_index) {
        if (eof) {
            this->_eof_flag = true;
        }
        if (this->_eof_flag && this->_unassembled_bytes == 0) {
            this->_output.end_input();
        }
        // dup data, simply drop it
        return;
    }
    else if (index < this->_head_index) {
        // has intersection, need to merge
        // get the intersection
        size_t offset = this->_head_index - index;
        sub.data = data.substr(offset);
        sub.start_index = index + offset;
        sub.length = sub.data.length();
    }
    else {
        // no intersection, just copy
        sub.data = data;
        sub.start_index = index;
        sub.length = sub.data.length();
    }
    this->_unassembled_bytes += sub.length;
        std::cout << "unassembled bytes: " << this->_unassembled_bytes << ". writen data " << sub.data << " with length " << sub.length << std::endl;
    
    // merge substring
    int merged_size = 0;
    // for (auto iter = this->_substrings.begin(); iter != this->_substrings.end(); ++iter) {
    //     SubString s = *iter;
    //     merged_size = _merge_substring(sub, *iter);
    //     if (merged_size >= 0) {
    //         // merged 
    //         this->_unassembled_bytes -= merged_size;
    //         iter = this->_substrings.erase(iter);
    //     }
    //     else {
    //         break;
    //     }
    // }
    auto iter = this->_substrings.lower_bound(sub);
    while (iter != this->_substrings.end() && (merged_size = _merge_substring(sub, *iter)) >= 0) {
        this->_unassembled_bytes -= merged_size;
        std::cout << "unassembled bytes: " << this->_unassembled_bytes << ". writen data " << sub.data << " with length " << sub.length << std::endl;
        this->_substrings.erase(iter);
        iter = this->_substrings.lower_bound(sub);
    }
    iter = this->_substrings.upper_bound(sub);
    if (iter != this->_substrings.begin()) {
        --iter;
        while ((merged_size = this->_merge_substring(sub, *iter)) >= 0) {
            this->_unassembled_bytes -= merged_size;
        std::cout << "unassembled bytes: " << this->_unassembled_bytes << ". writen data " << sub.data << " with length " << sub.length << std::endl;
            this->_substrings.erase(iter);
            iter = this->_substrings.lower_bound(sub);
            if (iter == this->_substrings.begin()) {
                break;
            }
            --iter;
        }
    }
    // insert substring
    this->_substrings.insert(sub);
    // write to output
    if (!this->_substrings.empty() && this->_substrings.begin()->start_index == this->_head_index) {
        // write to output
        SubString s = *(this->_substrings.begin());
        size_t writen_bytes = this->_output.write(s.data);
        this->_head_index += writen_bytes;
        this->_unassembled_bytes -= writen_bytes;
        std::cout << "unassembled bytes: " << this->_unassembled_bytes << ". writen data " << s.data << " with length " << s.length << std::endl;
        this->_substrings.erase(this->_substrings.begin());
    }
    // check eof
    if (eof) {
        this->_eof_flag = true;
    }
    if (this->_eof_flag && this->_unassembled_bytes == 0) {
        this->_output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return this->_unassembled_bytes; }

bool StreamReassembler::empty() const { return this->_unassembled_bytes == 0; }
