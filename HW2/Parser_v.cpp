#include "parser.h"

Parser_v::Parser_v(char *in, Parser_lib& Pl) : lib(Pl){
    file_name = in;

    preprocess();
    built_Instance();
    // show_netlist();
}

Parser_v::~Parser_v(){
    for(auto& t : wires)
        delete t;
    for(auto& t : instance)
        delete t;
}

void Parser_v::preprocess(){
    // remove all extra space and comments
    ifstream fin(file_name);

    char c, prev;
    string line;
    prev = ' ';
    bool m_cmt = false; // comment block flag
    while(getline(fin, line)){
        for(int i=0;i<line.length();i++){
            if(m_cmt && line[i] == '*' && line[i+1] == '/'){ // end of comment block
                m_cmt = false;
                i++;
            }
            else if(m_cmt) // commented character
                continue;
            else if(line[i] == '/' && line[i+1] == '/') // line comment
                break;
            else if(line[i] == '/' && line[i+1] == '*'){ // start of block comment
                m_cmt = true;
                i++;
            }
            else{
                ss << line[i];
                // if(line[i] == ';')
                //     ss << endl;
            }
        }
    }
}

void Parser_v::built_Instance(){
    string line;
    while(getline(ss, line, ';')){
        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        replace(line.begin(), line.end(), ',', ' ');
        replace(line.begin(), line.end(), '.', ' ');
        // cout<<line<<endl;
        stringstream zz(line);
        
        zz >> line;
        if(line == "module"){
            continue;
        }
        else if(line == "output"){
            while(zz >> line){
                wire_lib[line] = wires.size();
                output_wires.push_back(wire_lib[line]);
                Wire *tmp = new Wire;
                tmp->name = line;
                tmp->capacitance = 0.03;
                wires.push_back(tmp);
            }
        }
        else if(line == "input"){
            while(zz >> line){
                wire_lib[line] = wires.size();
                input_wires.push_back(wire_lib[line]);
                Wire *tmp = new Wire;
                tmp->name = line;
                tmp->capacitance = 0.0;
                wires.push_back(tmp);
            }
        }
        else if(line == "wire"){
            while(zz >> line){
                wire_lib[line] = wires.size();
                Wire *tmp = new Wire;
                tmp->name = line;
                tmp->capacitance = 0.0;
                wires.push_back(tmp);
            }
        }
        else if(line == "INVX1" || line == "NANDX1" || line == "NOR2X1"){
            string cell_name;
            string cell_type = line;
            string wire, port;

            // build instance info
            zz >> cell_name;
            // cout<<cell_name<<endl;
            Instance *tmp = new Instance;
            tmp->cell_ptr = lib.cell_lib[cell_type];
            for(int i=0;i<tmp->cell_ptr->pin_lib.size();i++){ // read pins
                zz >> port >> wire;
                int wire_idx = wire_lib[wire];
                if(tmp->cell_ptr->pin_lib[port].input){ // is input pin
                    tmp->input_wire.push_back(wire_idx);
                    wires[wire_idx]->capacitance += tmp->cell_ptr->pin_lib[port].capacitance;
                    wires[wire_idx]->output_cell.push_back(instance.size());
                }
                else{
                    tmp->output_wire = wire_idx;
                    wires[wire_idx]->input_cell = instance.size();
                }
            }

            tmp->name = cell_name;
            tmp->update_time = 0;
            tmp->input_transition = 0;
            tmp->longest_delay = DBL_MIN;
            instance_lib[tmp->name] = instance.size();
            instance.push_back(tmp);
            
        }
    }
}

void Parser_v::show_netlist(){
    for(int i=0;i<wires.size();i++){
        cout << "wire : " << wires[i]->name << endl;
        cout << "capacitance : " << wires[i]->capacitance<<endl;
        if(find(input_wires.begin(), input_wires.end(), i) == input_wires.end())
            cout << "\tfrom : " << instance[wires[i]->input_cell]->name << " to : ";
        else
            cout << "\tfrom input to : ";
        for(const auto& cell : wires[i]->output_cell)
            cout << instance[cell]->name << " ";
        cout << endl;
    }
}