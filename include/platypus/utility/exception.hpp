/**
 * @package     platypus-phyloinformary
 * @brief       Exception classes and support.
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

#ifndef PLATYPUS_EXCEPTION_HPP
#define PLATYPUS_EXCEPTION_HPP

#include <sstream>

namespace platypus {

/**
 * Base class for exceptions.
 */
class PlatypusException : public std::exception {
    public:
        PlatypusException(
                const std::string & filename,
                unsigned long line_num,
                const std::string & message)
            : filename_(filename)
            , line_num_(line_num)
            , message_(message) {
        }
        virtual ~PlatypusException() throw() { }
        virtual const char * what() const throw() {
            std::ostringstream o;
            o << "File: " << this->filename_ << std::endl;
            o << "Line: " << this->line_num_ << std::endl;
            o << "Error: " << this->message_ << std::endl;
            return o.str().c_str();
        }

    private:
        std::string     filename_;
        unsigned long   line_num_;
        std::string     message_;
};

} // namespace platypus

#endif
