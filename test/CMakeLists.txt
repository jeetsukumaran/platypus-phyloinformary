
SET(TEST_COMMON_SOURCE_FILES
        src/tests.hpp
        src/tests.cpp
   )

ADD_EXECUTABLE(basic_tree
    src/basic_tree.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(basic_tree basic_tree)

ADD_EXECUTABLE(basic_tree_children
    src/basic_tree_children.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(basic_tree_children basic_tree_children)

ADD_EXECUTABLE(basic_tree_deep_copy
    src/basic_tree_deep_copy.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(basic_tree_deep_copy basic_tree_deep_copy)

ADD_EXECUTABLE(basic_tree_leaf
    src/basic_tree_leaf.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(basic_tree_leaf basic_tree_leaf)

ADD_EXECUTABLE(basic_tree_postorder
    src/basic_tree_postorder.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(basic_tree_postorder basic_tree_postorder)

ADD_EXECUTABLE(basic_tree_preorder
    src/basic_tree_preorder.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(basic_tree_preorder basic_tree_preorder)

ADD_EXECUTABLE(basic_tree_sibling
    src/basic_tree_sibling.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(basic_tree_sibling basic_tree_sibling)

ADD_EXECUTABLE(data_tree
    src/data_tree.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(data_tree data_tree)

ADD_EXECUTABLE(heterogenous_tree_deep_copy
    src/heterogenous_tree_deep_copy.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(heterogenous_tree_deep_copy heterogenous_tree_deep_copy)

ADD_EXECUTABLE(heterogenous_tree_deep_copy2
    src/heterogenous_tree_deep_copy2.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(heterogenous_tree_deep_copy2 heterogenous_tree_deep_copy2)

ADD_EXECUTABLE(tokenizer_captured_delimiters
    src/tokenizer_captured_delimiters.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(tokenizer_captured_delimiters tokenizer_captured_delimiters)

ADD_EXECUTABLE(tokenizer_comment_capture
    src/tokenizer_comment_capture.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(tokenizer_comment_capture tokenizer_comment_capture)

ADD_EXECUTABLE(tokenizer_comments
    src/tokenizer_comments.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(tokenizer_comments tokenizer_comments)

ADD_EXECUTABLE(tokenizer_empty
    src/tokenizer_empty.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(tokenizer_empty tokenizer_empty)

ADD_EXECUTABLE(tokenizer_padded_quoted_str
    src/tokenizer_padded_quoted_str.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(tokenizer_padded_quoted_str tokenizer_padded_quoted_str)

ADD_EXECUTABLE(tokenizer_simple_quoted_str
    src/tokenizer_simple_quoted_str.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(tokenizer_simple_quoted_str tokenizer_simple_quoted_str)

ADD_EXECUTABLE(tokenizer_simple_str
    src/tokenizer_simple_str.cpp
    ${TEST_COMMON_SOURCE_FILES}
    ${MAIN_PLATYPUS_SOURCE_FILES})
ADD_TEST(tokenizer_simple_str tokenizer_simple_str)


