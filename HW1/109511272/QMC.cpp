#include "QMC.h"

QMC::QMC(){

}

QMC::~QMC(){

}

void QMC::load(char *file_name){
    ifstream fin(file_name);
    string line;
    stringstream ss;

    // get number of variables
    getline(fin, line);
    getline(fin, line);
    input_variable = stoi(line);

    // get on set
    getline(fin, line);
    getline(fin, line);
    ss.str(line);
    int num;
    while(ss >> num)
        on_set.insert(num);
    ss.clear();

    // get dc set
    getline(fin, line);
    getline(fin, line);
    ss.str(line);
    while(ss >> num)
        dc_set.insert(num);
}

void QMC::solve(){
    getImplicant();
    getPI();
    getMC();
}

int getNumOfOne(int n){
    int count = 0;
    while(n){
        count += n & 1;
        n >>= 1;
    }
    return count;
}

string getString(int n, int size){
    string t = bitset<8>(n).to_string();
    string out = "";
    for(int i=8-size;i<8;i++){
        out += t[i];
    }
    return out;
}

int getNumOfLiteral(string s){
    int count = 0;
    for(int i=0;i<s.length();i++)
        if(s[i] != '-')
            count++;
    return count;
}

void QMC::getImplicant(){
    // on_set
    for(const auto& n : on_set){
        implicant tmp;
        tmp.cover.insert(n);
        tmp.s = getString(n, input_variable);
        tmp.num = getNumOfOne(n);
        tmp.simplified = false;
        I.push_back(tmp);
    }

    // dc_set
    for(const auto& n : dc_set){
        implicant tmp;
        tmp.cover.insert(n);
        tmp.s = getString(n, input_variable);
        tmp.num = getNumOfOne(n);
        tmp.simplified = false;
        I.push_back(tmp);
    }
}

int QMC::diffIdx(const string& a, const string& b){
    int idx = -1;
    int diff = 0;
    for(int i=0;i<input_variable;i++){
        if(a[i] != b[i]){
            diff++;
            idx = i;
            if(diff == 2)
                return -1;
        }
    }
    return idx;
}

void QMC::getPI(){
    // divide implicants by number of 1
    map<int, vector<implicant>> m;
    for(auto const& i : I){
        m[i.num].push_back(i);
    }

    // get prime implicant
    unordered_set<string> existed;
    while(!m.empty()){
        map<int, vector<implicant>> tmp;
        for(int i=0;i<=input_variable;i++){
            for(auto& IA : m[i]){
                for(auto& IB : m[i+1]){
                    int idx = diffIdx(IA.s, IB.s);
                    if(idx != -1){ // can be simplify
                        implicant a;
                        a.cover.insert(IA.cover.begin(), IA.cover.end());
                        a.cover.insert(IB.cover.begin(), IB.cover.end());

                        a.s = IA.s;
                        a.s[idx] = '-';
                        a.num = min(IA.num, IB.num);

                        a.simplified = false;
                        if(!existed.count(a.s))
                            tmp[a.num].push_back(a);
                        existed.insert(a.s);
                        IA.simplified = true;
                        IB.simplified = true;
                    }
                }
                if(!IA.simplified)
                    PI.push_back(IA);
            }
        }

        m = tmp;
    }
}

string QMC::convert(const string& in){
    string out = "";
    for(int i=0;i<input_variable;i++){
        if(in[i] == '-')
            continue;
        out += 'A' + i;
        if(in[i] == '0')
            out += '\'';
    }
    return out;
}

void QMC::output(char *file_name){
    PI = I;
    vector<string> pi_s(PI.size()); // prime implicant string
    for(int i=0;i<PI.size();i++)
        pi_s[i] = PI[i].s;

    vector<string> mc_s(MC.size()); // minimum covering string
    for(int i=0;i<MC.size();i++)
        mc_s[i] = MC[i].s;

    // sort in alphabetic order
    sort(pi_s.begin(), pi_s.end(), greater<string>());
    sort(mc_s.begin(), mc_s.end(), greater<string>());


    // output to file
    ofstream fout(file_name);
    fout<<".p "<<PI.size()<<endl;
    int count = 0;
    for(const auto& a : pi_s){
        fout<<convert(a)<<endl;
        if(++count == 15)
            break;
    }
    
    fout<<endl<<".mc "<<MC.size()<<endl;
    for(const auto& m : mc_s){
        fout<<convert(m)<<endl;
    }
    fout<<"literal="<<min_literal;
}

void QMC::getMC(){
    // find the term only cover by one prime implicant
    map<int, int> cover_size;
    for(const auto& tmp : PI){
        for(const auto& c : tmp.cover)
            if(on_set.count(c))
                cover_size[c]++;
    }
    // save prime implicant into I
    I = PI;

    
    bool stop = false;
    while(!stop){
        stop = true;
        for(const auto& cs : cover_size){
            if(cs.second == 1){
                // find the prime implicant that cover this term
                for(int i=0;i<PI.size();i++){
                    if(PI[i].cover.count(cs.first)){
                        MC.push_back(PI[i]);
                        for(const auto& n : PI[i].cover)
                            cover_size.erase(n);
                        PI.erase(PI.begin()+i);
                        break;
                    }
                }
            }
        }
    }
    
    // brute force to find remaining min covering
    unordered_set<int> uncovered;
    for(const auto& n : cover_size)
        uncovered.insert(n.first);

    vector<implicant> remain_implicant;
    for(auto& pi : PI){
        unordered_set<int> new_cover;
        bool insert = false;
        for(const auto& n : uncovered)
            if(pi.cover.count(n)){
                new_cover.insert(n);
                insert = true;
            }
        if(insert){
            pi.cover.clear();
            pi.cover.insert(new_cover.begin(), new_cover.end());
            remain_implicant.push_back(pi);
        }
    }

    // build covered set
    for(const auto& n : on_set)
        if(!uncovered.count(n))
            covered.insert(n);

    // recursive find min literal solution
    min_literal = INT_MAX;

    vector<implicant> tmp;
    BF(0, covered, tmp, 0, remain_implicant);

    // organize all mc
    for(const auto& i : MC)
        min_literal += getNumOfLiteral(i.s);
    for(const auto& i : best_MC)
        MC.push_back(i);
}

void QMC::BF(int cur_literal, unordered_set<int> cur_covered, vector<implicant>& cur_implicant,
    int idx, vector<implicant>& remain){
        if(cur_literal < min_literal && cur_covered.size() == on_set.size()){
            min_literal = cur_literal;
            best_MC = cur_implicant;
            return ;
        }
        if(cur_literal > min_literal || idx == remain.size())
            return ;

        // not select idx implicant
        BF(cur_literal, cur_covered, cur_implicant, idx+1, remain);

        // select idx implicant
        cur_implicant.push_back(remain[idx]);
        cur_covered.insert(remain[idx].cover.begin(), remain[idx].cover.end());
        cur_literal += getNumOfLiteral(remain[idx].s);
        BF(cur_literal, cur_covered, cur_implicant, idx+1, remain);
        cur_implicant.pop_back();
    }