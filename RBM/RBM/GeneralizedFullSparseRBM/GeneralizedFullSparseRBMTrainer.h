﻿#pragma once
#include "Eigen/Core"
#include "../Trainer.h"
#include "GeneralizedFullSparseRBM.h"
#include "GeneralizedFullSparseRBMSampler.h"
#include "GeneralizedFullSparseRBMOptimizer.h"
#include <vector>
#include <omp.h>


template<class OPTIMIZERTYPE>
class Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>{
	struct Gradient {
		Eigen::VectorXd vBias;
		Eigen::VectorXd hBias;
		Eigen::MatrixXd weight;
		Eigen::VectorXd hSparseBias;
		Eigen::MatrixXd sparseWeight;
	};

	struct DataMean {
		Eigen::VectorXd vBias;
		Eigen::VectorXd hBias;
		Eigen::MatrixXd weight;
		Eigen::MatrixXd sparseWeight;
	};

	struct RBMExpected {
		Eigen::VectorXd vBias;
		Eigen::VectorXd hBias;
		Eigen::MatrixXd weight;
		Eigen::MatrixXd sparseWeight;
	};

private:
	Gradient gradient;
	DataMean dataMean;
	RBMExpected rbmexpected;
	Optimizer<GeneralizedFullSparseRBM, OPTIMIZERTYPE> optimizer;
	int _trainCount = 0;


public:
	int epoch = 0;
	int batchSize = 1;
	int cdk = 0;
	double learningRate = 0.01;

public:
	Trainer() = default;
	Trainer(GeneralizedFullSparseRBM & rbm);
	~Trainer() = default;

	// 勾配ベクトル初期化
	void initGradient(GeneralizedFullSparseRBM & rbm);

	// 確保済みの勾配ベクトルを0初期化
	void initGradient();

	// データ平均ベクトルを初期化
	void initDataMean(GeneralizedFullSparseRBM & rbm);

	// 確保済みデータ平均ベクトルを初期化
	void initDataMean();

	// サンプル平均ベクトルを初期化
	void initRBMExpected(GeneralizedFullSparseRBM & rbm);

	// 確保済みサンプル平均ベクトルを初期化
	void initRBMExpected();

	// 学習
	void train(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset);

	void trainCD(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset);
	void trainExact(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset);


	// 1回だけ学習
	void trainOnce(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset);

	void trainOnceCD(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset);
	void trainOnceExact(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset);


	// CD計算
	void calcContrastiveDivergence(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes);

	// CD計算
	void calcExact(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes);

	// データ平均の計算
	void calcDataMean(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes);

	// サンプル平均の計算
	void calcRBMExpectedCD(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes);

	// サンプル平均の計算
	void calcRBMExpectedExact(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes);


	// 勾配の計算
	void calcGradient(GeneralizedFullSparseRBM & rbm, std::vector<int> & data_indexes);

	// 勾配更新
	void updateParams(GeneralizedFullSparseRBM & rbm);

	// 対数尤度関数
	double logLikeliHood(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset);

	// 学習情報出力(JSON)
	std::string trainInfoJson(GeneralizedFullSparseRBM & rbm);

	// 学習情報から学習(JSON)
	void trainFromTrainInfo(GeneralizedFullSparseRBM & rbm, std::string json);
};

