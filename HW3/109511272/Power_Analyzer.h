#ifndef Power_Analyzer_H
#define Power_Analyzer_H


#include "parser.h"
#include <queue>
#include <iomanip>
using namespace std;
class Power_Analyzer{
    private:
        string case_name;
        ofstream fout_gate_info;
        ofstream fout_gate_power;
        ofstream fout_coverage;

        vector<Instance*> &instance;
        unordered_map<string, int> &instance_lib; // instance name and index in vector map
        unordered_map<string, Wire*> &wire_lib;
        vector<Wire*> &input_wires, &output_wires;

        double total_toggle;
        double require_toggle;
        double total_switching_power;
        int case_n;
    public:
        Power_Analyzer(Parser_v&, char *);
        ~Power_Analyzer();
        void solve_propagation();
        void solve(char *);
        double table_search(double, double, vector<vector<double>>&, vector<double>&, vector<double>&);
        void show();

        void output_loading();
        void output_gate_info();
        void output_gate_power();
        void output_total_power();
};

#endif


