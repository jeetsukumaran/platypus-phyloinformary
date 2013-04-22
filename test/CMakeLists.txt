
## Build and install the library for the test code base
SET(LIBDIR ${PROJECT_BINARY_DIR}/lib)
FILE(MAKE_DIRECTORY ${LIBDIR})
SET(TESTLIB platypustest)
ADD_LIBRARY(${TESTLIB}
    ../platypus/tokenizer.cpp
    src/tests.cpp
    )
INSTALL(TARGETS
    ${TESTLIB}
    DESTINATION ${LIBDIR})
LINK_DIRECTORIES(${LIBDIR})

## Build and install the test targets
SET(TEST_TARGET_SOURCES
    src/basic_tree.cpp
    src/basic_tree_children.cpp
    src/basic_tree_deep_copy.cpp
    src/basic_tree_leaf.cpp
    src/basic_tree_postorder.cpp
    src/basic_tree_preorder.cpp
    src/basic_tree_sibling.cpp
    src/data_tree.cpp
    src/heterogenous_tree_deep_copy.cpp
    src/heterogenous_tree_deep_copy2.cpp
    src/tokenizer_captured_delimiters.cpp
    src/tokenizer_comment_capture.cpp
    src/tokenizer_comments.cpp
    src/tokenizer_empty.cpp
    src/tokenizer_padded_quoted_str.cpp
    src/tokenizer_simple_quoted_str.cpp
    src/tokenizer_simple_str.cpp
   )
FOREACH(test_src_file ${TEST_TARGET_SOURCES})
    get_filename_component(basename ${test_src_file} NAME_WE)
    ADD_EXECUTABLE(${basename}
        ${test_src_file}
        )
    TARGET_LINK_LIBRARIES(${basename}
        ${TESTLIB})
    # INSTALL(TARGETS
    #     ${basename}
    #     test)
    ADD_TEST(${basename} ${basename})
ENDFOREACH()

