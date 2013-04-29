#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main() {
    std::string str = "";
    std::istringstream s(str);
    platypus::Tokenizer tokenizer = get_nexus_tokenizer();
    std::vector<std::string> observed;
    for (auto iter = tokenizer.begin(s); iter != tokenizer.end(); ++iter) {
        observed.push_back(*iter);
    }
    std::vector<std::string> expected;
    return compare_token_vectors(expected, observed, __FILE__, __LINE__);
}
