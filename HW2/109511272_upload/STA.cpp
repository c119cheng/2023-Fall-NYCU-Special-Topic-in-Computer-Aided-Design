#include "STA.h"

STA::STA(Parser_v& P, char *in) :
    wires(P.wires),
    instance(P.instance),
    wire_lib(P.wire_lib),
    instance_lib(P.instance_lib),
    input_wires(P.input_wires),
    output_wires(P.output_wires)
{
    case_name = in;
    case_name = case_name.substr(0, case_name.length()-2);
    // cout<<case_name<<endl;
    solve();
    // show();
}

STA::~STA(){

}

void STA::solve(){
    // using dp to calculate propagation delay and transition delay in topological order
    
    queue<Instance*> q; // queue for instance that has updated all transition delay
    
    //start from input nets
    for(const auto& input_idx : input_wires){
        Wire* cur_wire = wires[input_idx];
        cur_wire->longest_delay = 0;

        for(auto& instance_idx : cur_wire->output_cell){
            Instance *cur_instance = instance[instance_idx];
            cur_instance->update_time++;
            cur_instance->input_transition = 0;
            cur_instance->longest_delay = 0;
            if(cur_instance->update_time == cur_instance->input_wire.size()){
                q.push(cur_instance);
            }
        }
    }

    // calculate instance delay in topological order
    while(!q.empty()){
        Instance *cur_instance = q.front();
        q.pop();

        double output_capacitance = wires[cur_instance->output_wire]->capacitance;
        double input_transition = cur_instance->input_transition;

        cur_instance->TPLH = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["cell_rise"],
                                            cur_instance->cell_ptr->table_template_lib["cell_rise"]->x,
                                            cur_instance->cell_ptr->table_template_lib["cell_rise"]->y);
        cur_instance->TPHL = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["cell_fall"],
                                            cur_instance->cell_ptr->table_template_lib["cell_fall"]->x,
                                            cur_instance->cell_ptr->table_template_lib["cell_fall"]->y);       
        cur_instance->TR = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["rise_transition"],
                                            cur_instance->cell_ptr->table_template_lib["rise_transition"]->x,
                                            cur_instance->cell_ptr->table_template_lib["rise_transition"]->y);
        cur_instance->TF = table_search(input_transition, output_capacitance, cur_instance->cell_ptr->table_lib["fall_transition"],
                                            cur_instance->cell_ptr->table_template_lib["fall_transition"]->x,
                                            cur_instance->cell_ptr->table_template_lib["fall_transition"]->y); 

        cur_instance->propagation_time = max(cur_instance->TPLH, cur_instance->TPHL);
        cur_instance->worst_case_out = (cur_instance->TPLH > cur_instance->TPHL);
        cur_instance->transition_time = (cur_instance->worst_case_out) ? cur_instance->TR : cur_instance->TF;
        double out_longest = cur_instance->longest_delay + cur_instance->propagation_time;

        // update output wire info
        Wire *out_wire = wires[cur_instance->output_wire];
        out_wire->longest_delay = out_longest;

        // update output instance
        for(auto& out_idx : out_wire->output_cell){
            Instance *out_instance = instance[out_idx];
            out_instance->update_time++;
            if(out_longest + 0.005 > out_instance->longest_delay){
                out_instance->input_transition = cur_instance->transition_time;
                out_instance->longest_delay = out_longest + 0.005;
            }
            if(out_instance->update_time == out_instance->input_wire.size()){
                q.push(out_instance);
            }
        }
    }
}

double STA::table_search(double S, double C, vector<vector<double>>& table, vector<double>& x, vector<double>& y){
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
    return A + (B - A)/(x[c2] - x[c1]) * (C - x[c1]);
}

void STA::show(){
    for(auto& cur_instance : instance){
        cout << cur_instance->name << endl;
        cout <<"\tworst case output : " << cur_instance->worst_case_out << endl;
        cout <<"\tworst propagation delay : " << cur_instance->propagation_time << endl;
        cout <<"\tworst transition delay : " << cur_instance->transition_time<<endl<<endl;
    }
}

