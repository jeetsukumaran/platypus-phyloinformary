## ``make test`` will redirect all test output to "Testing/Temporary/LastTest.log"
## ``CTEST_OUTPUT_ON_FAILURE=1 make test`` or ``ctest --output-on-failure``
## will show output of failed tests.
## We create a custom target, ``make check`` that wraps up the latter
IF(CMAKE_CONFIGURATION_TYPES)
    ADD_CUSTOM_TARGET(check
        COMMAND ${CMAKE_COMMAND} -D COMPONENT=testsuite -P cmake_install.cmake
        COMMAND ${CMAKE_CTEST_COMMAND}
            --force-new-ctest-process --output-on-failure
            --build-config "$<CONFIGURATION>")
ELSE()
    ADD_CUSTOM_TARGET(check
        COMMAND ${CMAKE_COMMAND} -D COMPONENT=testsuite -P cmake_install.cmake
        COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure)
ENDIF()

## Build and install the library for the test code base
SET(LIBDIR ${PROJECT_BINARY_DIR}/lib)
FILE(MAKE_DIRECTORY ${LIBDIR})
SET(TESTLIB platypustesting)
ADD_LIBRARY(${TESTLIB}
    # STATIC
    src/platypus_testing.cpp
    )
ADD_DEPENDENCIES(check ${TESTLIB})
INSTALL(TARGETS
    ${TESTLIB}
    DESTINATION ${LIBDIR}
    COMPONENT testsuite)
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
    src/emplace_child.cpp
    src/heterogenous_tree_deep_copy.cpp
    src/heterogenous_tree_deep_copy2.cpp
    src/tokenizer_captured_delimiters.cpp
    src/tokenizer_comment_capture.cpp
    src/tokenizer_comments.cpp
    src/tokenizer_empty.cpp
    src/tokenizer_padded_quoted_str.cpp
    src/tokenizer_simple_quoted_str.cpp
    src/tokenizer_simple_str.cpp
    src/newick_reader_basic.cpp
    src/datatable_basic.cpp
    src/newick_reader_basic2.cpp
    src/newick_reader_edge_lengths.cpp
    src/newick_reader_multitrees.cpp
    src/newick_reader_missing_commas.cpp
    src/newick_reader_missing_semicolon.cpp
    src/newick_reader_multi_semicolons.cpp
    src/newick_reader_blank_nodes.cpp
    src/newick_reader_tree_stats.cpp
    src/newick_writer_basic.cpp
    src/newick_vector_reader.cpp
    src/standard_interface.cpp
    src/max_unbalanced_tree_right.cpp
    src/max_unbalanced_tree_left.cpp
    src/max_balanced_tree_even_power_of_two.cpp
    src/max_balanced_tree_even_non_power_of_two.cpp
    src/max_balanced_tree_odd.cpp
   )
FOREACH(test_src_file ${TEST_TARGET_SOURCES})
    get_filename_component(basename ${test_src_file} NAME_WE)
    ADD_EXECUTABLE(${basename}
        ${test_src_file}
        )
    ADD_DEPENDENCIES(check ${basename})
    TARGET_LINK_LIBRARIES(${basename}
        ${TESTLIB})
    ADD_TEST(${basename} ${basename})
ENDFOREACH()