template<class OPTIMIZERTYPE>
inline Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::Trainer(GeneralizedFullSparseRBM & rbm) {
	this->optimizer = Optimizer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>(rbm);
	initGradient(rbm);
	initDataMean(rbm);
	initRBMExpected(rbm);
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::initGradient(GeneralizedFullSparseRBM & rbm) {
	gradient.vBias.setConstant(rbm.getVisibleSize(), 0.0);
	gradient.hBias.setConstant(rbm.getHiddenSize(), 0.0);
	gradient.hSparseBias.setConstant(rbm.getHiddenSize(), 0.0);
	gradient.weight.setConstant(rbm.getVisibleSize(), rbm.getHiddenSize(), 0.0);
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::initGradient() {
	gradient.vBias.setConstant(0.0);
	gradient.hBias.setConstant(0.0);
	gradient.hSparseBias.setConstant(0.0);
	gradient.weight.setConstant(0.0);
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::initDataMean(GeneralizedFullSparseRBM & rbm) {
	dataMean.vBias.setConstant(rbm.getVisibleSize(), 0.0);
	dataMean.hBias.setConstant(rbm.getHiddenSize(), 0.0);
	dataMean.weight.setConstant(rbm.getVisibleSize(), rbm.getHiddenSize(), 0.0);
	dataMean.hSparseBias.setConstant(rbm.getHiddenSize(), 0.0);
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::initDataMean() {
	dataMean.vBias.setConstant(0.0);
	dataMean.hBias.setConstant(0.0);
	dataMean.weight.setConstant(0.0);
	dataMean.hSparseBias.setConstant(0.0);
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::initRBMExpected(GeneralizedFullSparseRBM & rbm) {
	rbmexpected.vBias.setConstant(rbm.getVisibleSize(), 0.0);
	rbmexpected.hBias.setConstant(rbm.getHiddenSize(), 0.0);
	rbmexpected.weight.setConstant(rbm.getVisibleSize(), rbm.getHiddenSize(), 0.0);
	rbmexpected.hSparseBias.setConstant(rbm.getHiddenSize(), 0.0);
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::initRBMExpected() {
	rbmexpected.vBias.setConstant(0.0);
	rbmexpected.hBias.setConstant(0.0);
	rbmexpected.weight.setConstant(0.0);
	rbmexpected.hSparseBias.setConstant(0.0);
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::train(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset) {
	for (int e = 0; e < epoch; e++) {
		trainOnce(rbm, dataset);
	}
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::trainCD(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset) {
	for (int e = 0; e < epoch; e++) {
		trainOnceCD(rbm, dataset);
	}
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::trainExact(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset) {
	for (int e = 0; e < epoch; e++) {
		trainOnceExact(rbm, dataset);
	}
}


// FIXME: CDとExactをフラグで切り分けられるように
// 1回だけ学習
template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::trainOnce(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset) {
	// 勾配初期化
	initGradient();

	// データインデックス集合
	std::vector<int> data_indexes(dataset.size());

	// ミニバッチ学習のためにデータインデックスをシャッフルする
	std::iota(data_indexes.begin(), data_indexes.end(), 0);
	std::shuffle(data_indexes.begin(), data_indexes.end(), std::mt19937());

	// ミニバッチ
	// バッチサイズの確認
	int batch_size = this->batchSize < dataset.size() ? dataset.size() : this->batchSize;

	// ミニバッチ学習に使うデータのインデックス集合
	std::vector<int> minibatch_indexes(batch_size);
	std::copy(data_indexes.begin(), data_indexes.begin() + batch_size, minibatch_indexes.begin());

	// Contrastive Divergence
	calcContrastiveDivergence(rbm, dataset, minibatch_indexes);

	// 勾配の更新
	updateParams(rbm);

	// オプティマイザの更新
	optimizer.updateOptimizer();

	// Trainer情報更新
	_trainCount++;
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::trainOnceCD(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset) {
	// 勾配初期化
	initGradient();

	// データインデックス集合
	std::vector<int> data_indexes(dataset.size());

	// ミニバッチ学習のためにデータインデックスをシャッフルする
	std::iota(data_indexes.begin(), data_indexes.end(), 0);
	std::shuffle(data_indexes.begin(), data_indexes.end(), std::mt19937());

	// ミニバッチ
	// バッチサイズの確認
	int batch_size = this->batchSize < dataset.size() ? dataset.size() : this->batchSize;

	// ミニバッチ学習に使うデータのインデックス集合
	std::vector<int> minibatch_indexes(batch_size);
	std::copy(data_indexes.begin(), data_indexes.begin() + batch_size, minibatch_indexes.begin());

	// Contrastive Divergence
	calcContrastiveDivergence(rbm, dataset, minibatch_indexes);

	// 勾配の更新
	updateParams(rbm);

	// オプティマイザの更新
	optimizer.updateOptimizer();

	// Trainer情報更新
	_trainCount++;
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::trainOnceExact(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset) {
	// 勾配初期化
	initGradient();

	// データインデックス集合
	std::vector<int> data_indexes(dataset.size());

	// ミニバッチ学習のためにデータインデックスをシャッフルする
	std::iota(data_indexes.begin(), data_indexes.end(), 0);
	std::shuffle(data_indexes.begin(), data_indexes.end(), std::mt19937());

	// ミニバッチ
	// バッチサイズの確認
	int batch_size = this->batchSize < dataset.size() ? dataset.size() : this->batchSize;

	// ミニバッチ学習に使うデータのインデックス集合
	std::vector<int> minibatch_indexes(batch_size);
	std::copy(data_indexes.begin(), data_indexes.begin() + batch_size, minibatch_indexes.begin());

	// Contrastive Divergence
	calcExact(rbm, dataset, minibatch_indexes);

	// 勾配の更新
	updateParams(rbm);

	// オプティマイザの更新
	optimizer.updateOptimizer();

	// Trainer情報更新
	_trainCount++;
}


template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::calcContrastiveDivergence(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes) {
	// データ平均の計算
	calcDataMean(rbm, dataset, data_indexes);

	// サンプル平均の計算(CD)
	calcRBMExpectedCD(rbm, dataset, data_indexes);

	// 勾配計算
	calcGradient(rbm, data_indexes);
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::calcExact(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes) {
	// データ平均の計算
	calcDataMean(rbm, dataset, data_indexes);

	// サンプル平均の計算(CD)
	calcRBMExpectedExact(rbm, dataset, data_indexes);

	// 勾配計算
	calcGradient(rbm, data_indexes);
}


template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::calcDataMean(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes) {
	// 0埋め初期化
	initDataMean();

	auto index_size = data_indexes.size();
    #pragma omp parallel for schedule(static)
	for (int n = 0; n < index_size; n++) {
		auto rbm_replica = rbm;
		auto & data = dataset[n];
		Eigen::VectorXd vect = Eigen::Map<Eigen::VectorXd>(data.data(), data.size());
		rbm_replica.nodes.v = vect;
		auto mu_vect = rbm_replica.muVect();

#pragma omp critical
		{
			for (int i = 0; i < rbm_replica.getVisibleSize(); i++) {
				dataMean.vBias(i) += vect(i);

				for (int j = 0; j < rbm_replica.getHiddenSize(); j++) {
					dataMean.weight(i, j) += vect(i) * rbm_replica.actHidJ(j, mu_vect(j));
				}
			}

			for (int j = 0; j < rbm_replica.getHiddenSize(); j++) {
				dataMean.hBias(j) += rbm_replica.actHidJ(j, mu_vect(j));
				dataMean.hSparseBias(j) += rbm_replica.actHidSparseJ(j, mu_vect(j));
			}
		}
	}
	dataMean.vBias /= static_cast<double>(data_indexes.size());
	dataMean.hBias /= static_cast<double>(data_indexes.size());
	dataMean.weight /= static_cast<double>(data_indexes.size());
	dataMean.hSparseBias /= static_cast<double>(data_indexes.size());
}


template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::calcRBMExpectedCD(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes) {
	// 0埋め初期化
	initRBMExpected();

	auto index_size = data_indexes.size();
	#pragma omp parallel for schedule(static)
	for (int n = 0; n < index_size; n++) {
		auto rbm_replica = rbm;
		auto & data = dataset[n];
		Eigen::VectorXd vect = Eigen::Map<Eigen::VectorXd>(data.data(), data.size());

		// GeneralizedFullSparseRBMの初期値設定
		rbm_replica.nodes.v = vect;

		for (int j = 0; j < rbm_replica.getHiddenSize(); j++) {
			rbm_replica.nodes.h(j) = rbm_replica.actHidJ(j);
		}

		// CD-K
		Sampler<GeneralizedFullSparseRBM> sampler;
		for (int k = 0; k < cdk; k++) {
			sampler.updateByBlockedGibbsSamplingVisible(rbm_replica);
			sampler.updateByBlockedGibbsSamplingHidden(rbm_replica);
		}

		// 結果を格納
#pragma omp critical
		{
			rbmexpected.vBias += rbm_replica.nodes.v;
			rbmexpected.hBias += rbm_replica.nodes.h;
			for (int j = 0; j < rbm_replica.getHiddenSize(); j++) {
				rbmexpected.hSparseBias(j) += -exp(rbm_replica.params.sparse(j)) * abs(rbm_replica.nodes.h(j));
			}

			for (int i = 0; i < rbm_replica.getVisibleSize(); i++) {
				for (int j = 0; j < rbm_replica.getHiddenSize(); j++) {
					rbmexpected.weight(i, j) += rbm_replica.nodes.v(i) * rbm_replica.nodes.h(j);
				}
			}
		}
	}

	rbmexpected.vBias /= static_cast<double>(data_indexes.size());
	rbmexpected.hBias /= static_cast<double>(data_indexes.size());
	rbmexpected.weight /= static_cast<double>(data_indexes.size());
	rbmexpected.hSparseBias /= static_cast<double>(data_indexes.size());
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::calcRBMExpectedExact(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset, std::vector<int> & data_indexes) {
	// 0埋め初期化
	initRBMExpected();

	StateCounter<std::vector<int>> sc(std::vector<int>(rbm.getVisibleSize(), 2));  // 可視変数Vの状態カウンター
	int v_state_map[] = { 0, 1 };  // 可視変数の状態->値変換写像

	auto max_count = sc.getMaxCount();
	#pragma omp parallel for schedule(static)
	for (int c = 0; c < max_count; c++) {
		// FIXME: stlのコピーは遅いぞ
		auto sc_replica = sc;
		sc_replica.innerCounter = c;

		auto v_state = sc_replica.getState();
		auto rbm_replica = rbm;

		// FIXME: v_i == 0 ときそのままcontinueしたほうが速いぞ

		for (int i = 0; i < rbm_replica.getVisibleSize(); i++) {
			rbm_replica.nodes.v(i) = v_state_map[v_state[i]];
		}


		auto b_dot_v = rbm_replica.nodes.getVisibleLayer().dot(rbm_replica.params.b);  // bとvの内積
		auto mu_vect = rbm_replica.muVect();
		auto sum_h_exp_mu_sparse = rbm_replica.sumHExpMuSparse(mu_vect);

#pragma omp critical
		{
			// 期待値一括計算
			// E[v_i]
			for (int i = 0; i < rbm_replica.getVisibleSize(); i++) {
				rbmexpected.vBias(i) += rbm_replica.nodes.v(i) * exp(b_dot_v) * sum_h_exp_mu_sparse;
			}

			// E[v_i h_j] and E[h_j] and E[|h_j|]
			for (int j = 0; j < rbm_replica.getHiddenSize(); j++) {
				auto mu_j = mu_vect(j);
				rbmexpected.hBias(j) += exp(b_dot_v) * sum_h_exp_mu_sparse * rbm_replica.actHidJ(j, mu_j);
				rbmexpected.hSparseBias(j) += exp(b_dot_v) * sum_h_exp_mu_sparse * rbm_replica.actHidSparseJ(j, mu_j);

				for (int i = 0; i < rbm_replica.getVisibleSize(); i++) {
					rbmexpected.weight(i, j) += rbm_replica.nodes.v(i) * exp(b_dot_v) * sum_h_exp_mu_sparse * rbm_replica.actHidJ(j, mu_j);
				}
			}
		}
	}

	auto z = rbm.getNormalConstant();
	rbmexpected.vBias /= z;
	rbmexpected.hBias /= z;
	rbmexpected.weight /= z;
	rbmexpected.hSparseBias /= z;
}


// 勾配の計算
template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::calcGradient(GeneralizedFullSparseRBM & rbm, std::vector<int> & data_indexes) {
	// 勾配ベクトルリセット
	initGradient();

	for (int i = 0; i < rbm.getVisibleSize(); i++) {
		gradient.vBias(i) = dataMean.vBias(i) - rbmexpected.vBias(i);

		for (int j = 0; j < rbm.getHiddenSize(); j++) {
			gradient.weight(i, j) = dataMean.weight(i, j) - rbmexpected.weight(i, j);
		}
	}

	for (int j = 0; j < rbm.getHiddenSize(); j++) {
		gradient.hBias(j) = dataMean.hBias(j) - rbmexpected.hBias(j);
		gradient.hSparseBias(j) = dataMean.hSparseBias(j) - rbmexpected.hSparseBias(j);
	}
}

// パラメータの更新
template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::updateParams(GeneralizedFullSparseRBM & rbm) {
	for (int i = 0; i < rbm.getVisibleSize(); i++) {
		rbm.params.b(i) += optimizer.getNewParamVBias(gradient.vBias(i), i);

		for (int j = 0; j < rbm.getHiddenSize(); j++) {
			rbm.params.w(i, j) += optimizer.getNewParamWeight(gradient.weight(i, j), i, j);
		}
	}

	for (int j = 0; j < rbm.getHiddenSize(); j++) {
		rbm.params.c(j) += optimizer.getNewParamHBias(gradient.hBias(j), j);
		rbm.params.sparse(j) += optimizer.getNewParamHSparse(gradient.hBias(j), j);
	}
}


// 対数尤度関数
template<class OPTIMIZERTYPE>
inline double Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::logLikeliHood(GeneralizedFullSparseRBM & rbm, std::vector<std::vector<double>> & dataset) {
	double value = 0.0;

	auto z = rbm.getNormalConstant();

	for (auto & data : dataset) {
		auto prob = rbm.probVis(data, z);
		value += log(prob);
	}

	return value;
}

// 学習情報出力(JSON)
template<class OPTIMIZERTYPE>
inline std::string Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::trainInfoJson(GeneralizedFullSparseRBM & rbm) {
	auto js = nlohmann::json();
	js["rbm"] = nlohmann::json::parse(rbm.params.serialize());
	js["trainCount"] = _trainCount;
	js["learningRate"] = learningRate;
	js["cdk"] = cdk;
	js["divSize"] = rbm.getHiddenDivSize();
	js["realFlag"] = rbm.isRealHiddenValue();

	return js.dump();
}

template<class OPTIMIZERTYPE>
inline void Trainer<GeneralizedFullSparseRBM, OPTIMIZERTYPE>::trainFromTrainInfo(GeneralizedFullSparseRBM & rbm, std::string json) {
	auto js = nlohmann::json::parse(json);
	rbm.params.deserialize(js["rbm"].dump());
	_trainCount = js["trainCount"];
	learningRate = js["learningRate"];
	cdk = js["cdk"];
	rbm.setHiddenDiveSize(js["divSize"]);
	rbm.setRealHiddenValue(js["realFlag"]);
}

