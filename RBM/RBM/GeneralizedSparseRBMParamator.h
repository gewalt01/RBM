﻿#pragma once
#include "RBMParamatorBase.h"
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
	GeneralizedSparseRBMParamator();
	GeneralizedSparseRBMParamator(size_t vsize, size_t hsize);
	~GeneralizedSparseRBMParamator();

	// 可視変数の総数を返す
	inline size_t getVisibleSize();

	// 隠れ変数の総数を返す
	inline size_t getHiddenSize();

	// 可視変数のバイアスを返す
	inline double getVisibleBias(int vindex);

	// 可視変数のバイアスベクトルを返す
	inline Eigen::VectorXd getVisibleBiasVector();

	// 隠れ変数のバイアスを返す
	inline double getHiddenBias(int hindex);

	// 隠れ変数のバイアスベクトルを返す
	inline Eigen::VectorXd getHiddenBiasVector();

	// ウェイトパラメータを返す
	inline double getWeight(int vindex, int hindex);

	// ウェイト行列を返す
	inline Eigen::MatrixXd getWeightMatrix();

	// 全てのパラメータを0で初期化
	void initParams();

	// 全てのパラメータを[min, max]の一様乱数で初期化
	void initParamsRandom(double range_min, double range_max);

	// パラメータ情報のシリアライズ
	std::string serialize();

	// パラメータ情報のデシリアライズ
	void deserialize(std::string js);

	// 隠れ変数のスパースパラメータを返す
	inline double getHiddenSparse(int hindex);

	// 隠れ変数のスパースパラメータベクトルを返す
	inline Eigen::VectorXd getHiddenSparseVector();

};

inline GeneralizedSparseRBMParamator::GeneralizedSparseRBMParamator(size_t v_size, size_t h_size) {
	vSize = v_size;
	hSize = h_size;

	initParams();
}

inline void GeneralizedSparseRBMParamator::initParams() {
	b.resize(vSize);
	b.setConstant(0.0);
	c.resize(hSize);
	c.setConstant(0.0);
	w.resize(vSize, hSize);
	w.setConstant(0.0);
	sparse.resize(hSize);
	sparse.setConstant(0.0);
}

inline void GeneralizedSparseRBMParamator::initParamsRandom(double range_min, double range_max) {
	b.resize(vSize);
	c.resize(hSize);
	w.resize(vSize, hSize);
	sparse.resize(hSize);

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(range_min, range_max);

	for (int i = 0; i < vSize; i++) {
		b(i) = dist(mt);

		for (int j = 0; j < hSize; j++) {
			w(i, j) = dist(mt);
		}
	}

	for (int j = 0; j < hSize; j++) {
		c(j) = dist(mt);
		sparse(j) = dist(mt);
	}
}


// 可視変数の総数を返す
inline size_t GeneralizedSparseRBMParamator::getVisibleSize() {
	return vSize;
}

// 隠れ変数の総数を返す
inline size_t GeneralizedSparseRBMParamator::getHiddenSize() {
	return hSize;
}

// 可視変数のバイアスを返す
inline double GeneralizedSparseRBMParamator::getVisibleBias(int vindex) {
	return b(vindex);
}

// 可視変数のバイアスベクトルを返す
inline Eigen::VectorXd GeneralizedSparseRBMParamator::getVisibleBiasVector() {
	return b;
}

// 隠れ変数のバイアスを返す
inline double GeneralizedSparseRBMParamator::getHiddenBias(int hindex) {
	return c(hindex);
}

// 隠れ変数のバイアスベクトルを返す
inline Eigen::VectorXd GeneralizedSparseRBMParamator::getHiddenBiasVector() {
	return c;
}

// ウェイトパラメータを返す
inline double GeneralizedSparseRBMParamator::getWeight(int vindex, int hindex) {
	return w(vindex, hindex);
}

// ウェイト行列を返す
inline Eigen::MatrixXd GeneralizedSparseRBMParamator::getWeightMatrix() {
	return w;
}

// パラメータ情報のシリアライズ
inline std::string GeneralizedSparseRBMParamator::serialize() {
	nlohmann::json json;
	json["vSize"] = vSize;
	json["hSize"] = hSize;
	json["params"]["b"] = std::vector<double>(this->b.data(), this->b.data() + this->b.cols() * this->b.rows());
	json["params"]["c"] = std::vector<double>(this->c.data(), this->c.data() + this->c.cols() * this->c.rows());
	json["params"]["w"] = std::vector<double>(this->w.data(), this->w.data() + this->w.cols() * this->w.rows());
	json["params"]["sparse"] = std::vector<double>(this->sparse.data(), this->sparse.data() + this->sparse.cols() * this->sparse.rows());

	return json.dump();
}

// パラメータ情報のデシリアライズ
inline void GeneralizedSparseRBMParamator::deserialize(std::string js) {
	auto json = nlohmann::json::parse(js);;

	vSize = json["vSize"];
	hSize = json["hSize"];
	std::vector<double> tmp_b(json["params"]["b"].begin(), json["params"]["b"].end());
	std::vector<double> tmp_c(json["params"]["c"].begin(), json["params"]["c"].end());
	std::vector<double> tmp_w(json["params"]["w"].begin(), json["params"]["w"].end());
	std::vector<double> tmp_sparse(json["params"]["sparse"].begin(), json["params"]["sparse"].end());

	this->b = Eigen::Map<Eigen::VectorXd>(tmp_b.data(), vSize);
	this->c = Eigen::Map<Eigen::VectorXd>(tmp_c.data(), hSize);
	this->w = Eigen::Map<Eigen::MatrixXd>(tmp_w.data(), vSize, hSize);
	this->sparse = Eigen::Map<Eigen::VectorXd>(tmp_sparse.data(), hSize);
}

// 隠れ変数のスパースパラメータを返す
inline double GeneralizedSparseRBMParamator::getHiddenSparse(int hindex) {
	return sparse(hindex);
}

// 隠れ変数のスパースパラメータベクトルを返す
inline Eigen::VectorXd GeneralizedSparseRBMParamator::getHiddenSparseVector() {
	return sparse;
}