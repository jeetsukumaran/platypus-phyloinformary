/**
 * @package     platypus-phyloinformary
 * @brief       An STL-like n-ary drected tree ("arborescence") container with
 *              data stored in nodes.
 * @author      Jeet Sukumaran
 * @copyright   Copyright (C) 2013 Jeet Sukumaran.
 *              This file is part of "platypus-phyloinformary".
 *              "platypus-phyloinformary" is free software: you can
 *              redistribute it and/or modify it under the terms of the GNU
 *              General Public License as published by the Free Software
 *              Foundation, either version 3 of the License, or (at your
 *              option) any later version.  "platypus-phyloinformary" is
 *              distributed in the hope that it will be useful, but WITHOUT ANY
 *              WARRANTY; without even the implied warranty of MERCHANTABILITY
 *              or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *              Public License for more details.  You should have received a
 *              copy of the GNU General Public License along with
 *              "platypus-phyloinformary".  If not, see
 *              <http://www.gnu.org/licenses/>.
 * @note        Modified/derived from:
 *                  tree.hh -- STL-like templated tree class.
 *                  Copyright (C) 2001-2011 Kasper Peeters <kasper@phi-sci.com>.
 *                  Distributed under the GNU General Public License version 3.
 *                  @see http://tree.phi-sci.com/
 *
 */

#ifndef PLATYPUS_TREE_HPP
#define PLATYPUS_TREE_HPP

#include <cassert>
#include <limits>
#include <stack>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// TreeNode

/**
 * 'TreeNode' serves as a abstraction layer between the data associated with the
 * node (it's "value") and the tree structure itself.
 *
 * @tparam NodeDataT
 *   Any assignable type an be used as the data -- a primitive type, or a
 *   struct or class provides an assignment operator method, ``NodeDataT &
 *   operator=(const NodeDataT &)``.
 */
template<class NodeDataT>
class TreeNode {

    public:

        /////////////////////////////////////////////////////////////////////////
        // Lifecycle

        TreeNode()
            : parent_(nullptr)
              , first_child_(nullptr)
              , last_child_(nullptr)
              , next_sibling_(nullptr) { }

        TreeNode(const NodeDataT& data)
            : parent_(nullptr)
              , first_child_(nullptr)
              , last_child_(nullptr)
              , next_sibling_(nullptr)
              , data_(data) { }

        TreeNode(TreeNode&& other)
            : parent_(other.parent_)
              , first_child_(other.first_child_)
              , last_child_(other.last_child_)
              , next_sibling_(other.next_sibling_)
              , data_(std::move(other.data_)) {
            other.parent_ = nullptr;
            other.first_child_ = nullptr;
            other.last_child_ = nullptr;
            other.next_sibling_ = nullptr;
        }

        // note: shallow copy!
        TreeNode(const TreeNode& other)
            : parent_(other.parent_)
              , first_child_(other.first_child_)
              , last_child_(other.last_child_)
              , next_sibling_(other.next_sibling_)
              , data_(other.data_) {
        }

        // note: shallow copy!
        TreeNode& operator=(const TreeNode& other) {
            this->parent_ = other.parent_;
            this->first_child_ = other.first_child_;
            this->last_child_ = other.last_child_;
            this->next_sibling_ = other.next_sibling_;
            this->data_ = other.data_;
            return *this;
        }

        ~TreeNode() {
            this->clear_links();
        }

        /////////////////////////////////////////////////////////////////////////
        // Structure

        inline void add_child(TreeNode<NodeDataT> * ch) {
            if (this->first_child_ == nullptr) {
                this->first_child_ = ch;
                this->last_child_ = ch;
            } else {
                this->last_child_->next_sibling_ = ch;
                this->last_child_ = ch;
            }
            ch->parent_ = this;
            ch->next_sibling_ = nullptr;
        }

        inline TreeNode<NodeDataT> * parent_node() const {
            return this->parent_;
        }

        inline void set_parent(TreeNode<NodeDataT> * parent) {
            this->parent_ = parent;
        }

