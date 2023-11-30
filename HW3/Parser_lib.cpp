#include "parser.h"
Parser_lib::Parser_lib(char *in){
    file_name = in;
    build_cell();
    // show();
}

Parser_lib::~Parser_lib(){
    for(auto& m : cell_lib)
        delete m.second;
    for(auto& m : lu_table_template_lib)
        delete m.second;
}

void Parser_lib::build_cell(){
    ifstream fin(file_name);
    string line;

    // for(int i=0;i<6;i++)
    //    getline(fin, line); // parse unit attributes

    while(getline(fin, line)){
        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        stringstream ss(line);
        ss >> line;
        if(line == "lu_table_template"){
            ss >> line; // template name
            parse_template(fin, line);
        }
        else if(line == "cell"){
            ss >> line; // cell name
            parse_cell(fin, line);        
        }
    }
}

void Parser_lib::parse_template(ifstream& fin, string name){
    lu_table_template *t = new lu_table_template;
    t->name = name;
    string line;
    while(getline(fin, line)){
        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        stringstream ss(line);
        ss >> line;
        if(line == "}")
            break;
        if(line == "index_1"){
            ss >> line;
            replace(line.begin(), line.end(), '\"', ' ');
            replace(line.begin(), line.end(), ',', ' ');
            replace(line.begin(), line.end(), ';', ' ');
            stringstream zz(line);
            double n;
            while(zz >> n){
                t->x.push_back(n);
            }
            t->num_col = t->x.size();
        }
        else if(line == "index_2"){
            ss >> line;
            replace(line.begin(), line.end(), '\"', ' ');
            replace(line.begin(), line.end(), ',', ' ');
            replace(line.begin(), line.end(), ';', ' ');
            stringstream zz(line);
            double n;
            while(zz >> n){
                t->y.push_back(n);
            }
            t->num_row = t->y.size();
        }
    }
    lu_table_template_lib[name] = t;
}

void Parser_lib::parse_cell(ifstream& fin, string name){
    Cell *cur_cell = new Cell;
    cur_cell->name = name;
    string line;
    while(getline(fin, line)){
        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        stringstream ss(line);
        ss >> line;
        if(line == "}")
            break;
        ss >> line; // pin name
        parse_pin(fin, cur_cell, line);
    }
    cell_lib[name] = cur_cell;
}

void Parser_lib::parse_pin(ifstream& fin, Cell* cell, string& pin_name){
    Pin cur_pin;
    cur_pin.name = pin_name;
    string line;
    while(getline(fin, line)){
        replace(line.begin(), line.end(), ';', ' ');
        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        stringstream ss(line);
        ss >> line;
        if(line == "}")
            break;
        if(line == "direction"){
            ss >> line >> line;
            cur_pin.input = (line == "input");
        }
        else if(line == "capacitance"){
            double c;
            ss >> line >> c;
            cur_pin.capacitance = c; 
        }
        else if(line == "internal_power"){
            parse_power(fin, cell);
        }
        else if(line == "timing"){
            parse_timing(fin, cell);
        }
    }
    cell->pin_lib[pin_name] = cur_pin;
}

void Parser_lib::parse_power(ifstream &fin, Cell *cell){
    string line;
    while(getline(fin, line)){
        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        stringstream ss(line);
        string table_name;
        ss >> line;
        if(line == "rise_power" || line == "fall_power"){
            ss >> table_name;
            cell->table_template_lib[line] = lu_table_template_lib[table_name];
            cell->table_lib[line] = vector<vector<double>>(cell->table_template_lib[line]->num_row,vector<double>(cell->table_template_lib[line]->num_col));
            parse_value(fin, cell->table_lib[line]);
        }
        else if(line == "}")
            break;
    }
}

void Parser_lib::parse_timing(ifstream& fin, Cell *cell){
    string line;
    while(getline(fin, line)){
        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        stringstream ss(line);
        string table_name;
        ss >> line;
        if(line == "cell_rise" || line == "cell_fall" || line == "rise_transition" || line == "fall_transition"){
            ss >> table_name;
            cell->table_template_lib[line] = lu_table_template_lib[table_name];
            cell->table_lib[line] = vector<vector<double>>(cell->table_template_lib[line]->num_row,vector<double>(cell->table_template_lib[line]->num_col));
            parse_value(fin, cell->table_lib[line]);
        }
        else if(line == "}")
            break;
    }
}

void Parser_lib::parse_value(ifstream& fin, vector<vector<double>>& arr){
    string line;
    for(int i=0;i<arr.size();i++){
        getline(fin, line);
        replace(line.begin(), line.end(), '\"', ' ');
        replace(line.begin(), line.end(), ',', ' ');
        replace(line.begin(), line.end(), '(', ' ');
        stringstream ss(line);

        if(i == 0)
            ss >> line; // for "value"
        for(int j=0;j<arr[0].size();j++){
            ss >> arr[i][j];
        }
    }

    getline(fin, line); // for "}"
    
}

void Parser_lib::show(){
    cout<<"show cell:"<<endl;
    for(auto cell_ptr : cell_lib){
        cout<<cell_ptr.second->name<<endl;
        cout<<"\tPins : "<<endl;
        for(auto pin : cell_ptr.second->pin_lib){
            cout<<"\t\t name : "<<pin.second.name<<endl;
            cout<<"\t\t capacitance : "<<pin.second.capacitance<<endl;
        }

        cout << endl << endl;
        for(auto table : cell_ptr.second->table_lib){
            cout<<"\t"<<table.first<<endl;
            vector<vector<double>> &tmp = table.second;
            for(int i=0;i<tmp.size();i++){
                cout<<"\t\t";
                for(int j=0;j<tmp[0].size();j++)
                    cout<<tmp[i][j]<<" ";
                cout<<endl;
            }
        }
        cout<<endl;
    }

    cout << "show table template" << endl;
    for(auto t : lu_table_template_lib){
        cout <<"\t name : " << t.first << endl;
        cout <<"\t x : ";
        for(auto n : t.second->x)
            cout<<n<<" ";
        cout<<endl;
        cout <<"\t y : ";
        for(auto n : t.second->y)
            cout<<n<<" ";
        cout<<endl;
    }
}