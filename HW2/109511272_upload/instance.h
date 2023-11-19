#ifndef INSTANCE_H
#define INSTANCE_H
#include <string>
#include <unordered_map>
#include <float.h>

using namespace std;

enum CELL_TYPE {INVX1, NANDX1, NOR2X1};
struct lu_table_template{
    string name;
    int num_row, num_col;
    vector<double> x, y;
};

struct Pin{
    string name;
    bool input;
    double capacitance;
};

struct Cell{
    string name;
    unordered_map<string, Pin> pin_lib; // Pin name -> pin info

    unordered_map<string, vector<vector<double>> > table_lib; // table for timing and power
    unordered_map<string, lu_table_template*> table_template_lib; // find correspond table template
};

struct Instance{
    string name;
    Cell *cell_ptr;
    vector<int> input_wire;

    int output_wire;
    CELL_TYPE cell_type; // 0->INV 1->NAND 2->NOR

    int update_time = 0; // how many time does transition time update
    double input_transition = 0;

    double TPLH; // propation delay LOW -> HIGH
    double TPHL; //propation delay HIGH -> LOW
    double TR; // rise time
    double TF; // fall time
    double transition_time;
    double propagation_time;
    bool worst_case_out;

    double longest_delay = DBL_MIN; // arrival
};

struct Wire{
    string name;
    double capacitance = 0;
    int input_cell;
    vector<int> output_cell;

    double longest_delay; // arrival
};

#endif