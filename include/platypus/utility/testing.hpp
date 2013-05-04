/**
 * @package     platypus-phyloinformary
 * @brief       Support for testing.
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

#ifndef PLATYPUS_UTILITY_TESTING_HPP

namespace platypus { namespace testing {

namespace detail {
    // support  for testing functions
    template<class T> void write_container_(std::ostream& out, const T& container, const std::string& separator=", ") {
        std::copy(container.cbegin(), container.cend(), std::ostream_iterator<typename T::value_type>(out, separator.c_str()));
    }
    template<class T> std::string join_container_(const T& container, const std::string& separator=", ") {
        std::ostringstream out;
        write_container_(out, container, separator);
        return out.str();
    }
    template <typename S> void log_(S&) {}
    template <typename S, typename T> void log_(S& stream, const T& arg1) {
        stream << arg1;
    }
    template <typename S, typename T> void log_(S& stream, const std::vector<T>& arg1) {
        write_container_(stream, arg1, ", ");
        log_(stream);
    }
    template <typename S, typename T, typename... Types> void log_(S& stream, const std::vector<T>& arg1, const Types&... args) {
        write_container_(stream, arg1, ", ");
        log_(stream, args...);
    }
    template <typename S, typename T, typename... Types> void log_(S& stream, const T& arg1, const Types&... args) {
        stream << arg1;
        log_(stream, args...);
    }
} // namespace detail

template <typename T, typename U, typename... Types>
int fail_test(const std::string& test_name,
        unsigned long line_num,
        const T& expected,
        const U& observed,
        const Types&... args) {
    std::cerr << "\n||| FAIL |||";
    std::cerr << "\n|     Test: " << test_name;
    std::cerr << "\n|     Line: " << line_num;
    platypus::testing::detail::log_(std::cerr, "\n| Expected: ", expected);
    platypus::testing::detail::log_(std::cerr, "\n| Observed: ", observed);
    platypus::testing::detail::log_(std::cerr, "\n|  Remarks: ", args...);
    std::cerr << std::endl;
    return 1;
}

template <typename T, typename U, typename... Types>
int compare_equal(
        const T& expected,
        const U& observed,
        const std::string& test_name,
        unsigned long line_num,
        const Types&... args) {
    if (expected != observed) {
        return fail_test(test_name, line_num, expected, observed, args...);
    } else {
        return 0;
    }
}

template <typename T>
inline bool is_almost_equal(T a, T b, double tolerance=1e-14) {
    if (a == b) {
        // shortcut, handles infinities
        return true;
    }
    T abs_a = std::fabs(a);
    T abs_b = std::fabs(b);
    T diff = std::fabs(a - b);
    if (abs_a < tolerance && abs_b < tolerance && diff < tolerance) {
        // Hacky, I know. Without this tests fail if, e.g. a = 0 and b =
        // 6.786e-15 ... is this a real failure?
        return true;
    }
    const static T MIN_T_VALUE = std::numeric_limits<T>::min();
    if (a == 0 || b == 0 || diff < MIN_T_VALUE)  {
        // a or b is zero or both are extremely close to it
        // relative error is less meaningful here
        return diff < (tolerance * MIN_T_VALUE);
    }
    // use relative error
    return (diff / (abs_a + abs_b)) < tolerance;
}

template <typename T, typename... Types>
int compare_almost_equal(
        T expected,
        T observed,
        const std::string& test_name,
        unsigned long line_num,
        const Types&... args) {
    if (!is_almost_equal(expected, observed))  {
        return fail_test(test_name,
                line_num,
                expected,
                observed,
                args...);
    } else {
        return 0;
    }
}

} } // namespace platypus::testing

#define PLATYPUS_UTILITY_TESTING_HPP
#endif