        inline TreeNode<NodeDataT> * first_child_node() const {
            return this->first_child_;
        }

        inline TreeNode<NodeDataT> * last_child_node() const {
            return this->last_child_;
        }

        inline TreeNode<NodeDataT> * next_sibling_node() const {
            return this->next_sibling_;
        }
        inline void set_next_sibling_node(TreeNode<NodeDataT> * nd) {
            this->next_sibling_ = nd;
        }

        bool is_leaf() const {
            return this->first_child_ == nullptr;
        }

        inline void clear_links() {
            this->parent_ = nullptr;
            this->first_child_ = nullptr;
            this->last_child_ = nullptr;
            this->next_sibling_ = nullptr;
        }

        /////////////////////////////////////////////////////////////////////////
        // Data

        const NodeDataT& data() const {
            return this->data_;
        }

        NodeDataT& data() {
            return this->data_;
        }

        void set_data(const NodeDataT& data) {
            this->data_ = data;
        }

        inline NodeDataT& parent() const {
            assert(this->parent_);
            return this->parent_->data;
        }

        inline NodeDataT& first_child() const {
            assert(this->first_child_);
            return this->first_child_->data_;
        }

        inline NodeDataT& last_child() const {
            assert(this->last_child_);
            return this->last_child_->data_;
        }

        inline NodeDataT& next_sibling() const {
            assert(this->next_sibling_);
            return this->next_sibling_->data;
        }

    private:
        TreeNode<NodeDataT> *        parent_;
        TreeNode<NodeDataT> *        first_child_;
        TreeNode<NodeDataT> *        last_child_;
        TreeNode<NodeDataT> *        next_sibling_;
        NodeDataT                    data_;

}; // TreeNode

////////////////////////////////////////////////////////////////////////////////
// Tree

template<class NodeDataT, class TreeNodeAllocatorT = std::allocator<TreeNode<NodeDataT>>>
class Tree {

    public:
        typedef TreeNode<NodeDataT> node_type;
        typedef NodeDataT value_type;

    public:

        /////////////////////////////////////////////////////////////////////////
        // Lifecycle

        // If ``manage_node_allocation`` is false, then client code is
        // responsible for creation (= allocation of memory + construction) +
        // disposal (= destruction + deallocation) of *all* node objects.
        //
        // All structural methods that require the base class to create nodes
        // will be unavailable (std::logic_error will be thrown).
        //
        // In addition, client code *must* call Tree::initialize(), passing in two
        // constructed node objects to service as the tree before any
        // operations.
        Tree(bool manage_node_allocation=true)
                : manage_node_allocation_(manage_node_allocation) {
            if (this->manage_node_allocation_) {
                this->initialize(this->create_internal_node(), this->create_internal_node());
            }
        }

        // Constructs a deep copy of structure; node data is copied using the
        // assignment operator (operator=) implemented for NodeDataT, so whether
        // or not the node data is deep copied or shallow copied depends
        // on the behavior of this operator
        Tree(const Tree& other)
            : manage_node_allocation_(other.manage_node_allocation_) {
            *this = other;
        }

        Tree(Tree&& other)
                : tree_node_allocator_(std::move(other.tree_node_allocator_)),
                  manage_node_allocation_(std::move(other.manage_node_allocation_)),
                  head_node_(std::move(other.head_node_)),
                  stop_node_(std::move(other.stop_node_)) {
            other.head_node_ = nullptr;
            other.stop_node_ = nullptr;
        }

        virtual ~Tree() {
            this->dispose_node(this->head_node_);
            this->dispose_node(this->stop_node_);
        }

        // Creates a deep copy of structure; node data is copied using the
        // assignment operator (operator=) implemented for NodeDataT, so whether
        // or not the node data is deep copied or shallow copied depends
        // on the behavior of this operator
        Tree& operator=(const Tree& other) {
            this->deep_copy_from(other);
            return *this;
        }

