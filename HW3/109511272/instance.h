#ifndef INSTANCE_H
#define INSTANCE_H
#include <string>
#include <unordered_map>
#include <float.h>

using namespace std;

enum CELL_TYPE {INVX1, NANDX1, NOR2X1};
struct Wire;
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
    // add a reference to capacitance
    string name;
    Cell *cell_ptr;
    vector<Wire*> input_wire;
    bool input_logic[2];

    Wire* output_wire;

    int update_time = 0; // how many time does transition time update
    bool prev_sensitive; // previous input logic is snesitive by path sensitization
    double input_transition = 0;

    // double TPLH; // propation delay LOW -> HIGH
    // double TPHL; //propation delay HIGH -> LOW
    // double TR; // rise time
    // double TF; // fall time
    double transition_time;
    double propagation_time;

    bool output_logic;
    double delay; // input delay

    // power
    double internal_power;
    double switching_power;

    // toggle
    int toggle_time_r; // 0->1
    int toggle_time_f; // 1->0
};

struct Wire{
    string name;
    double capacitance = 0;
    int input_instance;
    vector<int> output_instance;

    bool logic_value;
    double delay;
};

#endif