#include "Power_Analyzer.h"

bool cmp(const Instance* lhs, const Instance* rhs){
    string lhs_c = lhs->name;
    string rhs_c = rhs->name;
    return stoi(lhs_c.erase(0,1)) < stoi(rhs_c.erase(0,1));
}

Power_Analyzer::Power_Analyzer(Parser_v& P, char *in) :
    instance(P.instance),
    wire_lib(P.wire_lib),
    instance_lib(P.instance_lib),
    input_wires(P.input_wires),
    output_wires(P.output_wires)
{
    case_name = in;
    case_name = case_name.substr(0, case_name.length()-2);
    sort(instance.begin(), instance.end(), cmp); // sort instance in ascending order by instance name
    for(auto& i : instance)
        i->switching_power = 0.5 * i->output_wire->capacitance * 0.81;
    // cout<<case_name<<endl;
    // solve();
    // show();

    total_toggle = 0;
    require_toggle = instance.size() * 40; // 20 for 0->1, 20 for 1->0

    string file_name_info = "109511272_" + case_name + "_gate_info.txt";
    string file_name_power = "109511272_" + case_name + "_gate_power.txt";
    string file_name_coverage = "109511272_" + case_name + "_coverage.txt";
    fout_gate_info.open(file_name_info.c_str());
    fout_gate_power.open(file_name_power.c_str());
    fout_coverage.open(file_name_coverage.c_str());
    output_loading();
}

Power_Analyzer::~Power_Analyzer(){

}

void Power_Analyzer::solve(char *input_file){
    case_n = 1;
    ifstream fin(input_file);
    string line;
    int input_wire_size = input_wires.size();
    vector<string> input_order(input_wire_size);
    while(getline(fin, line)){
        stringstream ss(line);
        ss >> line;
        if(line == "input"){
            line = ss.str();
            replace(line.begin(), line.end(), ',', ' '); 
            ss.clear();
            ss.str(line);
            ss >> line;
            // get input order
            for(int i=0;i<input_wire_size;i++)
                ss >> input_order[i]; 
        }
        else if(line == ".end"){
            break;
        }
        else{ // solve pattern
            // get input wire logic patterns
            wire_lib[input_order[0]]->logic_value = (line=="1");
            for(int i=1;i<input_wire_size;i++){
                ss >> wire_lib[input_order[i]]->logic_value; // = (c=='1');
            }
            solve_propagation(); // propagate from input to output to get instance info

            output_gate_info();
            output_gate_power();
            output_total_power();
            case_n++;
        }
    }
}

void Power_Analyzer::solve_propagation(){
    // using dp to calculate propagation delay and transition delay in topological order
    total_switching_power = 0;
    queue<Instance*> q; // queue for instance that has updated all transition delay
    
    //start from input nets
    for(auto& cur_wire : input_wires){
        cur_wire->delay = 0;

        for(auto& instance_idx : cur_wire->output_instance){
            Instance *cur_instance = instance[instance_idx];
            cur_instance->update_time++;
            cur_instance->input_transition = 0;
            cur_instance->delay = 0;

            bool controlling_logic = cur_instance->cell_ptr->name == "NOR2X1";
            cur_instance->prev_sensitive = (cur_wire->logic_value == controlling_logic);
            
            if(cur_instance->update_time == cur_instance->input_wire.size()){
                q.push(cur_instance);
            }
        }
    }

    // calculate instance delay in topological order
    while(!q.empty()){
        Instance *cur_instance = q.front();
        q.pop();

        cur_instance->update_time = 0; // reset 
        bool prev_logic = cur_instance->output_logic;
        double output_capacitance = cur_instance->output_wire->capacitance;
        double input_transition = cur_instance->input_transition;
        // update logic value
        if(cur_instance->cell_ptr->name == "INVX1"){
            cur_instance->output_logic = !(cur_instance->input_wire[0]->logic_value);
        }
        else if(cur_instance->cell_ptr->name == "NOR2X1"){
            cur_instance->output_logic = !(cur_instance->input_wire[0]->logic_value || cur_instance->input_wire[1]->logic_value);
        }
        else{
            cur_instance->output_logic = !(cur_instance->input_wire[0]->logic_value && cur_instance->input_wire[1]->logic_value);
        }

        // get transition and propagation delay
        if(cur_instance->output_logic){
            cur_instance->propagation_time = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["cell_rise"],
                                                cur_instance->cell_ptr->table_template_lib["cell_rise"]->x,
                                                cur_instance->cell_ptr->table_template_lib["cell_rise"]->y);          
            cur_instance->transition_time = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["rise_transition"],
                                                cur_instance->cell_ptr->table_template_lib["rise_transition"]->x,
                                                cur_instance->cell_ptr->table_template_lib["rise_transition"]->y);
            cur_instance->internal_power = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["rise_power"],
                                                cur_instance->cell_ptr->table_template_lib["rise_power"]->x,
                                                cur_instance->cell_ptr->table_template_lib["rise_power"]->y);
        }
        else{
            cur_instance->propagation_time = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["cell_fall"],
                                                cur_instance->cell_ptr->table_template_lib["cell_fall"]->x,
                                                cur_instance->cell_ptr->table_template_lib["cell_fall"]->y);
            cur_instance->transition_time = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["fall_transition"],
                                                cur_instance->cell_ptr->table_template_lib["fall_transition"]->x,
                                                cur_instance->cell_ptr->table_template_lib["fall_transition"]->y); 
            cur_instance->internal_power = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["fall_power"],
                                                cur_instance->cell_ptr->table_template_lib["fall_power"]->x,
                                                cur_instance->cell_ptr->table_template_lib["fall_power"]->y);
        }
        double out_delay = cur_instance->delay + cur_instance->propagation_time;

        // update power and toggle for current pattern
        if(prev_logic != cur_instance->output_logic){
            if(prev_logic){ // 1->0
                if(cur_instance->toggle_time_f < 20){
                    total_toggle++;
                    cur_instance->toggle_time_f++;
                }
            }
            else{
                if(cur_instance->toggle_time_r < 20){
                    total_toggle++;
                    cur_instance->toggle_time_r++;
                }
            }
            total_switching_power += cur_instance->switching_power;
        }

        total_switching_power += cur_instance->internal_power;

        // update output wire info
        cur_instance->output_wire->logic_value = cur_instance->output_logic;
        cur_instance->output_wire->delay = out_delay;
        // update output instance
        for(auto& out_idx : cur_instance->output_wire->output_instance){
            Instance *out_instance = instance[out_idx];
            out_instance->update_time++;
            if(out_instance->cell_ptr->name == "INVX1"){
                out_instance->input_transition = cur_instance->transition_time;
                out_instance->delay = out_delay;
            }
            else if(out_instance->cell_ptr->name == "NOR2X1" || out_instance->cell_ptr->name == "NANDX1"){
                bool controlling_logic = out_instance->cell_ptr->name == "NOR2X1"; // for NOR = 1, for NAND = 0
                if(out_instance->update_time == 1){
                    out_instance->input_transition = cur_instance->transition_time;
                    out_instance->prev_sensitive = (cur_instance->output_logic == controlling_logic);
                    out_instance->delay = out_delay; // input delay of out instance
                }
                else{
                    if(out_instance->prev_sensitive && cur_instance->output_logic == controlling_logic){
                        // cout << out_instance->name << endl;
                        if(out_delay < out_instance->delay){ // cur instance delay < prev instance delay => change transition time by earliest controlling value
                            out_instance->input_transition = cur_instance->transition_time;
                            out_instance->delay = out_delay;
                        }
                    }
                    else if(cur_instance->output_logic == controlling_logic){ // first controlling value
                        out_instance->input_transition = cur_instance->transition_time;
                        out_instance->delay = out_delay;
                    }
                    else if(!out_instance->prev_sensitive && cur_instance->output_logic != controlling_logic){
                        if(out_delay > out_instance->delay){ // both no controllong => use longest
                            out_instance->input_transition = cur_instance->transition_time;
                            out_instance->delay = out_delay;
                        }
                    }
                }
            }
            if(out_instance->update_time == out_instance->input_wire.size()){
                q.push(out_instance);
            }
        }
    }
}

