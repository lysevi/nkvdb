#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include <ostream>
#include <algorithm>
#include <btree_common.h>
namespace trees
{
	template<typename  Key, typename  Value, size_t N>
	class BTree
	{
	public:
        struct node_data: std::pair<Key, Value>{
            node_data()=default;

            node_data(const Key&k,const Value&v){
                this->first=k;
                this->second=v;
            }

            bool operator==(const node_data&other){
                return this->first==other.first;
            }
            bool operator<=(const node_data&other){
                return this->first<=other.first;
            }
            bool operator>(const node_data&other){
                return this->first>other.first;
            }
        };

		struct Node
		{
            typedef Node*  Ptr;
            typedef size_t Weak;
			typedef node_data value_vector[N * 2];
			typedef Weak      child_vector[N * 2];
            value_vector vals;       // n >= size < 2*n
			size_t vals_size;
			child_vector childs;   // size(vals)+1
			size_t childs_size;
            size_t id;
			bool is_leaf;

			typename Node::Weak parent;
			typename Node::Weak next;
			Node();
			~Node();
			void insertValue(Key key,Value val);
			void insertChild(Key key, typename Node::Ptr C);
		};

	public:
		BTree() = delete;
		BTree(Node*_cache, size_t size, size_t root_pos, size_t _cache_pos);
		~BTree();

		Value find(Key key)const;
        typename Node::Ptr  find_node(Key key)const;
		bool insert(Key key, Value val); 
        void to_stream(std::ostream&stream)const;
        typename Node::Ptr getNode(const typename Node::Weak &w);
        typename Node::Ptr getNode(const typename Node::Weak &w)const;

		typename Node::Ptr m_root;
		size_t cache_pos;
		size_t cache_size;
	protected:
        typename Node::Ptr make_node();
		bool iner_find(Key key, typename Node::Ptr cur_node, typename Node::Ptr&out_ptr, Value &out_res)const; // return last_node, false if fail, or cur_node,true;
		bool isFull(const typename Node::Ptr node)const;
		void split_node(typename Node::Ptr node);
        void to_stream(std::ostream&stream, const typename Node::Ptr& root)const;
	private:
		size_t n;
        Node *cache;
       
		
	};
}

#include "btree.hpp"
