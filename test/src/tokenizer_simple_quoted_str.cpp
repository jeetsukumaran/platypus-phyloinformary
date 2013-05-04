#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main() {
    std::string str = "the quick 'brown fox' jumps over the 'lazy dog'";
    platypus::Tokenizer tokenizer = get_nexus_tokenizer();
    std::vector<std::string> observed;
    for (auto iter = tokenizer.begin(str); iter != tokenizer.end(); ++iter) {
        observed.push_back(*iter);
    }
    std::vector<std::string> expected{"the", "quick", "brown fox", "jumps", "over", "the", "lazy dog"};
    int fail = compare_token_vectors(expected, observed, __FILE__, __LINE__);
    if (fail == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
