#ifndef WIZARD_TRANSFORM_TREE_HPP
#define WIZARD_TRANSFORM_TREE_HPP

#include "ecs/ecs_types.hpp"
#include "ecs/state.hpp"
#include "transforms/transform.hpp"

#include <algorithm>
#include <vector>

namespace engineui
{
	class transform_tree
	{
	public:
		struct node
		{
			entity_id id;
			std::vector<node> children;
		};

		transform_tree(ecs::state& ecs_state) : _ecs_state(ecs_state)
		{
		}

		void insert(entity_id id)
		{
			if (!_ecs_state.has_entity(id))
			{
				int i = 0;
				i++;
			}

			assert(_ecs_state.has_entity(id));
			ecs::entity& e = _ecs_state.find_entity(id);
			
			
			if (!e.has<transforms::transform>())
			{
				int i = 0;
				i++;
			}
			
			assert(e.has<transforms::transform>());

			if (contains(id))
			{
				// early exit
				return;
			}

			// prepare new node for insertion
			node self;
			self.id = id;

			auto is_self = [&](auto& n) { return n.id == self.id; };
			auto is_lesser_node = [](const node& a, const node& b) { return a.id < b.id; };

			transforms::transform& t = e.get_component<transforms::transform>();
			if (t.has_parent)
			{
				// recursively verify insertion of parents before self
				insert(t.parent);
				node& parent = *find(t.parent);

				// insert self as child
				if (std::find_if(parent.children.begin(), parent.children.end(), is_self) == parent.children.end())
				{
					parent.children.push_back(self);
					std::sort(parent.children.begin(), parent.children.end(), is_lesser_node);
				}
			}
			else
			{
				// insert self as a root
				if (std::find_if(_roots.begin(), _roots.end(), is_self) == _roots.end())
				{
					_roots.push_back(self);
					std::sort(_roots.begin(), _roots.end(), is_lesser_node);
				}
			}
		}

		node* find(entity_id id)
		{
			assert(contains(id));
			return find_recurse(id, _roots);
		}

		bool contains(entity_id id)
		{
			return contains_recurse(id, _roots);
		}

		void clear()
		{
			_roots.clear();
		}

		/*
		FunctorOnStepDown should be a lambda that takes no parameters. It is called when stepping down the tree, from a parent to next operate on its children.
		FunctorOnNode should be a lambda that takes a node as reference parameter.
		FunctorOnStepUp should be a lambda that takes no parameters. It is called when stepping up the tree, after processing all children moving back up to parent.
		*/
		template <typename FunctorOnStepDown, typename FunctorOnNode, typename FunctorOnStepUp>
		void preorder(FunctorOnStepDown on_down, FunctorOnNode on_node, FunctorOnStepUp on_up)
		{
			preorder_recurse(on_down, on_node, on_up, _roots);
		}

	private:
		node* find_recurse(entity_id id, std::vector<node>& nodes)
		{
			for (auto& n : nodes)
			{
				if (n.id == id)
				{
					return &n;
				}
				if (node* p = find_recurse(id, n.children))
				{
					return p;
				}
			}
			return nullptr;
		}

		bool contains_recurse(entity_id id, std::vector<node>& nodes)
		{
			for (auto& n : nodes)
			{
				if (n.id == id || contains_recurse(id, n.children))
				{
					return true;
				}
			}
			return false;
		}

		template <typename FunctorOnStepDown, typename FunctorOnNode, typename FunctorOnStepUp>
		void preorder_recurse(FunctorOnStepDown on_down, FunctorOnNode on_node, FunctorOnStepUp on_up, std::vector<node>& nodes)
		{
			on_down();
			for (auto& n : nodes)
			{
				on_node(n);
				preorder_recurse(on_down, on_node, on_up, n.children);
			}
			on_up();
		}

	private:
		ecs::state& _ecs_state;
		std::vector<node> _roots;
	};
}

#endif
