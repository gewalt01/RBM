﻿#pragma once
#include "../RBMNodeBase.h"
#include "Eigen/Core"
#include <vector>
#include <numeric>

class GeneralizedSparseRBMNode : RBMNodeBase {
private:
	size_t vSize = 0;
	size_t hSize = 0;
public:
	Eigen::VectorXd v;
	Eigen::VectorXd h;

public:
	GeneralizedSparseRBMNode() = default;
	GeneralizedSparseRBMNode(size_t v_size, size_t h_size);
	~GeneralizedSparseRBMNode() = default;

	// 可視変数の総数を返す
	inline size_t getVisibleSize();

	// 隠れ変数の総数を返す
	inline size_t getHiddenSize();

	// 可視層の全てを返す
	inline Eigen::VectorXd getVisibleLayer();

	// 隠れ層の全てを返す
	inline Eigen::VectorXd getHiddenLayer();

	// 可視変数の値を返す
	inline double getVisibleUnit(int vindex);

	// 隠れ変数の値を返す
	inline double getHiddenUnit(int vindex);

	// 可視変数のノード番号集合を返す
	std::vector<int> getVnodeIndexSet();

	// 隠れ変数のノード番号集合を返す
	std::vector<int> getHnodeIndexSet();

	// 可視変数に値をセット
	template<typename ARRAY>
	void setVisibleData(ARRAY & data);
};

// 可視変数に値をセット
template<typename ARRAY>
void GeneralizedSparseRBMNode::setVisibleData(ARRAY & data) {
	for (int i = 0; i < vSize; i++) {
		v[i] = data[i];
	}
}
