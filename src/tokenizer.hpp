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
#include <vector>

namespace platypus {

class Tokenizer {

    public:
        Tokenizer(
            const std::string & uncaptured_delimiters,
            const std::string & captured_delimiters,
            const std::string & quote_chars,
            const std::string & esc_chars,
            const std::string & comment_begin,
            const std::string & comment_end,
            bool capture_comments=true);

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
                        const std::string & esc_chars,
                        const std::string & comment_begin,
                        const std::string & comment_end,
                        bool capture_comments);
			    iterator(const std::string & str,
                        const std::string & uncaptured_delimiters,
                        const std::string & captured_delimiters,
                        const std::string & quote_chars,
                        const std::string & esc_chars,
                        const std::string & comment_begin,
                        const std::string & comment_end,
                        bool capture_comments);
                iterator();
                ~iterator();
                // const reference operator*() const;
                // const pointer operator->() const;
                reference operator*();
                pointer operator->();
                bool operator==(const self_type& rhs) const;
                bool operator!=(const self_type& rhs) const;
                const self_type & operator++();
                self_type operator++(int);
                bool eof();
                bool token_is_quoted();
                bool token_has_comments();
                std::vector<std::string>& captured_comments();
                void clear_captured_comments();

            protected:

                value_type & get_next_token();
                void skip_to_next_significant_char();
                void handle_comment();
                bool is_uncaptured_delimiter();
                bool is_captured_delimiter(const std::string & s);
                bool is_captured_delimiter();
                bool is_quote_char();
                bool is_comment_begin();
                bool is_comment_end();
                int get_next_char();

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

        iterator begin(std::istream & src);
        iterator begin(const std::string & str);
        iterator end();

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
