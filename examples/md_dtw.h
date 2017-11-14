#ifndef MD_DTW_H
#define MD_DTW_H

#include <vector>
#include <iomanip>
#include <algorithm>

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

    MultiDimensionalTimeWarping(vector<vector<float> > input_time_series, vector<vector<float> > all_in, 
                                vector<vector<float> > cheese, vector<vector<float> > economic, vector<vector<float> > timing) :
        num_features_(input_time_series.size()), num_steps_(input_time_series[0].size()),
        time_series_(input_time_series), all_in_(all_in), cheese_(cheese), economic_(economic), timing_(timing) {

        // cout << "BSIZE: " << b_line.size() << endl;
        // baseline_ = b_line;

        // std::cout << std::endl << std::endl << "START time_series_: \n" ;
        // cout << "time_series_[0].size(): " << time_series_[0].size() <<endl;
        // for (unsigned int i = 0; i < time_series_.size(); ++i)
        // {
        //     for (unsigned int j = 0; j < time_series_[0].size() - 1; ++j)
        //     {
        //         std::cout << setprecision(3) << fixed << time_series_[i][j] << ",";
        //     }
        //     std::cout << setprecision(3) << fixed << time_series_[i][time_series_[0].size() - 1];
        //     std::cout << std::endl;
        // }
        // std::cout << std::endl << std::endl << "END time_series_: \n" ;
        ApplyWeights(time_series_);
        ApplyWeights(all_in_);
        ApplyWeights(cheese_);
        ApplyWeights(economic_);
        ApplyWeights(timing_);


        means_.reserve(num_features_);
        std_.reserve(num_features_);

        FillMeanAndStd();
        ApplyZeroMean();
        // FindDist() will use a vector<vector<float>> from a .h file, it is constant
        // vector<vector<float> > cheese;
        // cheese = input_time_series;
        // baseline_ = b_line;

        FillMeanAndStd(all_in_);
        ApplyZeroMean(all_in_);
        all_in_dist_ = FindDist(all_in_);

        FillMeanAndStd(cheese_);
        ApplyZeroMean(cheese_);
        cheese_dist_ = FindDist(cheese_);

        FillMeanAndStd(economic_);
        ApplyZeroMean(economic_);
        economic_dist_ = FindDist(economic_);

        FillMeanAndStd(timing_);
        ApplyZeroMean(timing_);
        timing_dist_ = FindDist(timing_);
        // all_in_ = FindDist(input_time_series);
        
        // economic_ = FindDist(input_time_series);
        // timing_ = FindDist(input_time_series);
    }


    float GetAllIn() {
        return all_in_dist_;
    }
    float GetCheese() {
        return cheese_dist_;
    }
    float GetEconomic() {
        return economic_dist_;
    }
    float GetTiming() {
        return timing_dist_;
    }

    string MostLikely()
    {
        float distances[] = {all_in_dist_, cheese_dist_, economic_dist_, timing_dist_};
        unsigned int smallest = 4;

        float smallest_val = 999999999999;

        for (unsigned int i = 1; i < 4; ++i)
        {
            if (distances[i] < smallest_val)
            {
                smallest = i;
                smallest_val = distances[i];
            }
        }

        switch(smallest)
        {
            case 0:
                return "All_In";
            break;
            case 1:
                return "Cheese";
            break;
            case 2:
                return "Economic";
            break;
            case 3:
                return "Timing";
            break;
            case 4:
                return "";
            break;
        }
    }


  private:

    void ApplyWeights(std::vector<std::vector<float> > &data)
    {
        for (unsigned int i = 0; i < num_features_; ++i)
        {
            for (unsigned int j = 0; j < data[0].size(); ++j)
            {
                data[i][j] *= weights_[i];
            }
        }
    }


    // Calculates mean for all features. Adds mean to means_ vector for
    // use in zero-mean calculation
    void FillMeanAndStd() {
        for (auto vec : time_series_) {
            float mean = 0.0f;

            for (auto x : vec) {
                mean += x;
            }
            if (mean > 0.001f) {
                mean /= vec.size();
                means_.push_back(mean);

                // Find standard deviation (std)
                float std = 0.0f;
                for (auto x : vec) {
                    std += (x - mean) * (x - mean);
                }
                std /= vec.size();
                std_.emplace_back(sqrt(std));

            } else {
                means_.push_back(mean);
                std_.emplace_back(0.0f);
            }
        }
    }


    // Applies zero mean to each element
    void ApplyZeroMean() {
        for (unsigned int i = 0; i < time_series_.size(); ++i) {
            vector<float>* vec = &time_series_[i];
            for (unsigned int j = 0; j < (*vec).size(); ++j) {
                if (std_[i] < 0.001f) {
                    (*vec)[j] = 0.0f;
                } else {
                    (*vec)[j] = ((*vec)[j] - means_[i]) / std_[i];
                }
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
            if (mean > 0.001f) {
                mean /= vec.size();
                base_means_.push_back(mean);

                // Find standard deviation (std)
                float std = 0.0f;
                for (auto x : vec) {
                    std += (x - mean) * (x - mean);
                }
                std /= vec.size();
                base_std_.emplace_back(sqrt(std));

            } else {
                base_means_.push_back(mean);
                base_std_.emplace_back(0.0f);
            }
        }
    }

    // Applies zero mean to each element
    void ApplyZeroMean(vector<vector<float> > &baseline) {
        for (unsigned int i = 0; i < baseline.size(); ++i) {
            vector<float>* vec = &baseline[i];
            for (unsigned int j = 0; j < (*vec).size(); ++j) {
                if (base_std_[i] < 0.001f) {
                    (*vec)[j] = 0.0f;
                } else {
                    (*vec)[j] = ((*vec)[j] - base_means_[i]) / base_std_[i];
                }
            }
        }
    }




    float FindDist(const vector<vector<float> > &baseline) {
        // Fill nxn matrix with inf
        const unsigned int n = num_steps_ + 1;
        vector<vector<float> > matrix(n, vector<float>(n, numeric_limits<float>::infinity()));

        // Will pull from .h file that has const vector<vector<float> for each strategy. The following line is filler:
        // baseline == matrix;

        // Origin is always 0.0
        matrix[0][0] = 0.0f;

        // Do edges
        // Set first column
        for (unsigned int i = 1; i < n; ++i) {
            matrix[i][0] = GetTaxicabNorm(baseline, i - 1, 0) + matrix[i - 1][0];
        }

        // Set first row
        for (unsigned int j = 1; j < n; ++j) {
            matrix[0][j] = GetTaxicabNorm(baseline, 0, j - 1) + matrix[0][j - 1];
        }

        // Do remainder of matrix
        for (unsigned int i = 1; i < n; ++i) {
            for (unsigned int j = 1; j < n; ++j) {
                matrix[i][j] = GetTaxicabNorm(baseline, i, j) + min({matrix[i - 1][j - 1], matrix[i][j - 1], matrix[i - 1][j]});
            }
        }

        // cout << endl << endl;
        // for (auto vec : matrix) {
        //     for (auto f : vec) {
        //         printf("%8.3f  ", f);
        //     }
        //     cout << endl;
        // }


        return WalkMatrix(matrix);
    }


    // This is a p1-norm. p1-norms are better at finding paths in this regard. p2-norm may struggle in high-feature spaces
    // https://rorasa.wordpress.com/2012/05/13/l0-norm-l1-norm-l2-norm-l-infinity-norm/
    float GetTaxicabNorm(const vector<vector<float> > &baseline, const unsigned int &t_indx, const unsigned int &b_indx) {
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
        float ret_val = matrix[num_steps_ ][num_steps_ ];

        // Check each 2x2 block on the way down the matrix until we hit 0,0
        unsigned int i = num_steps_ - 1;
        unsigned int j = num_steps_ - 1;
        while (i != 0 && j != 0) {
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
    vector<vector<float> > all_in_;
    vector<vector<float> > cheese_;
    vector<vector<float> > economic_;
    vector<vector<float> > timing_;

    float all_in_dist_ = numeric_limits<float>::infinity();
    float cheese_dist_ = numeric_limits<float>::infinity();
    float economic_dist_ = numeric_limits<float>::infinity();
    float timing_dist_ = numeric_limits<float>::infinity();

    vector<float> base_means_;
    vector<float> base_std_;

    float weights_[9] = {1.0, 1.1, 1.2, 1.2, 1.1, 1.0, 1.0, 1.3, 1.1};

};

#endif