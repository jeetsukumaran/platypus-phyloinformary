/**
 * @package     platypus-phyloinformary
 * @brief       A two-dimensional container with columns of varying
 *              (signed integer, unsigned integer, real, or string) types.
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
 */

#ifndef PLATYPUS_UTILITY_STREAM_HPP
#define PLATYPUS_UTILITY_STREAM_HPP

#include <memory>
#include <iostream>
#include <iomanip>
#include <vector>

namespace platypus { namespace stream {

//////////////////////////////////////////////////////////////////////////////
// OutputStreamManipulators

// from:: http://stackoverflow.com/questions/14702629/can-you-pass-a-manipulator-to-a-function

// an abstract class to provide a common interface to all manipulators
class abstrac_ostream_manipulator {
    public:
        virtual ~abstrac_ostream_manipulator() { }
        virtual void apply(std::ostream& out) const = 0;
};

// a wrapper template to let arbitrary manipulators follow that interface
template<typename M> class concrete_ostream_manipulator : public abstrac_ostream_manipulator {
    public:
        concrete_ostream_manipulator(const M& manip) : _manip(manip) { }
        void apply(std::ostream& out) const { out << _manip; }
    private:
        M _manip;
};

// a class to hide the memory management required for polymorphism
class ostream_manipulator {
    public:
        template<typename M> ostream_manipulator(const M& manip)
            : _manip(new concrete_ostream_manipulator<M>(manip)) { }
        template<typename R, typename A> ostream_manipulator(R (&manip)(A))
            : _manip(new concrete_ostream_manipulator<R (*)(A)>(&manip)) { }
        void apply(std::ostream& out) const { _manip->apply(out); }
    private:
        std::shared_ptr<abstrac_ostream_manipulator> _manip;
};

inline std::ostream& operator<<(std::ostream& out, const ostream_manipulator& manip) {
    manip.apply(out);
    return out;
}

typedef ostream_manipulator                     OutputStreamFormatter;
typedef std::vector<OutputStreamFormatter>      OutputStreamFormatters;

} } // namespace platypus::stream

#endif