        // If node allocation is non-managed (``manage_node_allocation ==
        // false``), then client code *must* call this method, passing in two
        // node objects before this class is used.
        void initialize(node_type * head_node, node_type * stop_node) {
            this->head_node_ = head_node;
            this->stop_node_ = stop_node;
            this->head_node_->set_next_sibling_node(this->stop_node_);
        }

        void clear() {
            if (this->manage_node_allocation_) {
                while (this->allocated_nodes_.size() > 0) {
                    auto ndi = this->allocated_nodes_.begin();
                    (*ndi)->clear_links();
                    this->dispose_node(*ndi);
                }
                this->initialize(this->create_internal_node(), this->create_internal_node());
            } else {
            }
        }

        /////////////////////////////////////////////////////////////////////////
        // Iterators

        class base_iterator {
            public:
				typedef base_iterator               self_type;
				typedef NodeDataT                    value_type;
				typedef NodeDataT *                  pointer;
				typedef NodeDataT &                  reference;
				typedef unsigned long               size_type;
				typedef int                         difference_type;
				typedef std::forward_iterator_tag   iterator_category;

                base_iterator(node_type * node)
                    : node_(node) { }
                virtual ~base_iterator() {}
                reference operator*() const {
                    return this->node_->data();
                }
                pointer operator->() const {
                    return &(this->node_->data());
                }
                bool operator==(const self_type& rhs) const {
                    return this->node_ == rhs.node_;
                }
                bool operator!=(const self_type& rhs) const {
                    return !(*this == rhs);
                }
                node_type * node() const {
                    return this->node_;
                }
                bool is_leaf() const {
                    assert(this->node_);
                    return this->node_->is_leaf();
                }
                reference parent() const {
                    assert(this->node_);
                    return this->node_->parent();
                }
                reference first_child() const {
                    assert(this->node_);
                    return this->node_->first_child();
                }
                reference last_child() const {
                    assert(this->node_);
                    return this->node_->last_child();
                }
                reference next_sibling() const {
                    assert(this->node_);
                    return this->node_->next_sibling();
                }
                node_type * parent_node() const {
                    assert(this->node_);
                    return this->node_->parent_node();
                }
                node_type * first_child_node() const {
                    assert(this->node_);
                    return this->node_->first_child_node();
                }
                node_type * last_child_node() const {
                    assert(this->node_);
                    return this->node_->last_child_node();
                }
                node_type * next_sibling_node() const {
                    assert(this->node_);
                    return this->node_->next_sibling_node();
                }
                bool operator!() const {
                    return this->node_ == nullptr;
                }
            protected:
                node_type * node_;
        }; // base_iterator
        typedef base_iterator iterator;

        class preorder_iterator : public base_iterator {
            public:
                typedef preorder_iterator self_type;
                preorder_iterator(node_type * node)
                    : base_iterator(node)
                    , skip_current_children_(false) { }
                virtual ~preorder_iterator() {}
                const self_type& operator++() {
                    assert(this->node_ != nullptr);
                    if (this->skip_current_children_ || this->node_->first_child_node() == nullptr) {
                        this->skip_current_children_ = false;
                        while (this->node_->next_sibling_node() == nullptr) {
                            this->node_ = this->node_->parent_node();
                            if (this->node_ == nullptr) {
                                return *this;
                            }
                        }
                        this->node_ = this->node_->next_sibling_node();
                    } else {
                        this->node_ = this->node_->first_child_node();
                    }
                    return *this;
                }
                self_type operator++(int) {
                    self_type i = *this;
                    ++(*this);
                    return i;
                }
            private:
                bool skip_current_children_;
        }; // preorder_iterator

        class postorder_iterator : public base_iterator {
            public:
                typedef postorder_iterator self_type;
                postorder_iterator(node_type * node)
                    : base_iterator(node) { }
                virtual ~postorder_iterator() {}
                const self_type& operator++() {
                    assert(this->node_ != nullptr);
                    node_type * nd = this->node_->next_sibling_node();
                    if (nd == nullptr) {
                        this->node_ = this->node_->parent_node();
                    } else {
                        this->node_ = nd;
                        nd = this->node_->first_child_node();
                        while (nd) {
                            this->node_= nd;
                            nd = this->node_->first_child_node();
                        }
                    }
                    return *this;
                }
                self_type operator++(int) {
                    self_type i = *this;
                    ++(*this);
                    return i;
                }
        }; // postorder_iterator

