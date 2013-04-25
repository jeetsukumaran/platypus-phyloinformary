#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "tests.hpp"

int main() {
    std::string str = "the quick 'brown fox''s friend' jumps over the 'lazy dog''s colleague'";
    platypus::Tokenizer tokenizer = get_nexus_tokenizer();
    std::vector<std::string> observed;
    for (auto iter = tokenizer.begin(str); iter != tokenizer.end(); ++iter) {
        observed.push_back(*iter);
    }
    std::vector<std::string> expected{"the", "quick", "brown fox's friend", "jumps", "over", "the", "lazy dog's colleague"};
    auto success = compare_token_vectors(__FILE__, expected, observed);
    if (!success) {
        return 1;
    } else {
        return 0;
    }
}
