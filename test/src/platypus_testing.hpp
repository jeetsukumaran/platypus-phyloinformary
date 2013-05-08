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
#include <functional>
#include <platypus/model/tree.hpp>
#include <platypus/utility/tokenizer.hpp>
#include <platypus/utility/testing.hpp>

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
            : is_rooted_(true)
              , ntips_(0)
              , nints_(0)
              , length_(0.0) { }
        ~TestDataTree() { }
        void set_is_rooted(bool rooted) {
            this->is_rooted_ = rooted;
        }
        bool is_rooted() const {
            return this->is_rooted_;
        }
        unsigned long get_ntips() const {
            return this->ntips_;
        }
        void set_ntips(unsigned long ntips) {
            this->ntips_ = ntips;
        }
        unsigned long get_nints() const {
            return this->nints_;
        }
        void set_nints(unsigned long nints) {
            this->nints_ = nints;
        }
        double get_length() const {
            return this->length_;
        }
        void set_length(double length) {
            this->length_ = length;
        }
    private:
        bool             is_rooted_;
        unsigned long    ntips_;
        unsigned long    nints_;
        double           length_;
}; // TestDataTree

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
int compare_against_newick_string(TreeT& tree,
        const std::string & remarks,
        const std::string & compare_str=STANDARD_TEST_TREE_NEWICK,
        bool fail_if_equal=false) {
    std::ostringstream out;
    write_newick(tree, out);
    std::string result = out.str();
    trim(result, " \t\n\r");
    if ( (fail_if_equal && result == compare_str) || (!fail_if_equal && result != compare_str) ) {
        return platypus::testing::fail_test(__FILE__,
                __LINE__,
                STANDARD_TEST_TREE_NEWICK,
                result,
                remarks);
    } else {
        return 0;
    }
}

template <class TreeT>
int compare_against_standard_test_tree(TreeT & tree,
        const std::function<std::string (const typename TreeT::value_type &)> & get_label = [](const typename TreeT::value_type & nv){return nv.get_label();}
        ) {
    int has_failed = 0;

    // postorder
    std::vector<std::string> postorder_visits;
    for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
        postorder_visits.push_back(get_label(*ndi));
    }
    has_failed += platypus::testing::compare_equal(
            STANDARD_TEST_TREE_POSTORDER,
            postorder_visits,
            __FILE__,
            __LINE__);

    // preorder
    std::vector<std::string> preorder_visits;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        preorder_visits.push_back(get_label(*ndi));
    }
    has_failed += platypus::testing::compare_equal(
            STANDARD_TEST_TREE_PREORDER,
            preorder_visits,
            __FILE__,
            __LINE__);

    // leaves
    std::vector<std::string> leaves;
    for (auto ndi = tree.leaf_begin(); ndi != tree.leaf_end(); ++ndi) {
        leaves.push_back(get_label(*ndi));
    }
    has_failed += platypus::testing::compare_equal(
            STANDARD_TEST_TREE_LEAVES,
            leaves,
            __FILE__,
            __LINE__);

    // children
    std::map<std::string, std::vector<std::string>> children;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        for (auto chi = tree.children_begin(ndi); chi != tree.children_end(); ++chi) {
            children[get_label(*ndi)].push_back(get_label(*chi));
        }
        auto expected_iter = STANDARD_TEST_TREE_CHILDREN.find(get_label(*ndi));
        assert(expected_iter != STANDARD_TEST_TREE_CHILDREN.end());
        auto expected_children = expected_iter->second;
        has_failed += platypus::testing::compare_equal(
                expected_children,
                children[get_label(*ndi)],
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
            siblings[get_label(*ndi)].push_back(get_label(*sib));
        }
        auto expected_iter = STANDARD_TEST_TREE_SIBLINGS.find(get_label(*ndi));
        assert(expected_iter != STANDARD_TEST_TREE_SIBLINGS.end());
        auto expected_siblings = expected_iter->second;
        has_failed += platypus::testing::compare_equal(
                expected_siblings,
                siblings[get_label(*ndi)],
                __FILE__,
                __LINE__,
                "Start node: ", *ndi);
    }

    return has_failed;
}

int compare_token_vectors(
        const std::vector<std::string> & expected,
        const std::vector<std::string> & observed,
        const std::string & test_title,
        unsigned long line_num);

} } // namespace platypus::test
#endif
