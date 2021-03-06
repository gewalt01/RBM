﻿#pragma once

#include "../RBMParamatorBase.h"
#include "Eigen/Core"
#include "json.hpp"
#include <string>
#include <random>

class RBMParamator : RBMParamatorBase {
private:
    size_t vSize;
    size_t hSize;
public:
    Eigen::VectorXd b;  // 可視変数のバイアス
    Eigen::VectorXd c;  // 隠れ変数のバイアス
    Eigen::MatrixXd w;  // 可視変数-隠れ変数間のカップリング


public:
    RBMParamator() = default;
    RBMParamator(size_t vsize, size_t hsize);
    ~RBMParamator() = default;

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
};
