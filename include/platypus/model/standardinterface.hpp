/**
 * @package     platypus-phyloinformary
 * @brief       Configure various objects and functions
 *              that require functors for tree and node operations to use
 *              bindings to "standard interface" class methods.
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
 */

#ifndef PLATYPUS_MODEL_STANDARDNODE_HPP
#define PLATYPUS_MODEL_STANDARDNODE_HPP

#include "tree.hpp"
#include "../base/base_producer.hpp"
#include "../base/base_reader.hpp"
#include "../base/base_writer.hpp"

namespace platypus {

//////////////////////////////////////////////////////////////////////////////
// StandardInterface
// Definition of standard interface node values.

template <class EdgeLengthT=double>
class StandardNodeValueInterface {
    public:
        virtual ~StandardNodeValueInterface() { }
        virtual void set_label(const std::string & label) = 0;
        virtual std::string & get_label() = 0;
        virtual const std::string & get_label() const = 0;
        virtual void set_edge_length(EdgeLengthT edge_length) = 0;
        virtual EdgeLengthT get_edge_length() const = 0;
}; // StandardNodeValueInterface

//////////////////////////////////////////////////////////////////////////////
// StandardTree
// Definition of standard interface for trees.

template <class NodeValueT, class TreeNodeAllocatorT = std::allocator<TreeNode<NodeValueT>>>
class StandardTreeInterface : public Tree<NodeValueT, TreeNodeAllocatorT> {
    public:
        StandardTreeInterface(bool is_rooted=false, bool manage_node_allocation=true)
                : Tree<NodeValueT, TreeNodeAllocatorT>(manage_node_allocation) {
        }
        StandardTreeInterface(const StandardTreeInterface & other)
                : Tree<NodeValueT, TreeNodeAllocatorT>(other) {
        }
        StandardTreeInterface(StandardTreeInterface && other)
                : Tree<NodeValueT, TreeNodeAllocatorT>(other) {
        }
        virtual StandardTreeInterface & operator=(StandardTreeInterface && other) {
            Tree<NodeValueT, TreeNodeAllocatorT>::operator=(other);
            return * this;
        }
        virtual ~StandardTreeInterface() { }
        virtual bool is_rooted() const = 0;
        virtual void set_is_rooted(bool rooted) = 0;
}; // StandardTreeInterface

//////////////////////////////////////////////////////////////////////////////
// StandardNodeValue
// Reference implementation of standard interface (for node values).

template <class EdgeLengthT=double>
class StandardNodeValue : public StandardNodeValueInterface<EdgeLengthT> {
    public:
        StandardNodeValue()
            : edge_length_(0.0) { }
        StandardNodeValue(const std::string & label)
            : label_(label)
              , edge_length_(0.0) { }
        virtual ~StandardNodeValue() { }
        virtual StandardNodeValue & operator=(const StandardNodeValue & nd) {
            this->label_ = nd.label_;
            this->edge_length_ = nd.edge_length_;
            return *this;
        }
        // virtual StandardNodeValue & operator=(StandardNodeValue && nd) {
        //     this->label_ = std::move(nd.label_);
        //     this->edge_length_ = std::move(nd.edge_length_);
        //     return *this;
        // }
        virtual void set_label(const std::string & label) override {
            this->label_ = label;
        }
        virtual std::string & get_label() override {
            return this->label_;
        }
        virtual const std::string & get_label() const override {
            return this->label_;
        }
        virtual void set_edge_length(EdgeLengthT edge_length) override {
            this->edge_length_ = edge_length;
        }
        virtual EdgeLengthT get_edge_length() const override {
            return this->edge_length_;
        }
        virtual void clear() {
            this->label_.clear();
            this->edge_length_ = 0.0;
        }
    protected:
        std::string    label_;
        EdgeLengthT    edge_length_;
}; // StandardNodeValue

//////////////////////////////////////////////////////////////////////////////
// StandardTree
// Reference implementation of standard interface (for node values).

template <class NodeValueT, class TreeNodeAllocatorT = std::allocator<TreeNode<NodeValueT>>>
class StandardTree : public StandardTreeInterface<NodeValueT, TreeNodeAllocatorT> {
    public:
        StandardTree(bool is_rooted=false, bool manage_node_allocation=true)
            : StandardTreeInterface<NodeValueT, TreeNodeAllocatorT>(manage_node_allocation)
            , is_rooted_(is_rooted) {
        }
        StandardTree(const StandardTree & other)
            : StandardTreeInterface<NodeValueT, TreeNodeAllocatorT>(other)
              , is_rooted_(other.is_rooted_) {
        }
        virtual ~StandardTree() {
        }
        virtual StandardTree & operator=(const StandardTree & other) {
            Tree<NodeValueT, TreeNodeAllocatorT>::operator=(other);
            this->is_rooted_ = other.is_rooted_;
            return * this;
        }
        virtual bool is_rooted() const {
            return this->is_rooted_;
        }
        virtual void set_is_rooted(bool rooted) {
            this->is_rooted_ = rooted;
        }
    private:
        bool is_rooted_;
}; // StandardTree

//////////////////////////////////////////////////////////////////////////////
// Functions

/**
 * Binds setter functions to default class method signatures.
 *
 * `TreeT` will have to provide:
 *
 *      void set_is_rooted(bool);
 *
 * `TreeT::value_type` will have to provide:
 *
 *      void set_label(const std::string &);
 *      void set_edge_length(double);
 */
template <class TreeT, class EdgeLengthT=double>
void bind_standard_interface(BaseTreeProducer<TreeT> & producer) {
    producer.set_tree_is_rooted_setter([] (TreeT & tree, bool rooted) { tree.set_is_rooted(rooted); });
    producer.set_node_label_setter([] (typename TreeT::value_type & nv, const std::string& label) { nv.set_label(label); });
    producer.set_edge_length_setter([] (typename TreeT::value_type & nv, EdgeLengthT length) { nv.set_edge_length(length); });
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
template <class TreeT, class EdgeLengthT=double>
void bind_standard_interface(BaseTreeWriter<TreeT> & writer) {
    writer.set_tree_is_rooted_getter([] (const TreeT & tree) -> bool {return tree.is_rooted(); });
    writer.set_node_label_getter([] (const typename TreeT::value_type & nv) -> std::string { return nv.get_label(); });
    writer.set_edge_length_getter([] (const typename TreeT::value_type & nv) -> EdgeLengthT {return nv.get_edge_length(); });
}

} // namespae platypus
#endif
