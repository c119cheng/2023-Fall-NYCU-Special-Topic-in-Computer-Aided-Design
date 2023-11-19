#include "parser.h"
#include "STA.h"
int main(int argc, char **argv){
    Parser_lib Pl(argv[3]); // parser .lib file
    Parser_v P(argv[1], Pl); // parser .v file
    STA S(P, argv[1]);
    S.output_loading();
    S.output_delay();
    S.output_path();
    return 0;
}