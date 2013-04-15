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

#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


namespace platypus {

class Tokenizer {

    public:
        Tokenizer(
            const std::string & uncaptured_delimiters = " \t\n\r",
            const std::string & captured_delimiters = "(),;:",
            const std::string & quote_chars = "\"'",
            const std::string & esc_chars = "\\",
            const std::string & comment_begin = "[",
            const std::string & comment_end = "]",
            bool capture_comments=true)
            : uncaptured_delimiters_(uncaptured_delimiters)
              , captured_delimiters_(captured_delimiters)
              , quote_chars_(quote_chars)
              , esc_chars_(esc_chars)
              , comment_begin_(comment_begin)
              , comment_end_(comment_end)
              , capture_comments_(capture_comments) {
        }

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
			    iterator(const std::string & str,
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
                iterator()
                    : allocated_src_ptr_(nullptr)
                      , src_ptr_(nullptr)
                      , esc_quote_chars_by_doubling(true) {
                }
                ~iterator() {
                    if (this->allocated_src_ptr_ != nullptr) {
                        delete this->allocated_src_ptr_;
                        this->allocated_src_ptr_ = nullptr;
                    }
                }
                // const reference operator*() const {
                //     return this->token_;
                // }
                // const pointer operator->() const {
                //     return &(this->token_);
                // }
                reference operator*() {
                    return this->token_;
                }
                pointer operator->() {
                    return &(this->token_);
                }
                bool operator==(const self_type& rhs) const {
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
                bool operator!=(const self_type& rhs) const {
                    return !(*this == rhs);
                }
                const self_type & operator++() {
                    if (!this->src_ptr_->good()) {
                        this->src_ptr_ = nullptr;
                    }
                    if (this->src_ptr_ != nullptr) {
                        this->get_next_token();
                    }
                    return *this;
                }
                self_type operator++(int) {
                    self_type i = *this;
                    ++(*this);
                    return i;
                }
                bool eof() {
                    return (this->src_ptr_ == nullptr) || (!this->src_ptr_->good());
                }
                bool token_is_quoted() {
                    return this->token_is_quoted_;
                }
                bool token_has_comments() {
                    return !this->captured_comments_.empty();
                }
                std::vector<std::string>& captured_comments() {
                    return this->captured_comments_;
                }
                void clear_captured_comments() {
                    this->captured_comments_.clear();
                }

            protected:

                value_type & get_next_token() {
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

                void skip_to_next_significant_char() {
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

                void handle_comment() {
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

                bool is_uncaptured_delimiter() {
                    return this->uncaptured_delimiters_.find(this->cur_char_) != std::string::npos;
                }

                bool is_captured_delimiter(const std::string & s) {
                    return this->captured_delimiters_.find(s) != std::string::npos;
                }

                bool is_captured_delimiter() {
                    return this->captured_delimiters_.find(this->cur_char_) != std::string::npos;
                }

                bool is_quote_char() {
                    return this->quote_chars_.find(this->cur_char_) != std::string::npos;
                }

                bool is_comment_begin() {
                    return this->comment_begin_.find(this->cur_char_) != std::string::npos;
                }

                bool is_comment_end() {
                    return this->comment_end_.find(this->cur_char_) != std::string::npos;
                }

                int get_next_char() {
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
                bool                        esc_quote_chars_by_doubling;
                std::string                 esc_chars_;
                std::string                 comment_begin_;
                std::string                 comment_end_;
                bool                        capture_comments_;

                // local storage
                std::string                 token_;
                int                         cur_char_;
                bool                        token_is_quoted_;
                std::vector<std::string>    captured_comments_;

        }; // iterator

        iterator begin(std::istream & src) {
            return iterator(src,
                this->uncaptured_delimiters_,
                this->captured_delimiters_,
                this->quote_chars_,
                this->esc_chars_,
                this->comment_begin_,
                this->comment_end_,
                this->capture_comments_);
        }

        iterator begin(const std::string & str) {
            return iterator(str,
                this->uncaptured_delimiters_,
                this->captured_delimiters_,
                this->quote_chars_,
                this->esc_chars_,
                this->comment_begin_,
                this->comment_end_,
                this->capture_comments_);
        }

        iterator end() {
            return iterator();
        }

    private:
        std::string     uncaptured_delimiters_;
        std::string     captured_delimiters_;
        std::string     quote_chars_;
        std::string     esc_chars_;
        std::string     comment_begin_;
        std::string     comment_end_;
        bool            capture_comments_;
}; // Tokenizer

} // namespace platypus

#endif
