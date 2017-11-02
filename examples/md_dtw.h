#ifndef MD_DTW_H
#define MD_DTW_H

#include <vector>

using namespace std;

/*
	Previous work I was storing vectors of vectors where the vector
	length was k (features). Going to change that so each vector is
	length n and there are k such vectors. This should make traversal
	simpler.

	I am utilizing the following as a reference:
	http://insy.ewi.tudelft.nl/sites/default/files/DTW-vASCI.pdf


*/

class MultiDimensionalTimeWarping {
  public:

	MultiDimensionalTimeWarping(vector<vector<float> > &input_time_series) :
		num_features_(input_time_series.size()), num_steps_(input_time_series[0].size()), time_series_(input_time_series) {
			CalculateMeanAndStd();
			ApplyZeroMean();
	}


	float GetAllIn() {
		return all_in_;
	}
	float GetCheese() {
		return cheese_;
	}
	float GetEconomic() {
		return economic_;
	}
	float GetTiming() {
		return timing_;
	}


  private:


	// Calculates mean for all features. Adds mean to means_ vector for
	// use in zero-mean calculation
	void CalculateMeanAndStd() {
		for (auto vec : time_series_) {
			float mean = 0.0f;

			for (auto f : vec) {
				mean += f;
			}

			mean /= vec.size();

			means_.push_back(mean);

			// Find standard deviation (std)
			float std = 0.0f;
			for (auto f : vec) {
				std += (f - mean) * (f - mean);
			}

			std /= vec.size();
			std_.emplace_back(sqrt(std));
		}
	}


	// Applies zero mean to each element
	void ApplyZeroMean() {
		for (unsigned int i = 0; i < time_series_.size(); ++i) {
			vector<float>* vec = &time_series_[i];
			for (unsigned int j = 0; j < (*vec).size(); ++j) {
				(*vec)[j] = ((*vec)[j] - means_[i]) / std_[i];
			}
		}
	}


	void FindDist() {
		vector<vector<float> > matrix(num_steps_, vector<float>(num_steps_, numeric_limits<float>::infinity()));


	}



	vector<vector<float> > time_series_;
	vector<float> means_;
	vector<float> std_;
	const unsigned int num_features_;
	const unsigned int num_steps_;

	float all_in_ = numeric_limits<float>::infinity();
	float cheese_ = numeric_limits<float>::infinity();
	float economic_ = numeric_limits<float>::infinity();
	float timing_ = numeric_limits<float>::infinity();

};

#endif