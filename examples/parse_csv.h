#ifndef PARSE_CSV_H
#define PARSE_CSV_H


vector<string> GetLines(string filename)
{
    vector<string> ret_val;

    ifstream fin;
    fin.open(filename.c_str());
    string line;

    getline(fin, line); // ignore headers
    while (getline(fin, line))
        ret_val.push_back(line);

    return ret_val;
}



#endif