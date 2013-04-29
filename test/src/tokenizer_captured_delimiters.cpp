#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "tests.hpp"

using namespace platypus::test;

int main() {
    std::string str = "(aaa:1.00,     (b:2.18e-1,      (ccc:11, d:1e-1)   k:  3)  u:   7)    rrr:0.0;";
    // std::cout << "\n[" << str << "]" << std::endl;
    std::istringstream s(str);
    platypus::Tokenizer tokenizer = get_nexus_tokenizer();
    std::vector<std::string> observed;
    for (auto iter = tokenizer.begin(s); iter != tokenizer.end(); ++iter) {
        // std::cout << "'" << *iter << "'" << std::endl;
        observed.push_back(*iter);
    }
    std::vector<std::string> expected{
            "(",
            "aaa",
            ":",
            "1.00",
            ",",
            "(",
            "b",
            ":",
            "2.18e-1",
            ",",
            "(",
            "ccc",
            ":",
            "11",
            ",",
            "d",
            ":",
            "1e-1",
            ")",
            "k",
            ":",
            "3",
            ")",
            "u",
            ":",
            "7",
            ")",
            "rrr",
            ":",
            "0.0",
            ";"
            };
    return compare_token_vectors(expected, observed, __FILE__, __LINE__);
}
