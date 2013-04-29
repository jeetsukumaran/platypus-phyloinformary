#include <iomanip>
#include "tests.hpp"

namespace platypus { namespace test {

//////////////////////////////////////////////////////////////////////////////
// BasicTree

BasicTree::BasicTree() {
}

BasicTree::~BasicTree() {
}

//////////////////////////////////////////////////////////////////////////////
// TestDataTree

std::ostream& operator<<(std::ostream& stream, const TestData& data) {
    if (!data.label_.empty()) {
        stream << data.label_;
    }
    stream << ":" << std::setprecision(4) << data.edge_length_;
    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// General String Support/Utility

std::string& ltrim(std::string& str, const std::string& chars) {
    size_t start_pos = str.find_first_not_of(chars);
    if (start_pos != std::string::npos) {
        str = str.substr(start_pos);
    } else {
        str.clear();
    }
    return str;
}

std::string& rtrim(std::string& str, const std::string& chars) {
    size_t end_pos = str.find_last_not_of(chars);
    if (end_pos != std::string::npos) {
        str = str.substr(0, end_pos + 1);
    } else {
        str.clear();
    }
    return str;
}

std::string& trim(std::string& str, const std::string& chars) {
    return rtrim(ltrim(str, chars), chars);
}

std::string& stripspaces(std::string& str) {
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

std::string& uppercase(std::string& str) {
    std::transform(str.begin(), str.end(),str.begin(), ::toupper);
    return str;
}

std::string& lowercase(std::string& str) {
    std::transform(str.begin(), str.end(),str.begin(), ::tolower);
    return str;
}

std::vector<std::string>& split(const std::string& str,
        const std::string& delimiter,
        std::vector<std::string>& tokens,
        bool trim_tokens,
        bool include_empty) {

    std::string token;
    size_t start_pos = 0;
    size_t end_pos = 0;
    size_t str_len = str.size();
    size_t delimiter_len = delimiter.size();

    while (start_pos < str_len && start_pos != std::string::npos) {
        end_pos = str.find(delimiter, start_pos);
        token = str.substr(start_pos, end_pos-start_pos);
        if (trim_tokens) {
            trim(token);
        }
        if (include_empty || !token.empty()) {
            tokens.push_back(token);
        }
        if (end_pos == std::string::npos) {
            break;
        }
        start_pos = end_pos + delimiter_len;
    }
    return tokens;
}

std::vector<std::string> split(const std::string& str,
        const std::string& delimiter,
        bool trim_tokens,
        bool include_empty) {
    std::vector<std::string> tokens;
    return split(str, delimiter, tokens, trim_tokens, include_empty);
}

//////////////////////////////////////////////////////////////////////////////
// Tokenizer

platypus::Tokenizer get_nexus_tokenizer() {
    return platypus::Tokenizer(
            " \t\n\r",      // uncaptured delimiters
            "(),;:",        // quote_chars
            "\"'",          // quote_chars
            true,           // esc_quote_chars_by_doubling
            "",             // esc_chars
            "[",            // comment_begin
            "]",            // comment_end
            true            // capture_comments
            );
}


//////////////////////////////////////////////////////////////////////////////
// Checking/Verification

int compare_token_vectors(
        const std::vector<std::string> & expected,
        const std::vector<std::string> & observed,
        const std::string & test_title,
        unsigned long line_num) {
    if (expected != observed) {
        std::cerr << "\n[" << test_title  << ": " << line_num << "]" << std::endl;
        std::cerr << "Expected:\n";
        for (auto & s : expected) {
            std::cerr << " '" << s << "'\n";
        }
        // std::copy(expected.begin(), expected.end(), std::ostream_iterator<std::string>(std::cerr, ", "));
        std::cerr << std::endl;
        std::cerr << "Received:\n";
        // std::copy(observed.begin(), observed.end(), std::ostream_iterator<std::string>(std::cerr, ", "));
        for (auto & s : observed) {
            std::cerr << " '" << s << "'\n";
        }
        std::cerr << std::endl;
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}


int validate_standard_test_tree(TestDataTree & tree) {
    int has_failed = 0;

    // postorder
    std::vector<std::string> postorder_visits;
    for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
        postorder_visits.push_back(ndi->get_label());
    }
    has_failed += check_equal(
            STANDARD_TEST_TREE_POSTORDER,
            postorder_visits,
            __FILE__,
            __LINE__);

    // preorder
    std::vector<std::string> preorder_visits;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        preorder_visits.push_back(ndi->get_label());
    }
    has_failed += check_equal(
            STANDARD_TEST_TREE_PREORDER,
            preorder_visits,
            __FILE__,
            __LINE__);

    // leaves
    std::vector<std::string> leaves;
    for (auto ndi = tree.leaf_begin(); ndi != tree.leaf_end(); ++ndi) {
        leaves.push_back(ndi->get_label());
    }
    has_failed += check_equal(
            STANDARD_TEST_TREE_LEAVES,
            leaves,
            __FILE__,
            __LINE__);

    // children
    std::map<std::string, std::vector<std::string>> children;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        for (auto chi = tree.children_begin(ndi); chi != tree.children_end(); ++chi) {
            children[ndi->get_label()].push_back(chi->get_label());
        }
        auto expected_iter = STANDARD_TEST_TREE_CHILDREN.find(ndi->get_label());
        assert(expected_iter != STANDARD_TEST_TREE_CHILDREN.end());
        auto expected_children = expected_iter->second;
        has_failed += check_equal(
                expected_children,
                children[ndi->get_label()],
                __FILE__,
                __LINE__,
                "Parent node: ", *ndi);
    }

    // siblings
    std::map<std::string, std::vector<std::string>> siblings;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        if (ndi == tree.begin()) {
            // skip root
            continue;
        }
        for (auto sib = tree.next_sibling_begin(ndi); sib != tree.next_sibling_end(); ++sib) {
            siblings[ndi->get_label()].push_back(sib->get_label());
        }
        auto expected_iter = STANDARD_TEST_TREE_SIBLINGS.find(ndi->get_label());
        assert(expected_iter != STANDARD_TEST_TREE_SIBLINGS.end());
        auto expected_siblings = expected_iter->second;
        has_failed += check_equal(
                expected_siblings,
                siblings[ndi->get_label()],
                __FILE__,
                __LINE__,
                "Start node: ", *ndi);
    }

    return has_failed;
}

} } // namespace platypus::test
