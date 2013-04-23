#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "tests.hpp"

int main() {
    std::string str = "";
    std::istringstream s(str);
    platypus::Tokenizer tokenizer = get_nexus_tokenizer();
    std::vector<std::string> observed;
    for (auto iter = tokenizer.begin(s); iter != tokenizer.end(); ++iter) {
        observed.push_back(*iter);
    }
    std::vector<std::string> expected;
    auto success = compare_token_vectors(__FILE__, expected, observed);
    if (!success) {
        return 1;
    } else {
        return 0;
    }
}
