#ifndef GET_FILE_NAMES_H
#define GET_FILE_NAMES_H

#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <iostream>

using namespace std;

string Filter(string Filter = "")
{
    static string filter = Filter;
    return filter;
}

void GetFileNames(string origin, vector<string> &files)
{
    
    DIR *directory_path;
    struct dirent *dir_path;
    if ((directory_path = opendir(origin.c_str())) != NULL)
    {
        string filter = Filter();
        while ((dir_path = readdir(directory_path)) != NULL)
        {
            string file_name = string(dir_path->d_name);

            // If a filter was set, only add filtered 
            if (filter.size() != 0)
            {
                if (file_name.find(filter) != string::npos)
                {
                    files.push_back(file_name);
                }
            }
            // Else add all
            else
            {
                files.push_back(file_name);
            }
            
        }
        closedir(directory_path);
    }
}



#endif