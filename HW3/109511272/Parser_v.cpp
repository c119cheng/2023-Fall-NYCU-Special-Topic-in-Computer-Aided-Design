#include "parser.h"

Parser_v::Parser_v(char *in, Parser_lib& Pl) : lib(Pl){
    file_name = in;

    // preprocess(); // no comments in lab3 .v file
    built_Instance();
    // show_netlist();
}

Parser_v::~Parser_v(){
    for(auto& t : wire_lib)
        delete t.second;
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
    ifstream fin(file_name);
    string line;
    while(getline(fin, line)){
        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        replace(line.begin(), line.end(), ',', ' ');
        replace(line.begin(), line.end(), '.', ' ');
        replace(line.begin(), line.end(), ';', ' ');
        // cout<<line<<endl;
        stringstream zz(line);
        
        zz >> line;

        if(line == "module"){
            continue;
        }
        else if(line == "output"){
            while(zz >> line){
                Wire *tmp = new Wire;
                tmp->name = line;
                tmp->capacitance = 0.03;
                wire_lib[line] = tmp;
                output_wires.push_back(tmp);
            }
        }
        else if(line == "input"){
            while(zz >> line){
                Wire *tmp = new Wire;
                tmp->name = line;
                tmp->capacitance = 0.0;
                wire_lib[line] = tmp;
                input_wires.push_back(tmp);
            }
        }
        else if(line == "wire"){
            while(zz >> line){
                Wire *tmp = new Wire;
                tmp->name = line;
                tmp->capacitance = 0.0;
                wire_lib[line] = tmp;
            }
        }
        else if(line == "INVX1" || line == "NANDX1" || line == "NOR2X1"){
            string cell_name;
            string cell_type = line;
            string wire, port;

            // build instance info
            zz >> cell_name;
            Instance *tmp = new Instance;
            tmp->cell_ptr = lib.cell_lib[cell_type];

            for(int i=0;i<tmp->cell_ptr->pin_lib.size();i++){ // read pins
                zz >> port >> wire;
                if(tmp->cell_ptr->pin_lib[port].input){ // is input pin
                    tmp->input_wire.push_back(wire_lib[wire]);
                    wire_lib[wire]->capacitance += tmp->cell_ptr->pin_lib[port].capacitance;
                    wire_lib[wire]->output_instance.push_back(instance.size());
                }
                else{
                    tmp->output_wire = wire_lib[wire];
                    wire_lib[wire]->input_instance = instance.size();
                }
            }

            tmp->name = cell_name;
            tmp->update_time = 0;
            tmp->output_logic = 0;
            tmp->toggle_time_f = 0;
            tmp->toggle_time_r = 0;
            instance_lib[tmp->name] = instance.size();
            instance.push_back(tmp);
        }
    }
}

void Parser_v::show_netlist(){
    for(auto wire : wire_lib){
        cout << "wire : " << wire.second->name << endl;
        cout << "capacitance : " << wire.second->capacitance<<endl;
        if(find(input_wires.begin(), input_wires.end(), wire.second) == input_wires.end())
            cout << "\tfrom : " << instance[wire.second->input_instance]->name << " to : ";
        else
            cout << "\tfrom input to : ";
        for(const auto& cell : wire.second->output_instance)
            cout << instance[cell]->name << " ";
        cout << endl;
    }
}