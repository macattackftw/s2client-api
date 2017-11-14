#ifndef PARSE_CSV_H
#define PARSE_CSV_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

std::vector<std::string> GetLines(std::string filename)
{
    std::vector<std::string> ret_val;

    std::ifstream fin;
    fin.open(filename.c_str());
    std::string line;
    
    getline(fin, line); // ignore headers
    while (getline(fin, line))
        ret_val.push_back(line);
    fin.close();
    return ret_val;
}



#endif