#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "../../src/tokenizer.hpp"
#include "tests.hpp"

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
    std::vector<std::string> expected({
            "(", "foo", ":","1", ",", "(", "bar", ":", "2",  ",", "c", ":", "2", ")"
            });
    auto success = compare_token_vectors(__FILE__, expected, observed);
    if (!success) {
        exit(1);
    } else {
        exit(0);
    }
}