        class leaf_iterator : public base_iterator {

            public:
                typedef leaf_iterator self_type;
                leaf_iterator(node_type * node, node_type * top_node = nullptr)
                    : base_iterator(node),
                      top_node_(top_node) { }
                virtual ~leaf_iterator() {}
                const self_type& operator++() {
                    if (this->node_->first_child_node() != nullptr) {
                        while (this->node_->first_child_node()) {
                            this->node_ = this->node_->first_child_node();
                        }
                    } else {
                        while (this->node_->next_sibling_node() == nullptr) {
                            if (this->node_->parent_node() == nullptr) {
                                return *this;
                            }
                            this->node_ = this->node_->parent_node();
                            if (this->top_node_ != nullptr && this->node_ == this->top_node_) {
                                return *this;
                            }
                        }
                        this->node_ = this->node_->next_sibling_node();
                        while (this->node_->first_child_node()) {
                            this->node_ = this->node_->first_child_node();
                        }
                    }
                    return *this;
                }
                self_type operator++(int) {
                    self_type i = *this;
                    ++(*this);
                    return i;
                }
            private:
                node_type * top_node_;
        }; // leaf_iterator

        class sibling_iterator : public base_iterator {

            public:
                typedef sibling_iterator self_type;
                sibling_iterator(node_type * node)
                    : base_iterator(node) {
                    }
                virtual ~sibling_iterator() {}
                const self_type& operator++() {
                    if (this->node_ != nullptr) {
                        this->node_ = this->node_->next_sibling_node();
                    }
                    return *this;
                }
                self_type operator++(int) {
                    self_type i = *this;
                    ++(*this);
                    return i;
                }
        }; // sibling_iterator

        /////////////////////////////////////////////////////////////////////////
        // Iteration Control

        // -- preorder iterator --

        preorder_iterator begin() const {
            return preorder_iterator(this->head_node_);
        }

        preorder_iterator end() const {
            return preorder_iterator(this->stop_node_);
        }

        preorder_iterator preorder_begin() const {
            return preorder_iterator(this->head_node_);
        }

        preorder_iterator preorder_end() const {
            return preorder_iterator(this->stop_node_);
        }

        // -- postorder iterator --

        postorder_iterator postorder_begin() const {
            node_type * nd = this->head_node_;
            while (nd->first_child_node() != nullptr) {
                nd = nd->first_child_node();
            }
            return postorder_iterator(nd);
        }

        postorder_iterator postorder_end() const {
            return postorder_iterator(this->head_node_->next_sibling_node());
        }

        // -- leaf iterator --

        leaf_iterator leaf_begin() const {
            return this->leaf_begin(this->head_node_);
        }

        leaf_iterator leaf_begin(node_type * nd) const {
            node_type * first = nd;
            while (first->first_child_node() != nullptr) {
                first = first->first_child_node();
            }
            return leaf_iterator(first, nd);
        }

        template <typename iter>
        leaf_iterator leaf_begin(iter& pos) const {
            return leaf_begin(pos.node());
        }

        leaf_iterator leaf_end() const {
            return this->leaf_end(this->head_node_);
        }

        leaf_iterator leaf_end(node_type * nd) const {
            return leaf_iterator(nd);
        }

        template <typename iter>
        leaf_iterator leaf_end(iter& pos) const {
            return leaf_iterator(pos.node());
        }

        // -- children iterator --

        sibling_iterator children_begin() const {
            return sibling_iterator(this->head_node_->first_child_node());
        }

        sibling_iterator children_begin(node_type * nd) const {
            return sibling_iterator(nd->first_child_node());
        }

        template <typename iter>
        sibling_iterator children_begin(iter& pos) const {
            return sibling_iterator(pos.node()->first_child_node());
        }

