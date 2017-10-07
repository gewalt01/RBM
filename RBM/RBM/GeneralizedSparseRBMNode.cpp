#include "GeneralizedSparseRBMNode.h"
#include <numeric>


GeneralizedSparseRBMNode::GeneralizedSparseRBMNode()
{
}


GeneralizedSparseRBMNode::~GeneralizedSparseRBMNode()
{
}

GeneralizedSparseRBMNode::GeneralizedSparseRBMNode(size_t v_size, size_t h_size) {
	vSize = v_size;
	hSize = h_size;
	v.setConstant(v_size, 0.0);
	h.setConstant(h_size, 0.0);
}

std::vector<int> GeneralizedSparseRBMNode::getVnodeIndexSet() {
	std::vector<int> set(vSize);
	std::iota(set.begin(), set.end(), 0);

	return set;
}

std::vector<int> GeneralizedSparseRBMNode::getHnodeIndexSet() {
	std::vector<int> set(hSize);
	std::iota(set.begin(), set.end(), 0);

	return set;
}