double Power_Analyzer::table_search(double S, double C, vector<vector<double>>& table, vector<double>& x, vector<double>& y){
    // input transition time, output capacitance, table for search, x label, y label
    int s1, s2, c1, c2;
    // get s1, s2
    s1 = 0; s2 =1;
    for(s1=0;s1<y.size()-1;s1++){
        if((s1 == 0 && S < y[s1]) || (S >= y[s1] && S<y[s1+1]))
            break;
    }

    if(s1 == y.size()-1)
        s1 = y.size() - 2;
    s2 = s1+1;

    // get c1, c2
    c1 = 0; c2 =1;
    for(c1=0;c1<x.size()-1;c1++)
        if((c1 == 0 && C < x[c1]) || (C >= x[c1] && C < x[c1+1]))
            break;
    if(c1 == x.size()-1)
        c1 = x.size() - 2;
    c2 = c1+1;

    //-------
    double A = table[s1][c1] + (table[s2][c1] - table[s1][c1]) / (y[s2] - y[s1]) * (S - y[s1]);
    double B = table[s1][c2] + (table[s2][c2] - table[s1][c2]) / (y[s2] - y[s1]) * (S - y[s1]);
    return max(0.0, A + (B - A)/(x[c2] - x[c1]) * (C - x[c1]));
}

void Power_Analyzer::show(){
    for(auto& cur_instance : instance){
        cout << cur_instance->name << endl;
        cout <<"\tworst case output : " << cur_instance->output_logic << endl;
        cout <<"\tworst propagation delay : " << cur_instance->propagation_time << endl;
        cout <<"\tworst transition delay : " << cur_instance->transition_time<<endl<<endl;
    }
}

void Power_Analyzer::output_loading(){
    string file_name = "109511272_" + case_name + "_load.txt";
    ofstream fout(file_name.c_str());
    for(auto& p : instance){
        fout << setprecision(6) << fixed << p->name << " " << p->output_wire->capacitance << endl;
    }
}

void Power_Analyzer::output_gate_info(){
    for(auto& p : instance){
        fout_gate_info << setprecision(6) << fixed << p->name << " " << p->output_logic
             << " " << p->propagation_time << " " << p->transition_time << endl;
    }
    fout_gate_info << endl;
}

void Power_Analyzer::output_gate_power(){
    for(auto& p : instance){
        fout_gate_power << setprecision(6) << fixed << p->name << " " << p->internal_power
             << " " << p->switching_power << endl;
    }
    fout_gate_power << endl;
}

void Power_Analyzer::output_total_power(){
    fout_coverage << case_n << " " << setprecision(6) << fixed << total_switching_power 
                     << " " << setprecision(2) << fixed << total_toggle / require_toggle * 100 << "%" << endl;
    fout_coverage << endl;
}