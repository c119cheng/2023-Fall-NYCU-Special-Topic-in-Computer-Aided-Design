#include "parser.h"
#include "Power_Analyzer.h"
int main(int argc, char **argv){
    Parser_lib Pl(argv[3]); // parser .lib file
    Parser_v P(argv[1], Pl); // parser .v file
    Power_Analyzer S(P, argv[1]);
    S.solve(argv[2]);
    return 0;
}