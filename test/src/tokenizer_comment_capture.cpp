#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "../../src/tokenizer.hpp"
#include "tests.hpp"

int main() {
    std::string str = "([the quick]apple[brown],([fox]banjo,([jumps]cucumber[over the],[really]dogwood)[lazy]eggplant)) rhubarb[dog];";
    platypus::Tokenizer tokenizer = get_nexus_tokenizer();
    std::vector<std::string> observed;
    std::map<std::string, std::vector<std::string>> exp_comments({
            { "apple",          {"the quick", "brown"},},
            { "banjo",          {"fox"},},
            { "cucumber",       {"jumps", "over the"},},
            { "dogwood",        {"really"},},
            { "eggplant",       {"lazy"},},
            { "rhubarb",        {"dog"},},
            });
    bool success = true;
    for (auto iter = tokenizer.begin(str); iter != tokenizer.end(); ++iter) {
        observed.push_back(*iter);
        auto & comments = iter.captured_comments();
        // obs_comments[*iter].insert(comments.begin(), comments.end());
        // if (!comments.empty()) {
        //     std::cout << " ** " << *iter << "                 ";
        //     for (auto & c : comments) {
        //         std::cout << "'" << c << "', ";
        //     }
        //     std::cout << std::endl;
        // }
        if (exp_comments[*iter] != comments) {
            std::cerr << "Failed to match comments for token '" << *iter << "': ";
            std::cerr << "\n  Expecting: ";
            for (auto & c : exp_comments[*iter]) {
                std::cerr << "'" << c << "', ";
            }
            std::cerr << "\n  But found: ";
            for (auto & c : comments) {
                std::cerr << "'" << c << "', ";
            }
            std::cerr << "\n";
            success = false;
        }
        iter.clear_captured_comments();
    }
    std::vector<std::string> expected({
            "(",
            "apple",
            ",",
            "(",
            "banjo",
            ",",
            "(",
            "cucumber",
            ",",
            "dogwood",
            ")",
            "eggplant",
            ")",
            ")",
            "rhubarb",
            ";"
            });
    auto success2 = compare_token_vectors(__FILE__, expected, observed);
    if (!success || !success2) {
        exit(1);
    } else {
        exit(0);
    }
}