        sibling_iterator children_end() const {
            return sibling_iterator(nullptr);
        }

        sibling_iterator children_end(node_type * nd) const {
            return sibling_iterator(nullptr);
        }

        template <typename iter>
        sibling_iterator children_end(iter& pos) const {
            return sibling_iterator(nullptr);
        }

        // -- next_sib iterator --

        sibling_iterator next_sibling_begin(node_type * nd) const {
            return sibling_iterator(nd);
        }

        template <typename iter>
        sibling_iterator next_sibling_begin(iter& pos) const {
            return sibling_iterator(pos.next_sibling_node());
        }

        sibling_iterator next_sibling_end() const {
            return sibling_iterator(nullptr);
        }

        sibling_iterator next_sibling_end(node_type * nd) const {
            return sibling_iterator(nullptr);
        }

        template <typename iter>
        sibling_iterator next_sibling_end(iter& pos) const {
            return sibling_iterator(nullptr);
        }

        /////////////////////////////////////////////////////////////////////////
        // Metrics

        inline unsigned long get_num_leaves() const {
            unsigned long leaf_count = 0;
            for (auto ndi = this->leaf_begin(); ndi != this->leaf_end(); ++ndi, ++leaf_count) {
            }
            return leaf_count;
        }

        /////////////////////////////////////////////////////////////////////////
        // Structure Access

        node_type * head_node() const {
            return this->head_node_;
        }

        node_type * stop_node() const {
            return this->stop_node_;
        }

        /////////////////////////////////////////////////////////////////////////
        // Structure Manipulation

        template<typename iter> iter add_child(iter& pos, node_type * node) {
            pos.node()->add_child(node);
            return iter(node);
        }

        template<typename iter> iter add_child(iter& pos) {
            return this->add_child(pos, this->create_node());
        }

        template<typename iter> iter add_child(iter& pos, const value_type& data) {
            return this->add_child(pos, this->create_node(data));
        }

        template<typename iter, typename... Types> iter emplace_child(iter& pos, const Types&... args) {
            return this->add_child(pos, std::move(value_type(args...)));
        }

        /////////////////////////////////////////////////////////////////////////
        // Default generic allocators

        virtual node_type * create_node() {
            if (this->manage_node_allocation_) {
                node_type * nd = this->tree_node_allocator_.allocate(1, 0);
                this->tree_node_allocator_.construct(nd);
                this->allocated_nodes_.insert(nd);
                return nd;
            } else {
                throw std::logic_error("Tree::create_node(): Request for node allocation but resource is not managed");
            }
        }

        virtual node_type * create_node(const value_type& data) {
            if (this->manage_node_allocation_) {
                node_type * nd = this->tree_node_allocator_.allocate(1, 0);
                this->tree_node_allocator_.construct(nd, data);
                this->allocated_nodes_.insert(nd);
                return nd;
            } else {
                throw std::logic_error("Tree::create_node(const value_type& data): Request for node allocation but resource is not managed");
            }
        }

        /////////////////////////////////////////////////////////////////////////
        // Specialized allocators for leaf and internal nodes.
        // Native implementation simply call Tree::create_node(), i.e., no
        // distinction is made between leaf and internal nodes.
        // Derived classes should override these methods appropriately if leaf
        // and internal nodes are to be managed separately.

        virtual node_type * create_leaf_node() {
            return this->create_node();
        }

        virtual node_type * create_leaf_node(const value_type& data) {
            return this->create_node(data);
        }

        virtual node_type * create_internal_node() {
            return this->create_node();
        }

        virtual node_type * create_internal_node(const value_type& data) {
            return this->create_node(data);
        }

//         template <typename... Types>
//         virtual node_type * create_emplaced_data_node(const Types&... args) {
//             if (this->manage_node_allocation_) {
//                 node_type * nd = this->tree_node_allocator_.allocate(1, 0);
//                 this->tree_node_allocator_.construct(nd, value_type(args...));
//                 // this->tree_node_allocator_.construct(nd, std::move(value_type(args...)));
//                 this->allocated_nodes_.insert(nd);
//                 return nd;
//             } else {
//                 throw std::logic_error("Tree::create_emplaced_data_node(const Types&... args): Request for node allocation but resource is not managed");
//             }
//         }

