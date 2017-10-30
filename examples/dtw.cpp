#include "get_file_names.h"
#include "parse_csv.h"
#include <iostream>
#include <limits>
#include <algorithm>

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


        vector<vector<float> > cheese;
        cheese.reserve(cols);
        vector<string> linez = GetLines(folder_name + csv_files[1]);

        for (auto v : cheese)
            v.reserve(linez.size());

        for (auto line : linez)
        {
            // From the end of the line walk backwards looking for commas
            // Split that bit off and add to the appropriate vector.
            // Skips first column
            size_t pos = 0;
            vector<float> row(cols, 0.0f);
            for (int i = cols - 1; i > 0; --i)
            {
                pos = line.rfind(",");
                row[i] = stof(line.substr(pos + 1));
                // cheese[i].emplace_back(stof(line.substr(pos + 1)));
                line.resize(pos);
            }
            // Deal with the first column
            row[0] = stof(line);
            cheese.push_back(row);
            // columns[0].push_back(stof(line));

        }

    for (string file : csv_files)
    {
        vector<string> lines = GetLines(folder_name + file);
        unsigned int n = lines.size();
        if (n == 0)
            continue;
        // Each file has a vector of vectors so it can be compared against the different
        // strategy's vector of vectors
        vector<vector<float> > columns;
        columns.reserve(cols);
        // vector<vector<float> > cheese;
        // cheese.reserve(cols);

        for (auto v : columns)
            v.reserve(n);

        for (auto line : lines)
        {
            // From the end of the line walk backwards looking for commas
            // Split that bit off and add to the appropriate vector.
            // Skips first column
            size_t pos = 0;
            vector<float> row(cols, 0.0f);
            for (int i = cols - 1; i > 0; --i)
            {
                pos = line.rfind(",");
                row[i] = stof(line.substr(pos + 1));
                // cheese[i].emplace_back(stof(line.substr(pos + 1)));
                line.resize(pos);
            }
            // Deal with the first column
            row[0] = stof(line);
            // cheese.push_back(row);
            columns.push_back(row);
            // columns[0].push_back(stof(line));

        }

        // Debug print start
            // for (unsigned int i = 0; i < n; ++i)
            // {
            //     for (unsigned int j = 0; j < cols - 1; ++j)
            //     {
            //         cout << columns[i][j] << ",";
            //     }
            //     cout << columns[i][cols - 1];
            //     cout << endl;
            // }
            // cout << endl;
        // Debug end
        // Initialize matrix to infinity
        

        // For each possible strat
        // for (unsigned int m = 0; m < strats.size(); ++m)    // use foreach
            
            vector<vector<float> > matrix(n, vector<float>(n, numeric_limits<float>::infinity()));

            // For each dimension
            for (unsigned int k = 1; k < cols; ++k)
            {
                // cout << "matrix size: " << matrix.size() << endl;
                // Do edges
                matrix[0][0] = 0.0f;
                // Set first column
                for (unsigned int i = 1; i < n; ++i)
                {
                    matrix[i][0] = fabs(cheese[i][k] - columns[0][k]) + matrix[i-1][0];
                }
                // Set first row
                for (unsigned int j = 1; j < n; ++j)
                {
                    matrix[0][j] = fabs(cheese[0][k] - columns[j][k]) + matrix[0][j -1];
                }

                // Do remainder of matrix
                for (unsigned int i = 1; i < n; ++i)
                {
                    for (unsigned int j = 1; j < n; ++j)
                    {
                        matrix[i][j] = fabs(cheese[i][k] - columns[j][k]) + min(min(matrix[i-1][j-1],matrix[i][j-1]),matrix[i-1][j]);
                    }
                }
            }

            // Debug Print
                for (unsigned int i = 0; i < n; ++i)
                {
                    for (unsigned int j = 0; j < n; ++j)
                    {
                        cout << matrix[i][j] << " ";
                    }
                    cout << endl;
                }
            // Debug end

        
        return 0;   // temp
    }

    return 0;
}