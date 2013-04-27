#include <sstream>
#include <iostream>
#include <vector>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include "tests.hpp"


namespace platypus {

/**
    * Binds setter functions to default class method signatures.
    *
    * `TreeT::value_type` will have to provide:
    *
    *      void set_label(const std::string &);
    *      void set_edge_length(double);
    *
    *  `TreeT` will have to provide:
    *
    *      void set_is_rooted(bool);
    */
template <class T>
void configure_producer_for_default_interface(T & producer) {
    producer.set_tree_is_rooted_setter([] (typename T::tree_type & tree, bool rooted) { tree.set_is_rooted(rooted); });
    producer.set_node_label_setter([] (typename T::tree_value_type & nv, const std::string& label) { nv.set_label(label); });
    producer.set_edge_length_setter([] (typename T::tree_value_type & nv, double length) { nv.set_edge_length(length); });
}
template <class T>
void configure_producer_for_default_interface_ptr(T & producer) {
    producer.set_tree_is_rooted_setter([] (typename T::tree_type & tree, bool rooted) { tree.set_is_rooted(rooted); });
    producer.set_node_label_setter([] (typename T::tree_value_type & nv, const std::string& label) { nv->set_label(label); });
    producer.set_edge_length_setter([] (typename T::tree_value_type & nv, double length) { nv->set_edge_length(length); });
}


/**
 * Binds getter functions to default class method signatures.
 *
 * `TreeT::value_type` will have to provide:
 *
 *      std::string get_label() const;
 *      double get_edge_length() const;
 *
 *  `TreeT` will have to provide:
 *
 *      bool is_rooted(bool) const;
 */
template <class T>
void configure_writer_for_default_interface(T & writer) {
    writer.set_tree_is_rooted_getter([] (const typename T::tree_type & tree) -> bool {return tree.is_rooted(); });
    writer.set_node_label_getter([] (const typename T::tree_value_type & nv) -> std::string { return nv.get_label(); });
    writer.set_edge_length_getter([] (const typename T::tree_value_type & nv) -> double {return nv.get_edge_length(); });
}

template <class T>
void configure_writer_for_default_interface_ptr(T & writer) {
    writer.set_tree_is_rooted_getter([] (const typename T::tree_type & tree) -> bool {return tree.is_rooted(); });
    writer.set_node_label_getter([] (const typename T::tree_value_type & nv) -> std::string { return nv->get_label(); });
    writer.set_edge_length_getter([] (const typename T::tree_value_type & nv) -> double {return nv->get_edge_length(); });
}

class CommonNode {
    public:
        CommonNode()
            : edge_length_(0.0) { }
        CommonNode(const std::string & label)
            : label_(label)
              , edge_length_(0.0) { }
        CommonNode & operator=(const CommonNode & nd) {
            this->label_ = nd.label_;
            this->edge_length_ = nd.edge_length_;
            return *this;
        }
        CommonNode & operator=(CommonNode && nd) {
            this->label_ = std::move(nd.label_);
            this->edge_length_ = std::move(nd.edge_length_);
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
}; // CommonNode

template <class NodeValueT>
class CommonTree : public platypus::Tree<NodeValueT> {
    public:
        bool is_rooted() const { return true; }
        void set_is_rooted(bool) { }
}; // CommonTree

} // namespace platypus

int trees_as_objects() {

    // tree source
    std::string tree_string;
    for (int i = 0; i < 5; ++i) {
        tree_string += STANDARD_TEST_TREE_NEWICK;
    }

    // tree
    typedef platypus::CommonTree<platypus::CommonNode> TreeType;
    std::vector<TreeType> trees;
    auto tree_factory = [&trees] () -> TreeType & { trees.emplace_back(); return trees.back(); };

    // reading
    auto reader = platypus::NewickReader<TreeType>(tree_factory);
    configure_producer_for_default_interface(reader);
    reader.read_from_string(tree_string);

    // check reading
    if (trees.size() != 5) {
        return 1;
    }

    // writer
    auto writer = platypus::NewickWriter<TreeType>();
    configure_writer_for_default_interface(writer);
    std::ostringstream out;
    writer.write(trees.begin(), trees.end(), out);
    auto result = out.str();

    std::string expected =
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n";

    if (result != expected) {
        return fail_test(__FILE__,
                expected,
                result);
    } else {
        return 0;
    }
}

int trees_as_pointers() {

    // tree source
    std::string tree_string;
    for (int i = 0; i < 5; ++i) {
        tree_string += STANDARD_TEST_TREE_NEWICK;
    }

    // tree
    typedef platypus::CommonTree<platypus::CommonNode> TreeType;
    std::vector<TreeType *> trees;
    auto tree_factory = [&trees] () -> TreeType & { auto t = new TreeType(); trees.push_back(t); return *t; };

    // reading
    auto reader = platypus::NewickReader<TreeType>(tree_factory);
    configure_producer_for_default_interface(reader);
    reader.read_from_string(tree_string);

    // check reading
    if (trees.size() != 5) {
        return 1;
    }

    // writer
    auto writer = platypus::NewickWriter<TreeType>();
    configure_writer_for_default_interface(writer);
    std::ostringstream out;
    writer.write(trees.begin(), trees.end(), out);
    auto result = out.str();

    std::string expected =
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n";

    for (auto & t : trees) {
        delete t;
    }
    if (result != expected) {
        return fail_test(__FILE__,
                expected,
                result);
    } else {
        return 0;
    }
}

// template <class NodeValueT>
// class CommonPtrTree : public platypus::Tree<NodeValueT *> {
//     public:
//         typedef NodeValueT * value_type;
//         typedef platypus::TreeNode<value_type> node_type;
//         bool is_rooted() const { return true; }
//         void set_is_rooted(bool) { }
//         virtual node_type * create_node() override {
//             auto nd = new platypus::TreeNode<value_type>();
//             auto value = new NodeValueT();
//             nd->set_value(value);
//             return nd;
//             // if (this->manage_node_allocation_) {
//             //     node_type * nd = this->tree_node_allocator_.allocate(1, 0);
//             //     this->tree_node_allocator_.construct(nd);
//             //     this->allocated_nodes_.insert(nd);
//             //     return nd;
//             // } else {
//             //     throw std::logic_error("Tree::create_node(): Request for node allocation but resource is not managed");
//             // }
//         }

//         virtual node_type * create_node(const value_type& value) override {
//             throw std::logic_error("not implemented");
//             // if (this->manage_node_allocation_) {
//             //     node_type * nd = this->tree_node_allocator_.allocate(1, 0);
//             //     this->tree_node_allocator_.construct(nd, value);
//             //     this->allocated_nodes_.insert(nd);
//             //     return nd;
//             // } else {
//             //     throw std::logic_error("Tree::create_node(const value_type& value): Request for node allocation but resource is not managed");
//             // }
//         }
// }; // CommonTree

// int node_values_as_pointers() {
//     // tree source
//     std::string tree_string;
//     for (int i = 0; i < 5; ++i) {
//         tree_string += STANDARD_TEST_TREE_NEWICK;
//     }

//     // tree
//     typedef CommonPtrTree<platypus::CommonNode> TreeType;
//     std::vector<TreeType> trees;
//     auto tree_factory = [&trees] () -> TreeType & { trees.emplace_back(); return trees.back(); };

//     // reading
//     auto reader = platypus::NewickReader<TreeType>(tree_factory);
//     configure_producer_for_default_interface_ptr(reader);
//     reader.read_from_string(tree_string);

//     // check reading
//     if (trees.size() != 5) {
//         return 1;
//     }

//     // // writer
//     // auto writer = platypus::NewickWriter<TreeType>();
//     // writer.configure_for_default_interface();
//     // std::ostringstream out;
//     // writer.write(trees.begin(), trees.end(), out);
//     // auto result = out.str();

//     // std::string expected =
//     //     "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
//     //     "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
//     //     "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
//     //     "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
//     //     "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n";

//     // if (result != expected) {
//     //     return fail_test(__FILE__,
//     //             expected,
//     //             result);
//     // } else {
//     //     return 0;
//     // }

//     return 0;
// }

int main() {
    int fail = 0;
    fail += trees_as_objects();
    fail += trees_as_pointers();
    // fail += node_values_as_pointers();
    return fail;
}
