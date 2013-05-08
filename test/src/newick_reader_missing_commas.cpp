#include "platypus_testing.hpp"

using namespace platypus::test;

int main () {
    std::string tree_string = "((a, c) (b, d));";
    try {
        auto trees = platypus::test::get_test_data_tree_vector_from_string<TestDataTree>(tree_string);
    } catch (const platypus::NewickReaderMalformedStatementError & e) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