        /////////////////////////////////////////////////////////////////////////
        // Default dealllocators

        virtual void dispose_node(node_type * nd) {
            if (this->manage_node_allocation_) {
                this->allocated_nodes_.erase(nd);
                this->tree_node_allocator_.destroy(nd);
                this->tree_node_allocator_.deallocate(nd, 1);
            }
        }

        /////////////////////////////////////////////////////////////////////////
        // Cloning/Copying

        /**
         * Rebuilds this as a deep copy of ``other``.
         *
         * @tparam T
         *   Specialization or derived of platypus::Tree.
         * @param src_tree
         *   Tree to be copied.
         * @param deep_copy_node_data_f
         *   Function object which deep copies nodes: f(const typename T::value_type & src, typename Tree::value_type & dest)
         */
        template <typename T>
        void deep_copy_from(const T& src_tree,
                const std::function<void (const typename T::value_type &, typename Tree::value_type &)> deep_copy_node_data_f
                ) {
            this->clear();
            if (this->head_node_ == nullptr && this->stop_node_ == nullptr) {
                this->initialize(this->create_internal_node(), this->create_internal_node());
            } else if (this->head_node_ == nullptr) {
                this->initialize(this->create_internal_node(), this->stop_node_);
            } else if (this->stop_node_ == nullptr) {
                this->initialize(this->head_node_, this->create_internal_node());
            }
            std::unordered_map<typename T::node_type *, node_type *> node_map;
            typename T::node_type * other_node = nullptr;
            typename T::node_type * other_child_node = nullptr;
            node_type * new_node = nullptr;
            for (auto ndi = src_tree.postorder_begin();
                    ndi != src_tree.postorder_end();
                    ++ndi) {
                other_node = ndi.node();
                new_node = nullptr;
                if (other_node->is_leaf()) {
                    new_node = this->create_leaf_node();
                } else {
                    if (other_node == src_tree.head_node()) {
                        new_node = this->head_node_;
                    } else {
                        new_node = this->create_internal_node();
                    }
                    for (auto chi = src_tree.children_begin(ndi);
                            chi != src_tree.children_end(ndi);
                            ++chi) {
                        other_child_node = chi.node();
                        assert(node_map.find(other_child_node) != node_map.end());
                        new_node->add_child(node_map[other_child_node]);
                    }
                }
                // new_node->set_data(other_node->data());
                // new_node->data() = other_node->data();
                deep_copy_node_data_f(other_node->data(), new_node->data());
                node_map[other_node] = new_node;
            }
        }

        /**
         * Rebuilds this as a deep copy of ``other``.
         * Node data value (Tree::value_type) is copied using the assignment
         * operator, which must be defined/overloaded to accept const
         * reference to type other::value_type.
         *
         * @tparam T
         *    Specialization or derived of platypus::Tree.
         * @param src_tree
         *    Tree to be copied.
         */
        template <typename T>
        void deep_copy_from(const T& src_tree) {
            // std::function<void (const typename T::value_type &, typename Tree::value_type &)> deep_copy_node_data_f( [] (const typename T::value_type & src, typename Tree::value_type & dest) { dest = src; } );
            std::function<void (const typename T::value_type &, typename Tree::value_type &)> deep_copy_node_data_f( [] (const typename T::value_type & src, typename Tree::value_type & dest) { dest = src; } );
            this->deep_copy_from(src_tree, deep_copy_node_data_f);
        }

    protected:
        TreeNodeAllocatorT                  tree_node_allocator_;
        bool                                manage_node_allocation_;
        std::unordered_set<node_type *>     allocated_nodes_;
        node_type *                         head_node_;
        node_type *                         stop_node_;

}; // Tree

} // namespace platypus

#endif