bool cmp(const pair<string, double>& lhs, const pair<string, double>& rhs){
    if(lhs.second != rhs.second)
        return lhs.second > rhs.second;
    string lhs_c = lhs.first;
    string rhs_c = rhs.first;
    return stoi(lhs_c.erase(0,1)) < stoi(rhs_c.erase(0,1));
}

void STA::output_loading(){
    vector<pair<string, double>> v;
    for(auto& instance_ptr : instance){
        v.push_back(pair<string, double>{instance_ptr->name, wires[instance_ptr->output_wire]->capacitance});
    }
    sort(v.begin(), v.end(), cmp);
    string file_name = "109511272_" + case_name + "_load.txt";
    ofstream fout(file_name.c_str());
    for(auto& p : v){
        fout << setprecision(6) << fixed << p.first << " " << p.second << endl;
    }
}

void STA::output_delay(){
    vector<pair<string, double>> v;
    for(auto& instance_ptr : instance){
        v.push_back(pair<string, double>(instance_ptr->name, instance_ptr->propagation_time));
    }
    sort(v.begin(), v.end(), cmp);
    string file_name = "109511272_" + case_name + "_delay.txt";
    ofstream fout(file_name.c_str());

    for(auto& p : v){
        Instance *instance_ptr = instance[instance_lib[p.first]];
        fout << setprecision(6) << fixed << p.first << " " << instance_ptr->worst_case_out
             << " " << p.second << " " << instance_ptr->transition_time << endl;
    }
}

void STA::output_path(){
    double max_delay = DBL_MIN;
    double min_delay = DBL_MAX;
    int max_idx = 0;
    int min_idx = 0;

    for(auto& out_idx : output_wires){
        Wire *cur_wire = wires[out_idx];
        if(cur_wire->longest_delay > max_delay){
            max_delay = cur_wire->longest_delay;
            max_idx = out_idx;
        }
        if(cur_wire->longest_delay < min_delay){
            min_delay = cur_wire->longest_delay;
            min_idx = out_idx;
        }
    }
    double out_max = max_delay;
    double out_min = min_delay;
    vector<string> max_path;
    vector<string> min_path;
    max_path.push_back(wires[max_idx]->name);
    min_path.push_back(wires[min_idx]->name);

    while(max_delay != 0){
        // backward find the longest critical path
        int instance_idx = wires[max_idx]->input_cell;
        Instance *instance_ptr = instance[instance_idx];
        max_idx = instance_ptr->input_wire[0];
        max_delay = wires[max_idx]->longest_delay;
        if(instance_ptr->input_wire.size() > 1 && wires[instance_ptr->input_wire[1]]->longest_delay > max_delay){
            max_delay = wires[instance_ptr->input_wire[1]]->longest_delay;
            max_idx = instance_ptr->input_wire[1];
        }
        max_path.push_back(wires[max_idx]->name);
    }

    while(min_delay != 0){
        // backward find the shortest critical path
        int instance_idx = wires[min_idx]->input_cell;
        Instance *instance_ptr = instance[instance_idx];
        min_idx = instance_ptr->input_wire[0];
        min_delay = wires[min_idx]->longest_delay;
        if(instance_ptr->input_wire.size() > 1 && wires[instance_ptr->input_wire[1]]->longest_delay > min_delay){
            min_delay = wires[instance_ptr->input_wire[1]]->longest_delay;
            min_idx = instance_ptr->input_wire[1];
        }
        min_path.push_back(wires[min_idx]->name);
    }

    // output path
    string file_name = "109511272_" + case_name + "_path.txt";
    ofstream fout(file_name.c_str());

    fout << "Longest delay = " << setprecision(6) << fixed << out_max << ", the path is: ";
    for(int i=max_path.size()-1;i>0;i--)
        fout << max_path[i] << " -> ";
    fout << max_path[0] << endl;

    fout << "Shortest delay = " << setprecision(6) << fixed << out_min << ", the path is: ";
    for(int i=min_path.size()-1;i>0;i--)
        fout << min_path[i] << " -> ";
    fout << min_path[0] << endl;
}