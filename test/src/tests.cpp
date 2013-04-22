
#include <iomanip>
#include "tests.hpp"

//////////////////////////////////////////////////////////////////////////////
// BasicTree

BasicTree::BasicTree() {
}

BasicTree::~BasicTree() {
}

//////////////////////////////////////////////////////////////////////////////
// DataTree

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

bool compare_token_vectors(const std::string & test_title,
        const std::vector<std::string> & expected,
        const std::vector<std::string> & observed) {
    if (expected != observed) {
        std::cerr << "\n[" << test_title << "]" << std::endl;
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
        return false;
    } else {
        return true;
    }
}
