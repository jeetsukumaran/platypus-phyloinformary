AM_CPPFLAGS = -std=c++11 @CPPFLAGS@

# TESTS = \
# 	basic_tree_deep_copy

# check_PROGRAMS = \
# 	basic_tree_deep_copy

# basic_tree_deep_copy_SOURCES = \
# 	../../src/tree.hpp \
# 	src/tests.hpp \
# 	src/tests.cpp \
# 	src/basic_tree_deep_copy.cpp

TESTS = \
	basic_tree \
	basic_tree_preorder \
	basic_tree_postorder \
	basic_tree_leaf \
	basic_tree_children \
	basic_tree_sibling \
	basic_tree_deep_copy \
	heterogenous_tree_deep_copy \
	heterogenous_tree_deep_copy2 \
	data_tree

bin_PROGRAMS = \
	basic_tree \
	basic_tree_preorder \
	basic_tree_postorder \
	basic_tree_leaf \
	basic_tree_children \
	basic_tree_sibling \
	basic_tree_deep_copy \
	heterogenous_tree_deep_copy \
	heterogenous_tree_deep_copy2 \
	data_tree

COMMON_SRC = \
	../../src/tree.hpp \
	src/tests.hpp \
	src/tests.cpp

basic_tree_SOURCES = \
	$(COMMON_SRC) \
	src/basic_tree.cpp

basic_tree_preorder_SOURCES = \
	$(COMMON_SRC) \
	src/basic_tree_preorder.cpp

basic_tree_postorder_SOURCES = \
	$(COMMON_SRC) \
	src/basic_tree_postorder.cpp

basic_tree_leaf_SOURCES = \
	$(COMMON_SRC) \
	src/basic_tree_leaf.cpp

basic_tree_children_SOURCES = \
	$(COMMON_SRC) \
	src/basic_tree_children.cpp

basic_tree_sibling_SOURCES = \
	$(COMMON_SRC) \
	src/basic_tree_sibling.cpp

basic_tree_deep_copy_SOURCES = \
	$(COMMON_SRC) \
	src/basic_tree_deep_copy.cpp

heterogenous_tree_deep_copy_SOURCES = \
	$(COMMON_SRC) \
	src/heterogenous_tree_deep_copy.cpp

heterogenous_tree_deep_copy2_SOURCES = \
	$(COMMON_SRC) \
	src/heterogenous_tree_deep_copy2.cpp

data_tree_SOURCES = \
	$(COMMON_SRC) \
	src/data_tree.cpp
