#ifndef PLATYPUS_TESTING_HPP
#define PLATYPUS_TESTING_HPP

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <platypus/platypus.hpp>

namespace platypus { namespace test {

//////////////////////////////////////////////////////////////////////////////
// BasicTree

class BasicTree : public platypus::Tree<std::string> {
    public:
        typedef platypus::TreeNode<std::string> TreeNodeType;
    public:
        BasicTree();
        ~BasicTree();
}; // BasicTree

//////////////////////////////////////////////////////////////////////////////
// TestDataTree

class TestData {
    public:
        TestData()
            : edge_length_(0.0) { }
        TestData(const std::string & label)
            : label_(label)
              , edge_length_(0.0) { }
        TestData & operator=(const TestData & nd) {
            this->label_ = nd.label_;
            this->edge_length_ = nd.edge_length_;
            return *this;
        }
        void set_label(const std::string & label) {
            this->label_ = label;
        }
        std::string & get_label() {
            return this->label_;
        }
        const std::string get_label() const {
            return this->label_;
        }
        void set_edge_length(double edge_length) {
            this->edge_length_ = edge_length;
        }
        double get_edge_length() const {
            return this->edge_length_;
        }
    protected:
        std::string     label_;
        double          edge_length_;
    friend std::ostream& operator<<(std::ostream& stream, const TestData& data);
}; // TestData

class TestDataTree : public platypus::Tree<TestData> {
    public:
        typedef platypus::TreeNode<TestData> TreeNodeType;
    public:
        TestDataTree()
            : is_rooted_(true) { }
        ~TestDataTree() { }
        void set_is_rooted(bool rooted) {
            this->is_rooted_ = rooted;
        }
        bool is_rooted() const {
            return this->is_rooted_;
        }
    private:
        bool is_rooted_;
}; // TestDataTree

template <class TreeT>
platypus::NewickReader<TreeT> get_test_data_tree_newick_reader(std::vector<TreeT> & trees) {
    auto tree_factory = [&trees] () -> TreeT & { trees.emplace_back(); return trees.back(); };
    auto is_rooted_f = [] (TreeT & tree, bool) {}; // no-op
    auto node_label_f = [] (TestData & value, const std::string& label) {value.set_label(label);};
    auto node_edge_f = [] (TestData & value, double len) {value.set_edge_length(len);};
    platypus::NewickReader<TreeT> tree_reader;
    tree_reader.set_tree_factory(tree_factory);
    tree_reader.set_tree_is_rooted_setter(is_rooted_f);
    tree_reader.set_node_label_setter(node_label_f);
    tree_reader.set_edge_length_setter(node_edge_f);
    return tree_reader;
}

template <class TreeT>
platypus::NewickWriter<TreeT> get_standard_newick_writer(bool include_edge_lengths=true) {
    platypus::NewickWriter<TreeT>  newick_writer;
    newick_writer.set_tree_is_rooted_getter([](const TreeT & tree)->bool {return tree.is_rooted();} );
    newick_writer.set_node_label_getter([](const typename TreeT::value_type & nv)->std::string {return nv.get_label();} );
    if (include_edge_lengths) {
        newick_writer.set_edge_length_getter([](const typename TreeT::value_type & nv)->double {return nv.get_edge_length();} );
    }
    return newick_writer;
}


template <class TreeT>
platypus::NewickReader<TreeT> get_single_tree_newick_reader(TreeT & tree) {
    auto tree_factory = [&tree] () -> TreeT & { return tree; };
    auto is_rooted_f = [] (TreeT & tree, bool) {}; // no-op
    auto node_label_f = [] (TestData & value, const std::string& label) {value.set_label(label);};
    auto node_edge_f = [] (TestData & value, double len) {value.set_edge_length(len);};
    platypus::NewickReader<TreeT> tree_reader;
    tree_reader.set_tree_factory(tree_factory);
    tree_reader.set_tree_is_rooted_setter(is_rooted_f);
    tree_reader.set_node_label_setter(node_label_f);
    tree_reader.set_edge_length_setter(node_edge_f);
    return tree_reader;
}


//////////////////////////////////////////////////////////////////////////////
// General String Support/Utility

std::string& ltrim(std::string& str, const std::string& chars=" \t");
std::string& rtrim(std::string& str, const std::string& chars=" \t");
std::string& trim(std::string& str, const std::string& chars=" \t");
std::string& stripspaces(std::string& str);
std::string& uppercase(std::string& str);
std::string& lowercase(std::string& str);

std::vector<std::string>& split(const std::string& str,
        const std::string& delimiter,
        std::vector<std::string>& tokens,
        bool trim_tokens=true,
        bool include_empty=true);
std::vector<std::string> split(const std::string& str,
        const std::string& delimiter,
        bool trim_tokens=true,
        bool include_empty=true);


//////////////////////////////////////////////////////////////////////////////
// Logging and printing

template<class T>
void write_container(std::ostream& out, const T& container, const std::string& separator=", ") {
    std::copy(container.cbegin(), container.cend(), std::ostream_iterator<typename T::value_type>(out, separator.c_str()));
}

template<class T>
std::string join_container(const T& container, const std::string& separator=", ") {
    std::ostringstream out;
    write_container(out, container, separator);
    return out.str();
}

template <typename S>
void log(S&) {}

template <typename S, typename T>
void log(S& stream, const T& arg1) {
    stream << arg1;
}

template <typename S, typename T>
void log(S& stream, const std::vector<T>& arg1) {
    write_container(stream, arg1, ", ");
    log(stream);
}

template <typename S, typename T, typename... Types>
void log(S& stream, const std::vector<T>& arg1, const Types&... args) {
    write_container(stream, arg1, ", ");
    log(stream, args...);
}

template <typename S, typename T, typename... Types>
void log(S& stream, const T& arg1, const Types&... args) {
    stream << arg1;
    log(stream, args...);
}

//////////////////////////////////////////////////////////////////////////////
// Testing

template <typename T, typename U, typename... Types>
int fail_test(const std::string& test_name,
        unsigned long line_num,
        const T& expected,
        const U& observed,
        const Types&... args) {
    std::cerr << "\n||| FAIL |||";
    std::cerr << "\n|     Test: " << test_name;
    std::cerr << "\n|     Line: " << line_num;
    log(std::cerr, "\n| Expected: ", expected);
    log(std::cerr, "\n| Observed: ", observed);
    log(std::cerr, "\n|  Remarks: ", args...);
    std::cerr << std::endl;
    return EXIT_FAILURE;
}

template <typename T, typename U, typename... Types>
int check_equal(
        const T& expected,
        const U& observed,
        const std::string& test_name,
        unsigned long line_num,
        const Types&... args) {
    if (expected != observed) {
        return fail_test(test_name,
                line_num,
                expected,
                observed,
                args...);
    } else {
        return EXIT_SUCCESS;
    }
}

template <typename T>
inline bool is_almost_equal(T a, T b, double tolerance=1e-14) {
    if (a == b) {
        // shortcut, handles infinities
        return true;
    }
    T abs_a = std::fabs(a);
    T abs_b = std::fabs(b);
    T diff = std::fabs(a - b);
    if (abs_a < tolerance && abs_b < tolerance && diff < tolerance) {
        // Hacky, I know. Without this tests fail if, e.g. a = 0 and b =
        // 6.786e-15 ... is this a real failure?
        return true;
    }
    const static T MIN_T_VALUE = std::numeric_limits<T>::min();
    if (a == 0 || b == 0 || diff < MIN_T_VALUE)  {
        // a or b is zero or both are extremely close to it
        // relative error is less meaningful here
        return diff < (tolerance * MIN_T_VALUE);
    }
    // use relative error
    return (diff / (abs_a + abs_b)) < tolerance;
}

template <typename T, typename... Types>
int check_almost_equal(
        T expected,
        T observed,
        const std::string& test_name,
        unsigned long line_num,
        const Types&... args) {
    if (!is_almost_equal(expected, observed))  {
        return fail_test(test_name,
                line_num,
                expected,
                observed,
                args...);
    } else {
        return EXIT_SUCCESS;
    }
}

//////////////////////////////////////////////////////////////////////////////
// Tree Parsing

template <class TreeType>
void build_tree(TreeType& tree,
        const std::string& str) {
    /*
    //
    // Following tree:
    //
    //                  a
    //                 / \
    //                /   \
    //               /     \
    //              /       \
    //             /         \
    //            /           \
    //           /             c
    //          b             / \
    //         / \           /   \
    //        /   e         /     f
    //       /   / \       /     / \
    //      /   /   \     g     /   h
    //     /   /     \   / \   /   / \
    //    i    j     k  l  m  n   o   p
    //
    //  Can be specified as:
    //
    //      a -> b -> i;
    //      b -> e -> j;
    //      e -> k;
    //      a -> c;
    //      c -> g;
    //      c -> f;
    //      g -> l;
    //      g -> m;
    //      f -> n;
    //      f -> h -> o;
    //      h -> p;
    //
    // Newick:
    //
    //      ((i, (j, k)e)b, ((l, m)g, (n, (o, p)h)f)c)a;
    */

    std::unordered_map<std::string, typename TreeType::node_type *>                  nodes;
    std::unordered_map<std::string, std::vector<typename TreeType::node_type *>>     parent_children;
    auto entries = split(str, ";", true, false);
    std::vector<std::string> relations;
    for (auto & entry : entries) {
        relations.clear();
        split(entry, "->", relations, true, false);
        std::string & parent_src = relations[0];
        typename TreeType::node_type * ch_node = nullptr;
        for (unsigned idx = 1; idx < relations.size(); ++idx) {
            const std::string & ch_src = relations[idx];
            if (nodes.find(ch_src) == nodes.end()) {
                ch_node = tree.create_node(typename TreeType::value_type(ch_src));
                nodes[ch_src] = ch_node;
            } else {
                ch_node = nodes[ch_src];
            }
            parent_children[parent_src].push_back(ch_node);
            parent_src = ch_src;
        }
    }
    typename TreeType::node_type * cur_node = nullptr;
    for (auto & subtree : parent_children) {
        auto & parent_src = subtree.first;
        auto & child_nodes = subtree.second;
        if (nodes.find(parent_src) == nodes.end()) {
            cur_node = tree.begin().node();
            cur_node->set_value(typename TreeType::value_type(parent_src));
        } else {
            cur_node = nodes[parent_src];
        }
        for (auto & ch_node : child_nodes) {
            cur_node->add_child(ch_node);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// Standard Tree for Tests

const std::string STANDARD_TEST_TREE_STRING = "a -> b -> i; b -> e -> j; e -> k; a -> c; c -> g; c -> f; g -> l; g -> m; f -> n; f -> h -> o; h -> p;";
const std::string STANDARD_TEST_TREE_NEWICK = "((i, (j, k)e)b, ((l, m)g, (n, (o, p)h)f)c)a;";
const std::vector<std::string> STANDARD_TEST_TREE_PREORDER = {"a", "b", "i", "e", "j", "k", "c", "g", "l", "m", "f", "n", "h", "o", "p"};
const std::vector<std::string> STANDARD_TEST_TREE_POSTORDER = {"i", "j", "k", "e", "b", "l", "m", "g", "n", "o", "p", "h", "f", "c", "a"};
const std::vector<std::string> STANDARD_TEST_TREE_LEAVES = {"i", "j", "k", "l", "m", "n", "o", "p"};
const std::map<std::string, std::vector<std::string>> STANDARD_TEST_TREE_CHILDREN = {
    {"a", {"b", "c"}},
    {"b", {"i", "e"}},
    {"c", {"g", "f"}},
    {"e", {"j", "k"}},
    {"f", {"n", "h"}},
    {"g", {"l", "m"}},
    {"h", {"o", "p"}},
    {"i", {}},
    {"j", {}},
    {"k", {}},
    {"l", {}},
    {"m", {}},
    {"n", {}},
    {"o", {}},
    {"p", {}},
};
const std::map<std::string, std::vector<std::string>> STANDARD_TEST_TREE_SIBLINGS = {
    {"a", {}},
    {"b", {"c"}},
    {"c", {}},
    {"e", {}},
    {"f", {}},
    {"g", {"f"}},
    {"h", {}},
    {"i", {"e"}},
    {"j", {"k"}},
    {"k", {}},
    {"l", {"m"}},
    {"m", {}},
    {"n", {"h"}},
    {"o", {"p"}},
    {"p", {}},
};

//////////////////////////////////////////////////////////////////////////////
// Tree I/O

// template <class TreeT, class iter>
// void write_newick_node(const TreeT& tree,
//         const iter& tree_iter,
//         std::ostream& out) {
//     if (!tree_iter.is_leaf()) {
//         out << "(";
//         int ch_count = 0;
//         for (auto chi = tree.children_begin(tree_iter);
//                 chi != tree.children_end(tree_iter);
//                 ++chi, ++ch_count) {
//             if (ch_count > 0) {
//                 out << ", ";
//             }
//             write_newick_node(tree, chi, out);
//         }
//         out << ")";
//     } else {
//     }
//     out << *tree_iter;
// }

// template <class TreeT>
// void write_newick(const TreeT& tree,
//         std::ostream& out) {
//     // if (tree.is_rooted()) {
//     //     out << "[&R] ";
//     // } else {
//     //     out << "[&U] ";
//     // }
//     write_newick_node(tree, tree.begin(), out);
//     out << ";" << std::endl;
// }

template <class TreeT, class iter>
void write_newick_node(const TreeT& tree,
        const iter& tree_iter,
        std::ostream& out,
        const std::function<void (typename TreeT::value_type &, std::ostream&)> & write_node_f) {
    if (!tree_iter.is_leaf()) {
        out << "(";
        int ch_count = 0;
        for (auto chi = tree.children_begin(tree_iter);
                chi != tree.children_end(tree_iter);
                ++chi, ++ch_count) {
            if (ch_count > 0) {
                out << ", ";
            }
            write_newick_node(tree, chi, out, write_node_f);
        }
        out << ")";
    }
    write_node_f(*tree_iter, out);
}

template <class TreeT>
void write_newick(const TreeT& tree,
        std::ostream& out,
        const std::function<void (typename TreeT::value_type &, std::ostream&)> & write_node_f) {
    write_newick_node(tree, tree.begin(), out, write_node_f);
    out << ";" << std::endl;
}

template <class TreeT>
void write_newick(const TreeT& tree, std::ostream& out) {
    const std::function<void (typename TreeT::value_type &, std::ostream &)> write_node_f(
            [] (typename TreeT::value_type & nv, std::ostream & out) { out << nv; });
    write_newick(tree, out, write_node_f);
}

//////////////////////////////////////////////////////////////////////////////
// Tokenizer

platypus::Tokenizer get_nexus_tokenizer();

//////////////////////////////////////////////////////////////////////////////
// Checking/Verification

template <class TreeT>
int check_newick(TreeT& tree,
        const std::string & remarks,
        const std::string & compare_str=STANDARD_TEST_TREE_NEWICK,
        bool fail_if_equal=false) {
    std::ostringstream out;
    write_newick(tree, out);
    std::string result = out.str();
    trim(result, " \t\n\r");
    if ( (fail_if_equal && result == compare_str) || (!fail_if_equal && result != compare_str) ) {
        return fail_test(__FILE__,
                __LINE__,
                STANDARD_TEST_TREE_NEWICK,
                result,
                remarks);
    } else {
        return EXIT_SUCCESS;
    }
}

int validate_standard_test_tree(TestDataTree & tree);


int compare_token_vectors(
        const std::vector<std::string> & expected,
        const std::vector<std::string> & observed,
        const std::string & test_title,
        unsigned long line_num);

} } // namespace platypus::test
#endif