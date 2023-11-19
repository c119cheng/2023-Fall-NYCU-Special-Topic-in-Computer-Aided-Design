#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
using namespace std;

#include "instance.h"
class STA;
class Parser_v;
class Parser_lib{
    private:
        char *file_name;
        unordered_map<string, Cell*> cell_lib; // cell name -> cell info
        unordered_map<string, lu_table_template*> lu_table_template_lib;
    public:
        Parser_lib(char *);
        ~Parser_lib();
        void build_cell();
        void parse_template(ifstream&, string);
        void parse_cell(ifstream&, string); // parse cell
        void parse_pin(ifstream&, Cell*, string&);
        void parse_power(ifstream&, Cell*);
        void parse_timing(ifstream&, Cell*);
        void parse_value(ifstream&, vector<vector<double>>&);

        void show();
        friend class Parser_v;
        friend class STA;
};

class Parser_v{
    private:
        char *file_name;
        Parser_lib& lib;
        stringstream ss;
        vector<Wire*> wires;
        vector<Instance*> instance;
        unordered_map<string, int> wire_lib, instance_lib; // instance name and index in vector map
        vector<int> input_wires, output_wires;
    public:
        Parser_v(char *, Parser_lib&);
        ~Parser_v();
        void preprocess();
        void built_Instance();
        void show_netlist();

    friend class STA;
};

#endif