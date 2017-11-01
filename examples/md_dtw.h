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

	MultiDimensionalTimeWarping(const vector<vector<float> > &input_time_series) {
		time_series_ = input_time_series;
	}








  private:


	// Calculates mean for a given vector of floats. Adds mean to means_ vector for
	// use in zero-mean calculation
	void CalculateMeanAndStd(const vector<float> &feature) {
		float mean = 0.0f;

		for (auto f : feature) {
			mean += f;
		}

		mean /= feature.size();

		means_.push_back(mean);

		// Find standard deviation (std)
		float std = 0.0f;
		for (auto f : feature) {
			std += (f - mean) * (f - mean);
		}

		std /= feature.size();
		std_.emplace_back(sqrt(std))
	}

	inline float CalculateZeroMean(const float &x, const int &index) {
		return (x - means_[index]) / std_[index];
	}






	vector<vector<float> > time_series_;
	vector<float> means_;
	vector<float> std_;

};

#endif