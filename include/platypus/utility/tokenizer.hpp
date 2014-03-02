/**
 * @package     platypus-phyloinformary
 * @brief       Tokenizing of elements from input stream.
 * @author      Jeet Sukumaran
 * @copyright   Copyright (C) 2013 Jeet Sukumaran.
 *              This file is part of "platypus-phyloinformary".
 *              "platypus-phyloinformary" is free software: you can
 *              redistribute it and/or modify it under the terms of the GNU
 *              General Public License as published by the Free Software
 *              Foundation, either version 3 of the License, or (at your
 *              option) any later version.  "platypus-phyloinformary" is
 *              distributed in the hope that it will be useful, but WITHOUT ANY
 *              WARRANTY; without even the implied warranty of MERCHANTABILITY
 *              or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *              Public License for more details.  You should have received a
 *              copy of the GNU General Public License along with
 *              "platypus-phyloinformary".  If not, see
 *              <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PLATYPUS_TOKENIZER_HPP
#define PLATYPUS_TOKENIZER_HPP

#include <stdexcept>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "../base/exception.hpp"

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// Tokenizer Exceptions

class TokenizerException : public PlatypusException {
    public:
        TokenizerException(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : PlatypusException(filename, line_num, message) { }
};

class TokenizerUnterminatedQuoteError : public TokenizerException {
    public:
        TokenizerUnterminatedQuoteError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : TokenizerException(filename, line_num, message) { }
};

class TokenizerUnexpectedEndOfStreamError : public TokenizerException {
    public:
        TokenizerUnexpectedEndOfStreamError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : TokenizerException(filename, line_num, message) { }
};

////////////////////////////////////////////////////////////////////////////////
// Tokenizer
class Tokenizer {

    public:
        Tokenizer(
            const std::string & uncaptured_delimiters,
            const std::string & captured_delimiters,
            const std::string & quote_chars,
            bool esc_quote_chars_by_doubling,
            const std::string & esc_chars,
            const std::string & comment_begin,
            const std::string & comment_end,
            bool capture_comments)
            : uncaptured_delimiters_(uncaptured_delimiters)
            , captured_delimiters_(captured_delimiters)
            , quote_chars_(quote_chars)
            , esc_quote_chars_by_doubling_(esc_quote_chars_by_doubling)
            , esc_chars_(esc_chars)
            , comment_begin_(comment_begin)
            , comment_end_(comment_end)
            , capture_comments_(capture_comments) {
        }

        virtual ~Tokenizer() {}

        class iterator {
            public:
				typedef iterator                    self_type;
				typedef std::string                 value_type;
				typedef value_type *                pointer;
				typedef value_type &                reference;
				typedef unsigned long               size_type;
				typedef int                         difference_type;
				typedef std::forward_iterator_tag   iterator_category;

			public:

			    iterator(std::istream & src,
                        const std::string & uncaptured_delimiters,
                        const std::string & captured_delimiters,
                        const std::string & quote_chars,
                        bool esc_quote_chars_by_doubling,
                        const std::string & esc_chars,
                        const std::string & comment_begin,
                        const std::string & comment_end,
                        bool capture_comments)
                        : allocated_src_ptr_(nullptr)
                            , src_ptr_(&src)
                            , uncaptured_delimiters_(uncaptured_delimiters)
                            , captured_delimiters_(captured_delimiters)
                            , quote_chars_(quote_chars)
                            , esc_quote_chars_by_doubling_(esc_quote_chars_by_doubling)
                            , esc_chars_(esc_chars)
                            , comment_begin_(comment_begin)
                            , comment_end_(comment_end)
                            , capture_comments_(capture_comments)
                            , cur_char_(0)
                            , eof_flag_(false) {
                    this->get_next_token();
                }

			    iterator(const std::string & str,
                        const std::string & uncaptured_delimiters,
                        const std::string & captured_delimiters,
                        const std::string & quote_chars,
                        bool esc_quote_chars_by_doubling,
                        const std::string & esc_chars,
                        const std::string & comment_begin,
                        const std::string & comment_end,
                        bool capture_comments)
                        : src_string_copy_(str)
                            , allocated_src_ptr_(new std::istringstream(src_string_copy_))
                            , src_ptr_(this->allocated_src_ptr_)
                            , uncaptured_delimiters_(uncaptured_delimiters)
                            , captured_delimiters_(captured_delimiters)
                            , quote_chars_(quote_chars)
                            , esc_quote_chars_by_doubling_(esc_quote_chars_by_doubling)
                            , esc_chars_(esc_chars)
                            , comment_begin_(comment_begin)
                            , comment_end_(comment_end)
                            , capture_comments_(capture_comments)
                            , cur_char_(0)
                            , eof_flag_(false) {
                    this->get_next_token();
                }

                iterator()
                    : allocated_src_ptr_(nullptr)
                        , src_ptr_(nullptr)
                        , esc_quote_chars_by_doubling_(true)
                        , eof_flag_(true) {
                }

                ~iterator() {
                    if (this->allocated_src_ptr_ != nullptr) {
                        delete this->allocated_src_ptr_;
                        this->allocated_src_ptr_ = nullptr;
                    }
                }

                inline reference operator*() {
                    return this->token_;
                }

                inline pointer operator->() {
                    return &(this->token_);
                }

                inline bool operator==(const self_type& rhs) const {
                    if (this->eof_flag_ == false && rhs.eof_flag_ == false) {
                        return true;
                    } else if ( this->src_ptr_ == rhs.src_ptr_) {
                        return true;
                    } else if (this->src_ptr_ && rhs.src_ptr_) {
                        return *(this->src_ptr_) == *(rhs.src_ptr_);
                    } else {
                        return false;
                    }
                }

                inline bool operator!=(const self_type& rhs) const {
                    return !(*this == rhs);
                }

                inline const self_type & operator++() {
                    if (!this->src_ptr_ || !this->src_ptr_->good()) {
                        this->set_eof();
                    } else if (this->src_ptr_ != nullptr) {
                        this->get_next_token();
                    }
                    return *this;
                }

                inline const self_type & require_next() {
                    if (!this->src_ptr_->good()) {
                        throw TokenizerUnexpectedEndOfStreamError(__FILE__, __LINE__, "Unexpected end of stream");
                    }
                    this->get_next_token();
                    return *this;
                }

                inline self_type operator++(int) {
                    self_type i = *this;
                    ++(*this);
                    return i;
                }

                inline bool eof() {
                    return this->eof_flag_;
                    if (this->src_ptr_ && !this->src_ptr_->good()) {
                        this->src_ptr_ = nullptr;
                    }
                    // return (this->src_ptr_ == nullptr) || (!this->src_ptr_->good());
                    return (this->src_ptr_ == nullptr);
                }

                inline void set_eof() {
                    this->src_ptr_ = nullptr;
                    this->token_.clear();
                    this->eof_flag_ = true;
                }

                inline bool token_is_quoted() {
                    return this->token_is_quoted_;
                }

                inline bool token_has_comments() {
                    return !this->captured_comments_.empty();
                }

                inline std::vector<std::string>& captured_comments() {
                    return this->captured_comments_;
                }

                inline void clear_captured_comments() {
                    this->captured_comments_.clear();
                }

            protected:

                inline value_type & get_next_token() {
                    assert(this->src_ptr_ != nullptr);
                    this->token_is_quoted_ = false;
                    auto & src = *(this->src_ptr_);
                    this->skip_to_next_significant_char();
                    if (this->cur_char_ == EOF && !src.good()) {
                        this->set_eof();
                        return this->token_;
                    }
                    if (this->is_captured_delimiter()) {
                        this->token_ = this->cur_char_;
                        this->get_next_char();
                        return this->token_;
                    } else if (this->is_quote_char()) {
                        this->token_is_quoted_ = true;
                        std::ostringstream dest;
                        int cur_quote_char = this->cur_char_;
                        if (!src.good()) {
                            throw TokenizerUnterminatedQuoteError(__FILE__, __LINE__, "Unterminated quote");
                            // // TODO! unterminated quote
                            // this->set_eof();
                            // return this->token_;
                        }
                        this->get_next_char();
                        while (true) {
                            if (!src.good()) {
                                throw TokenizerUnterminatedQuoteError(__FILE__, __LINE__, "Unterminated quote");
                                // // TODO! unterminated quote
                                // this->set_eof();
                                // return this->token_;
                            }
                            if (this->cur_char_ == cur_quote_char) {
                                this->get_next_char();
                                if (this->esc_quote_chars_by_doubling_) {
                                    if (this->cur_char_ == cur_quote_char) {
                                        dest.put(cur_quote_char);
                                        this->get_next_char();
                                    } else {
                                        // this->get_next_char();
                                        break;
                                    }
                                } else {
                                    this->get_next_char();
                                    break;
                                }
                            } else {
                                dest.put(this->cur_char_);
                                this->get_next_char();
                            }
                        }
                        this->token_ = dest.str();
                        return this->token_;
                    } else {
                        std::ostringstream dest;
                        this->token_is_quoted_ = false;
                        while (src.good() && this->cur_char_ != EOF) {
                            if (this->is_uncaptured_delimiter() ) {
                                this->get_next_char();
                                break;
                            } else if (this->is_captured_delimiter()) {
                                break;
                            } else if (this->is_comment_begin()) {
                                this->handle_comment();
                                if (!src.good()) {
                                    this->src_ptr_ = nullptr;
                                    break;
                                }
                            } else {
                                dest.put(this->cur_char_);
                                this->get_next_char();
                            }
                        }
                        this->token_ = dest.str();
                        if (this->token_.empty()) {
                            if (src.good()) {
                                this->get_next_token();
                            } else {
                                this->set_eof();
                            }
                        }
                        return this->token_;
                    }
                }

                inline void skip_to_next_significant_char() {
                    auto & src = *(this->src_ptr_);
                    if (! src.good() ) {
                        this->set_eof();
                        return;
                    }
                    if (this->cur_char_ == 0) {
                        this->get_next_char();
                    }
                    if ( !this->is_uncaptured_delimiter() ) {
                        return;
                    }
                    while ( src.good() && this->is_uncaptured_delimiter() ) {
                        this->get_next_char();
                    }
                    if (!src.good() && this->is_uncaptured_delimiter()) {
                        this->set_eof();
                    }
                }

                inline void handle_comment() {
                    auto & src = *(this->src_ptr_);
                    std::ostringstream dest;
                    unsigned int nesting = 0;
                    bool comment_complete = false;
                    while (src.good()) {
                        if ( this->is_comment_end() ) {
                            nesting -= 1;
                            if (nesting <= 0) {
                                comment_complete = true;
                                this->get_next_char();
                                break;
                            }
                        } else if ( this->is_comment_begin() ) {
                            nesting += 1;
                        } else if (this->capture_comments_) {
                            dest.put(this->cur_char_);
                        }
                        this->get_next_char();
                    }
                    if (!src.good() && !comment_complete) {
                        this->set_eof();
                    }
                    if (this->capture_comments_) {
                        this->captured_comments_.push_back(dest.str());
                    }
                }

                inline bool is_uncaptured_delimiter() {
                    return this->uncaptured_delimiters_.find(this->cur_char_) != std::string::npos;
                }

                inline bool is_captured_delimiter(const std::string & s) {
                    return this->captured_delimiters_.find(s) != std::string::npos;
                }

                inline bool is_captured_delimiter() {
                    return this->captured_delimiters_.find(this->cur_char_) != std::string::npos;
                }

                inline bool is_quote_char() {
                    return this->quote_chars_.find(this->cur_char_) != std::string::npos;
                }

                inline bool is_comment_begin() {
                    return this->comment_begin_.find(this->cur_char_) != std::string::npos;
                }

                inline bool is_comment_end() {
                    return this->comment_end_.find(this->cur_char_) != std::string::npos;
                }

                inline int get_next_char() {
                    this->cur_char_ = this->src_ptr_->get();
                    return this->cur_char_;
                }

            protected:
                //  copy of source over lifespan
                std::string                 src_string_copy_;
                std::istream *              allocated_src_ptr_;

                // src stream
                std::istream *              src_ptr_;

                // configuration
                std::string                 uncaptured_delimiters_;
                std::string                 captured_delimiters_;
                std::string                 quote_chars_;
                bool                        esc_quote_chars_by_doubling_;
                std::string                 esc_chars_;
                std::string                 comment_begin_;
                std::string                 comment_end_;
                bool                        capture_comments_;

                // local storage
                std::string                 token_;
                int                         cur_char_;
                bool                        token_is_quoted_;
                std::vector<std::string>    captured_comments_;
                bool                        eof_flag_;

        }; // iterator

        iterator begin(std::istream & src) {
            return Tokenizer::iterator(src,
                this->uncaptured_delimiters_,
                this->captured_delimiters_,
                this->quote_chars_,
                this->esc_quote_chars_by_doubling_,
                this->esc_chars_,
                this->comment_begin_,
                this->comment_end_,
                this->capture_comments_);
        }
        iterator begin(const std::string & str) {
            return Tokenizer::iterator(str,
                this->uncaptured_delimiters_,
                this->captured_delimiters_,
                this->quote_chars_,
                this->esc_quote_chars_by_doubling_,
                this->esc_chars_,
                this->comment_begin_,
                this->comment_end_,
                this->capture_comments_);
        }
        iterator end() {
            return Tokenizer::iterator();
        }

    private:
        std::string     uncaptured_delimiters_;
        std::string     captured_delimiters_;
        std::string     quote_chars_;
        bool            esc_quote_chars_by_doubling_;
        std::string     esc_chars_;
        std::string     comment_begin_;
        std::string     comment_end_;
        bool            capture_comments_;

}; // Tokenizer

////////////////////////////////////////////////////////////////////////////////
// NexusTokenizer

class NexusTokenizer : public Tokenizer {
    public:
        NexusTokenizer()
            : Tokenizer(
                    " \t\n\r",      // uncaptured delimiters
                    "(),;:",        // captured delimiters
                    "\"'",          // quote_chars
                    true,           // esc_quote_chars_by_doubling
                    "",             // esc_chars
                    "[",            // comment_begin
                    "]",            // comment_end
                    true            // capture_comments
                    ) {
        }
}; // NewickTokenizer

} // namespace platypus

#endif
