#include "get_file_names.h"
#include "parse_csv.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include "md_dtw.h"

using namespace std;

int main(int argc, char** argv)
{
    string folder_name;
    if (argc > 2) 
        folder_name = string(argv[1]);
    else
        folder_name = "/home/kyle/Documents/DTWtest/";
    vector<string> csv_files;
    cout << "Enter number of columns: ";
    int cols;
    cin >> cols;
    Filter(".csv");
    GetFileNames(folder_name, csv_files);
    vector<vector<vector <float> > > strats;

        vector<string> linez = GetLines(folder_name + csv_files[1]);
        vector<string> lineb = GetLines(folder_name + csv_files[0]);

        vector<vector<float> > time_seriesa(cols, vector<float>(linez.size(), 0.0f));
        vector<vector<float> > baseline(cols, vector<float>(lineb.size(), 0.0f));


        for (unsigned int i = 0; i < linez.size(); ++i)
        {
            // From the end of the line walk backwards looking for commas
            // Split that bit off and add to the appropriate vector.
            // Skips first column
            size_t pos = 0;            
            for (int j = cols - 1; j > 0; --j)
            {
                pos = linez[i].rfind(",");
                time_seriesa[j][i] = stof(linez[i].substr(pos + 1));
                linez[i].resize(pos);
            }
            // Deal with the first column
            time_seriesa[0][i] = stof(linez[i]);
        }

        for (unsigned int i = 0; i < lineb.size(); ++i)
        {
            // From the end of the line walk backwards looking for commas
            // Split that bit off and add to the appropriate vector.
            // Skips first column
            size_t pos = 0;            
            for (int j = cols - 1; j > 0; --j)
            {
                pos = lineb[i].rfind(",");
                baseline[j][i] = stof(lineb[i].substr(pos + 1));
                lineb[i].resize(pos);
            }
            // Deal with the first column
            baseline[0][i] = stof(lineb[i]);
        }

        // for (unsigned int i = 0; i < cols; ++i)
        // {
        //     for (unsigned int j = 0; j < linez.size() - 1; ++j)
        //     {
        //         cout << time_seriesa[i][j] << ",";
        //     }
        //     cout << time_seriesa[i][linez.size() - 1];
        //     cout << endl;
        // }
        // cout << endl;


        // for (unsigned int i = 0; i < cols; ++i)
        // {
        //     for (unsigned int j = 0; j < lineb.size() - 1; ++j)
        //     {
        //         cout << baseline[i][j] << ",";
        //     }
        //     cout << baseline[i][lineb.size() - 1];
        //     cout << endl;
        // }
        // cout << endl;


        MultiDimensionalTimeWarping md_dtw(time_seriesa, baseline);
        cout << md_dtw.GetCheese() << endl;

    return 0;
}