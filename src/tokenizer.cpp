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

#include "tokenizer.hpp"

namespace platypus {

//////////////////////////////////////////////////////////////////////////////
// Tokenizer

Tokenizer::Tokenizer(
    const std::string & uncaptured_delimiters,
    const std::string & captured_delimiters,
    const std::string & quote_chars,
    const std::string & esc_chars,
    const std::string & comment_begin,
    const std::string & comment_end,
    bool capture_comments)
    : uncaptured_delimiters_(uncaptured_delimiters)
    , captured_delimiters_(captured_delimiters)
    , quote_chars_(quote_chars)
    , esc_chars_(esc_chars)
    , comment_begin_(comment_begin)
    , comment_end_(comment_end)
    , capture_comments_(capture_comments) {
}

Tokenizer::iterator Tokenizer::begin(std::istream & src) {
    return Tokenizer::iterator(src,
        this->uncaptured_delimiters_,
        this->captured_delimiters_,
        this->quote_chars_,
        this->esc_chars_,
        this->comment_begin_,
        this->comment_end_,
        this->capture_comments_);
}

Tokenizer::iterator Tokenizer::begin(const std::string & str) {
    return Tokenizer::iterator(str,
        this->uncaptured_delimiters_,
        this->captured_delimiters_,
        this->quote_chars_,
        this->esc_chars_,
        this->comment_begin_,
        this->comment_end_,
        this->capture_comments_);
}

Tokenizer::iterator Tokenizer::end() {
    return Tokenizer::iterator();
}

//////////////////////////////////////////////////////////////////////////////
// Tokenizer::iterator

Tokenizer::iterator::iterator(std::istream & src,
        const std::string & uncaptured_delimiters,
        const std::string & captured_delimiters,
        const std::string & quote_chars,
        const std::string & esc_chars,
        const std::string & comment_begin,
        const std::string & comment_end,
        bool capture_comments)
        : allocated_src_ptr_(nullptr)
            , src_ptr_(&src)
            , uncaptured_delimiters_(uncaptured_delimiters)
            , captured_delimiters_(captured_delimiters)
            , quote_chars_(quote_chars)
            , esc_quote_chars_by_doubling(true)
            , esc_chars_(esc_chars)
            , comment_begin_(comment_begin)
            , comment_end_(comment_end)
            , capture_comments_(capture_comments)
            , cur_char_(0) {
    this->get_next_token();
}

Tokenizer::iterator::iterator(const std::string & str,
        const std::string & uncaptured_delimiters,
        const std::string & captured_delimiters,
        const std::string & quote_chars,
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
            , esc_quote_chars_by_doubling(true)
            , esc_chars_(esc_chars)
            , comment_begin_(comment_begin)
            , comment_end_(comment_end)
            , capture_comments_(capture_comments)
            , cur_char_(0) {
    this->get_next_token();
}

Tokenizer::iterator::iterator()
    : allocated_src_ptr_(nullptr)
        , src_ptr_(nullptr)
        , esc_quote_chars_by_doubling(true) {
}

Tokenizer::iterator::~iterator() {
    if (this->allocated_src_ptr_ != nullptr) {
        delete this->allocated_src_ptr_;
        this->allocated_src_ptr_ = nullptr;
    }
}

Tokenizer::iterator::reference Tokenizer::iterator::operator*() {
    return this->token_;
}

Tokenizer::iterator::pointer Tokenizer::iterator::operator->() {
    return &(this->token_);
}

bool Tokenizer::iterator::operator==(const Tokenizer::iterator::self_type& rhs) const {
    if ( this->src_ptr_ == rhs.src_ptr_) {
        return true;
    // } else if (this->src_ptr_ == nullptr && (rhs.src_ptr_ != nullptr && !rhs.src_ptr_->good())) {
    //     return true;
    // } else if (rhs.src_ptr_ == nullptr && (this->src_ptr_ != nullptr && !this->src_ptr_->good())) {
    //     return true;
    } else if (this->src_ptr_ && rhs.src_ptr_) {
        return *(this->src_ptr_) == *(rhs.src_ptr_);
    } else {
        return false;
    }
}

bool Tokenizer::iterator::operator!=(const Tokenizer::iterator::self_type& rhs) const {
    return !(*this == rhs);
}

const Tokenizer::iterator::self_type & Tokenizer::iterator::operator++() {
    if (!this->src_ptr_->good()) {
        this->src_ptr_ = nullptr;
    }
    if (this->src_ptr_ != nullptr) {
        this->get_next_token();
    }
    return *this;
}

Tokenizer::iterator::self_type Tokenizer::iterator::operator++(int) {
    self_type i = *this;
    ++(*this);
    return i;
}

bool Tokenizer::iterator::eof() {
    return (this->src_ptr_ == nullptr) || (!this->src_ptr_->good());
}

bool Tokenizer::iterator::token_is_quoted() {
    return this->token_is_quoted_;
}

bool Tokenizer::iterator::token_has_comments() {
    return !this->captured_comments_.empty();
}

std::vector<std::string>& Tokenizer::iterator::captured_comments() {
    return this->captured_comments_;
}

void Tokenizer::iterator::clear_captured_comments() {
    this->captured_comments_.clear();
}

Tokenizer::iterator::value_type & Tokenizer::iterator::get_next_token() {
    assert(this->src_ptr_ != nullptr);
    this->token_is_quoted_ = false;
    auto & src = *(this->src_ptr_);
    this->skip_to_next_significant_char();
    if (this->cur_char_ == EOF && !src.good()) {
        this->src_ptr_ = nullptr;
        this->token_.clear();
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
            // TODO! unterminated quote
            this->src_ptr_ = nullptr;
            this->token_.clear();
            return this->token_;
        }
        this->get_next_char();
        while (true) {
            if (!src.good()) {
                // TODO! unterminated quote
                this->src_ptr_ = nullptr;
                this->token_.clear();
                return this->token_;
            }
            if (this->cur_char_ == cur_quote_char) {
                this->get_next_char();
                if (this->esc_quote_chars_by_doubling) {
                    if (this->cur_char_ == cur_quote_char) {
                        dest.put(cur_quote_char);
                        this->get_next_char();
                    } else {
                        this->get_next_char();
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
        if (this->token_.empty() && src.good()) {
            this->get_next_token();
        }
        return this->token_;
    }
}

void Tokenizer::iterator::skip_to_next_significant_char() {
    auto & src = *(this->src_ptr_);
    if (! src.good() ) {
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
}

void Tokenizer::iterator::handle_comment() {
    auto & src = *(this->src_ptr_);
    std::ostringstream dest;
    unsigned int nesting = 0;
    while (src.good()) {
        if ( this->is_comment_end() ) {
            nesting -= 1;
            if (nesting <= 0) {
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
    if (this->capture_comments_) {
        this->captured_comments_.push_back(dest.str());
    }
}

bool Tokenizer::iterator::is_uncaptured_delimiter() {
    return this->uncaptured_delimiters_.find(this->cur_char_) != std::string::npos;
}

bool Tokenizer::iterator::is_captured_delimiter(const std::string & s) {
    return this->captured_delimiters_.find(s) != std::string::npos;
}

bool Tokenizer::iterator::is_captured_delimiter() {
    return this->captured_delimiters_.find(this->cur_char_) != std::string::npos;
}

bool Tokenizer::iterator::is_quote_char() {
    return this->quote_chars_.find(this->cur_char_) != std::string::npos;
}

bool Tokenizer::iterator::is_comment_begin() {
    return this->comment_begin_.find(this->cur_char_) != std::string::npos;
}

bool Tokenizer::iterator::is_comment_end() {
    return this->comment_end_.find(this->cur_char_) != std::string::npos;
}

int Tokenizer::iterator::get_next_char() {
    this->cur_char_ = this->src_ptr_->get();
    return this->cur_char_;
}

} // platypus
