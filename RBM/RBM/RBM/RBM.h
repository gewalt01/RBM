﻿#pragma once
#include "RBMParamator.h"
#include "RBMNode.h"
#include "../RBMMath.h"
#include "../StateCounter.h"
#include <cmath>
#include <vector>
#include <numeric>

//
// Restricted Boltzmann Machine(variables \in {0, 1})
//
class RBM {
private:
	size_t vSize = 0;
	size_t hSize = 0;

public:
	RBMParamator params;
	RBMNode nodes;

public:
	RBM() = default;
	RBM(size_t v_size, size_t h_size);
	~RBM() = default;

	// 可視変数の数を返す
	size_t getVisibleSize();

	// 隠れ変数の数を返す
	size_t getHiddenSize();

	// 規格化定数を返します
	double getNormalConstant();

	// エネルギー関数を返します
	double getEnergy();

	// 自由エネルギーを返します
	double getFreeEnergy();

	// 隠れ変数の活性化関数的なもの
	double actHidJ(int hindex);

	// 可視変数に関する外部磁場と相互作用
	double lambda(int vindex);

	// exp(lambda)の可視変数に関する全ての実現値の総和
	double sumExpLambda(int vindex);

	// 隠れ変数に関する外部磁場と相互作用
	double mu(int hindex);

	// exp(mu)の可視変数に関する全ての実現値の総和
	double miniNormalizeConstantHidden(int hindex);

	// 隠れ変数を条件で与えた可視変数の条件付き確率, P(v_i | h)
	double condProbVis(int vindex, double value);

	// 可視変数を条件で与えた隠れ変数の条件付き確率, P(h_j | v)
	double condProbHid(int hindex, double value);

	// 可視変数の期待値, E[v_i]
	double expectedValueVis(int vindex);

	// 隠れ変数の期待値, E[h_j]
	double expectedValueHid(int hindex);
};


