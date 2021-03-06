﻿#pragma once
#include "../RBMParamatorBase.h"
#include "Eigen/Core"
#include "json.hpp"
#include <random>

class GeneralizedSparseRBMParamator : RBMParamatorBase {
private:
	size_t vSize;
	size_t hSize;
public:
	Eigen::VectorXd b;  // 可視変数のバイアス
	Eigen::VectorXd c;  // 隠れ変数のバイアス
	Eigen::MatrixXd w;  // 可視変数-隠れ変数間のカップリング
	Eigen::VectorXd sparse;  // 隠れ変数のスパースパラメータ


public:
	GeneralizedSparseRBMParamator() = default;
	GeneralizedSparseRBMParamator(size_t vsize, size_t hsize);
	~GeneralizedSparseRBMParamator() = default;

	// 可視変数の総数を返す
	size_t getVisibleSize();

	// 隠れ変数の総数を返す
	size_t getHiddenSize();

	// 可視変数のバイアスを返す
	double getVisibleBias(int vindex);

	// 可視変数のバイアスベクトルを返す
	Eigen::VectorXd getVisibleBiasVector();

	// 隠れ変数のバイアスを返す
	double getHiddenBias(int hindex);

	// 隠れ変数のバイアスベクトルを返す
	Eigen::VectorXd getHiddenBiasVector();

	// ウェイトパラメータを返す
	double getWeight(int vindex, int hindex);

	// ウェイト行列を返す
	Eigen::MatrixXd getWeightMatrix();

	// 全てのパラメータを0で初期化
	void initParams();

	// 全てのパラメータを[min, max]の一様乱数で初期化
	void initParamsRandom(double range_min, double range_max);
	void initParamsRandom(double range_min, double range_max, int seed);

	// 全てのパラメータを[min, max]の一様乱数で初期化
	void initParamsXavier();
	void initParamsXavier(int seed);

	// パラメータ情報のシリアライズ
	std::string serialize();

	// パラメータ情報のデシリアライズ
	void deserialize(std::string js);

	// 隠れ変数のスパースパラメータを返す
	double getHiddenSparse(int hindex);

	// 隠れ変数のスパースパラメータベクトルを返す
	Eigen::VectorXd getHiddenSparseVector();

	// パラメータ出力
	void printParams();
};
