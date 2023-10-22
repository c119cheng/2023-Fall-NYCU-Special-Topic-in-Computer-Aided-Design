
CC = g++
CFLAGS =  -std=c++11 -O3
MAIN_CPP = 109511272.cpp
LINK_CPPS = QMC.cpp
EXE_FILE = 109511272.o

CHECKCC = valgrind
CHECKFLAGS = --leak-check=full -s --show-leak-kinds=all --track-origins=yes 
#---------------------

all:
	$(CC) $(CFLAGS) -o $(EXE_FILE) $(MAIN_CPP) $(LINK_CPPS)

sample:
	time ./$(EXE_FILE) ./testcase_2/input0.txt output.txt

demo1:
	time ./$(EXE_FILE) ./testcase/input1.txt output1.txt

demo2:
	time ./$(EXE_FILE) ./testcase/input2.txt output2.txt

demo3:
	time ./$(EXE_FILE) ./testcase_2/input3.txt output3.txt
demo4:
	time ./$(EXE_FILE) ./testcase_2/input4.txt output4.txt
demo5:
	time ./$(EXE_FILE) ./testcase_2/input5.txt output5.txt
demo6:
	time ./$(EXE_FILE) ./testcase_2/input6.txt output6.txt


clean:
	rm $(MAIN_FILE) $(LINK_FILES) $(EXE_FILE) check.log output.txt

check:
	$(CHECKCC) $(CHECKFLAGS) ./$(EXE_FILE) ./testcase/input_1.dat output.txt > check.log
