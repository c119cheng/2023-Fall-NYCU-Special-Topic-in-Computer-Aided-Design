#include "QMC.h"
int main(int argc, char **argv){
    QMC sol;
    sol.load(argv[1]);
    sol.solve();
    sol.output(argv[2]);
    return 0;
}