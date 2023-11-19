#ifndef STA_H
#define STA_H


#include "parser.h"
#include <queue>
#include <iomanip>
using namespace std;
class STA{
    private:
        string case_name;
        vector<Wire*> &wires;
        vector<Instance*> &instance;
        unordered_map<string, int> &wire_lib, &instance_lib; // instance name and index in vector map
        vector<int> &input_wires, &output_wires;
    public:
        STA(Parser_v&, char *);
        ~STA();
        void solve();
        double table_search(double, double, vector<vector<double>>&, vector<double>&, vector<double>&);
        void show();

        void output_loading();
        void output_path();
        void output_delay();
};

#endif


