#ifndef MD_DTW_H
#define MD_DTW_H

#include <vector>
#include <iomanip>

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

    MultiDimensionalTimeWarping(vector<vector<float> > &input_time_series, const vector<vector<float> > &b_line) :
        num_features_(input_time_series.size()), num_steps_(input_time_series[0].size()), time_series_(input_time_series)  {
        means_.reserve(num_features_);
        std_.reserve(num_features_);

        FillMeanAndStd();
        ApplyZeroMean();
        // FindDist() will use a vector<vector<float>> from a .h file, it is constant 
        vector<vector<float> > cheese;
        cheese = input_time_series; 
        baseline_ = b_line;
        FillMeanAndStd(baseline_);
        ApplyZeroMean(baseline_);
        // all_in_ = FindDist(input_time_series);
        cheese_ = FindDist(baseline_);
        // economic_ = FindDist(input_time_series);
        // timing_ = FindDist(input_time_series);
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
    void FillMeanAndStd() {
        for (auto vec : time_series_) {
            float mean = 0.0f;

            for (auto x : vec) {
                mean += x;
            }

            mean /= vec.size();

            means_.push_back(mean);

            // Find standard deviation (std)
            float std = 0.0f;
            for (auto x : vec) {
                std += (x - mean) * (x - mean);
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


    // Baselines will be precomputed so they don't have to run through this every time.
    // These two methods are here for trialing only.
    void FillMeanAndStd(const vector<vector<float> > &baseline) {
    	base_means_.clear();
    	base_std_.clear();

        for (auto vec : baseline) {
            float mean = 0.0f;

            for (auto x : vec) {
                mean += x;
            }

            mean /= vec.size();

            base_means_.push_back(mean);

            // Find standard deviation (std)
            float std = 0.0f;
            for (auto x : vec) {
                std += (x - mean) * (x - mean);
            }

            std /= vec.size();
            base_std_.emplace_back(sqrt(std));
        }
    }

    // Applies zero mean to each element
    void ApplyZeroMean(vector<vector<float> > &baseline) {
        for (unsigned int i = 0; i < baseline.size(); ++i) {
            vector<float>* vec = &baseline[i];
            for (unsigned int j = 0; j < (*vec).size(); ++j) {
                (*vec)[j] = ((*vec)[j] - means_[i]) / std_[i];
            }
        }
    }




    float FindDist(const vector<vector<float> > &baseline) {
        // Fill nxn matrix with inf
        vector<vector<float> > matrix(num_steps_, vector<float>(num_steps_, numeric_limits<float>::infinity()));

        // Will pull from .h file that has const vector<vector<float> for each strategy. The following line is filler:
        baseline == matrix;

        // Origin is always 0.0
        matrix[0][0] = 0.0f;

        // Do edges
        // Set first column
        for (unsigned int i = 1; i < num_steps_; ++i) {
            matrix[i][0] = GetTaxicabNorm(baseline, i, 0) + matrix[i - 1][0];
        }
        // Set first row
        for (unsigned int j = 1; j < num_steps_; ++j) {
            matrix[0][j] = GetTaxicabNorm(baseline, 0, j) + matrix[0][j - 1];
        }

        // Do remainder of matrix
        for (unsigned int i = 1; i < num_steps_; ++i) {
            for (unsigned int j = 1; j < num_steps_; ++j) {
                matrix[i][j] = GetTaxicabNorm(baseline, i, j) + min(min(matrix[i - 1][j - 1], matrix[i][j - 1]), matrix[i - 1][j]);
            }
        }

        for (auto vec : matrix)
        {
        	for (auto f : vec)
        	{
        		cout << setprecision(3) << fixed << f << "  ";
        	}
        	cout << endl;
        }


        return WalkMatrix(matrix);
    }


    // This is a p1-norm. p1-norms are better at finding paths in this regard. p2-norm may struggle in high-feature spaces
    // https://rorasa.wordpress.com/2012/05/13/l0-norm-l1-norm-l2-norm-l-infinity-norm/
    inline float GetTaxicabNorm(const vector<vector<float> > &baseline, const unsigned int &t_indx, const unsigned int &b_indx) {
        float ret_val = 0.0f;

        // For each feature in the time series calculate the taxicab norm between it and the relevant baseline
        // Ignore first entry as that is the time interval which is constant between series
        for (unsigned int i = 1; i < num_features_; ++i) {
        	ret_val += fabs(time_series_[i][t_indx] - baseline[i][b_indx]);
        }
        return ret_val;
    }

    // Start in the corner and walk the matrix looking for the smallest path
    float WalkMatrix(const vector<vector<float> > &matrix) {
        // Corner value:
        float ret_val = matrix[num_steps_ - 1][num_steps_ - 1];

        // Check each 2x2 block on the way down the matrix until we hit 0,0
        unsigned int i = num_steps_ - 1;
        unsigned int j = num_steps_ - 1;
        while (i != 0 && j != 0) {
        	// cout << "i,j: " << i << "," << j<< endl;
            float left = matrix[i][j - 1];
            float up = matrix[i - 1][j];
            float mid = matrix[i - 1][j - 1];

            // Middle move preference
            if (mid <= left) {
                if (mid <= up) {
                    --i;
                    --j;
                    ret_val += mid;
                } else {
                    --i;
                    ret_val += up;
                }
            } else if (left <= up) {
                --j;
                ret_val += left;
            } else {
                --i;
                ret_val += up;
            }
        }
        return ret_val;
    }


    vector<vector<float> > time_series_;
    vector<float> means_;
    vector<float> std_;
    const unsigned int num_features_;
    const unsigned int num_steps_;
    vector<vector<float> > baseline_;

    float all_in_ = numeric_limits<float>::infinity();
    float cheese_ = numeric_limits<float>::infinity();
    float economic_ = numeric_limits<float>::infinity();
    float timing_ = numeric_limits<float>::infinity();

    vector<float> base_means_;
    vector<float> base_std_;

};

#endif