#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <bitset>
#include <map>
#include <algorithm>
#include <climits>
using namespace std;


struct implicant{
    unordered_set<int> cover; // the term this implicant covered
    string s; // expression
    int num; // num of one in s
    bool simplified;
};

class QMC{
    private:
        int input_variable;
        unordered_set<int> on_set, dc_set, covered;
        vector<implicant> I;
        vector<implicant> PI;
        vector<implicant> MC;

        int min_literal;
        vector<implicant> best_MC;
    public:
        QMC();
        ~QMC();
        void load(char *);
        void solve();
        void output(char *);
        string convert(const string&); // convert binary to alphabetic

        int diffIdx(const string&, const string&);
        void getImplicant();
        void getPI();
        void getMC();

        void BF(int, unordered_set<int>, vector<implicant>&, int, vector<implicant>&);
};