#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "tests.hpp"

int main() {
    std::string str = "the    quick    brown\t\tfox \n  jumps over\t\t\n the    lazy dog";
    platypus::Tokenizer tokenizer = get_nexus_tokenizer();
    std::vector<std::string> observed;
    for (auto iter = tokenizer.begin(str); iter != tokenizer.end(); ++iter) {
        observed.push_back(*iter);
    }
    std::vector<std::string> expected{"the", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"};
    auto success = compare_token_vectors(__FILE__, expected, observed);
    if (!success) {
        return 1;
    } else {
        return 0;
    }
}
