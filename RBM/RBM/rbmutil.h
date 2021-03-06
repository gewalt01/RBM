﻿#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include "StateCounter.h"
#include "Sampler.h"
#include <omp.h>

namespace rbmutil {

	// generate data from rbm
	template <class T, class STL>
	STL data_gen(T & rbm, int update_count) {
		return rbmutil::data_gen<T, STL>(rbm, update_count, std::random_device()());
	}

	template <class T, class STL>
	STL data_gen(T & rbm, int update_count, int seed) {
		Sampler<T> sampler;
		std::mt19937 mt(seed);
		sampler.randEngine = mt;

		for (int c = 0; c < update_count; c++) {
			sampler.updateByBlockedGibbsSamplingVisible(rbm);
			sampler.updateByBlockedGibbsSamplingHidden(rbm);
		}

		std::vector<double> dat(rbm.getVisibleSize());

		for (int i = 0; i < dat.size(); i++) {
			dat[i] = rbm.nodes.v(i);
		}

		return dat;
	}

	// output stl value to stdout
	template <class STL>
	void print_stl(STL & stl) {
		for (int i = 0; i < stl.size() - 1; i++) {
			std::cout << stl[i] << ", ";
		}

		std::cout << stl[stl.size() - 1] << std::endl;
	}

	// Kullback–Leibler divergence
	template <class RBM1, class RBM2, class STL>
	double kld(RBM1 & rbm1, RBM2 & rbm2, STL & v_val) {
		StateCounter<std::vector<int>> sc(std::vector<int>(rbm1.getVisibleSize(), v_val.size()));
		auto setting_data_from_state = [&](auto & state_counter, auto & dat) {
			auto state = state_counter.getState();

			for (int i = 0; i < rbm1.getVisibleSize(); i++) {
				dat[i] = v_val[state[i]];
			}
		};



		int max_count = sc.getMaxCount();
		double value = 0.0;

//#pragma omp parallel for schedule(static) reduction(+:value)
		for (int c = 0; c < max_count; c++) {
			std::vector<double> dat(rbm1.getVisibleSize());
			auto sc_replica = sc;
			sc_replica.innerCounter = c;
			setting_data_from_state(sc_replica, dat);
			auto rbm1_replica = rbm1;
			auto rbm2_replica = rbm2;
			double prob[2];
			prob[0] = rbm1_replica.probVis(dat);
			prob[1] = rbm2_replica.probVis(dat);

			value += prob[0] * log(prob[0] / prob[1]);
			if (isnan(value) || isinf(value)) {
				volatile auto debug_value = value;
				volatile auto p1 = prob[0];
				volatile auto p2 = prob[1];
				volatile auto z1 = rbm1_replica.getNormalConstant();
				volatile auto z2 = rbm2_replica.getNormalConstant();

				std::cout << "Train type:" << rbm2.trainType << std::endl;
				std::cout << "p1: " << p1 << std::endl;
				std::cout << "p2: " << p2 << std::endl;
				std::cout << "z1: " << z1 << std::endl;
				std::cout << "z2: " << z2 << std::endl;

				std::ofstream outputfile("./output/paramsdump.json", std::ios::out | std::ios::trunc);
				outputfile << rbm2_replica.params.serialize();
				outputfile.close();
				print_params(rbm2_replica);

				throw;
			}
		}

		return value;
	}

	template<class RBM>
	void print_params(RBM & rbm) {
		rbm.params.printParams();
	}
}

