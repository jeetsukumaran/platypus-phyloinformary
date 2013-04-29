#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main() {
    std::string str = "[&R] (foo:1 [a foo object], [start of subgroup](bar:2, c:2)[end of group][][][";
    // std::cout << "\n[" << str << "]" << std::endl;
    std::istringstream s(str);
    platypus::Tokenizer tokenizer = get_nexus_tokenizer();
    std::vector<std::string> observed;
    for (auto iter = tokenizer.begin(s); iter != tokenizer.end(); ++iter) {
        // std::cout << "'" << *iter << "'" << std::endl;
        observed.push_back(*iter);
    }
    std::vector<std::string> expected{
            "(", "foo", ":","1", ",", "(", "bar", ":", "2",  ",", "c", ":", "2", ")"
            };
    return compare_token_vectors(expected, observed, __FILE__, __LINE__);
}
