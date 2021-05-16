#include "algoInterval.h"

#include <algorithm>



namespace space {
	NodeInterval::NodeInterval(IBoard::Ptr v_board, unsigned int v_depth)
	{
		//TODO;
	}
	int space::NodeInterval::familySize() const
	{
		return 0;  //TODO
	}



	void AlgoInterval::prune(NodeInterval::Ptr)
	{  //TODO
	}

	void AlgoInterval::fold(NodeInterval::Ptr)
	{  //TODO
	}

	void AlgoInterval::expand()
	{  
		this->expand(this->root);
	}

	void AlgoInterval::expand(NodeInterval::Ptr node)
	{  
		if (node->children.size() == 0)
			this->expandLeafNode(node);
		else {
			for (auto& mn : node->children) {
				this->expand(mn.second);
			}
		}
	}

	void AlgoInterval::expandLeafNode(NodeInterval::Ptr node)
	{  //TODO
	}

	void AlgoInterval::refresh()
	{  
		this->refreshNode(this->root);
	}

	void AlgoInterval::refreshNode(NodeInterval::Ptr node)
	{
		if (node->children.size() == 0) { // Nothing to do for leaf nodes
			return;
		}
		for (auto child : node->children) {
			this->refreshNode(child.second);
		}
		this->prune(node);
		this->fold(node);
	}

}



