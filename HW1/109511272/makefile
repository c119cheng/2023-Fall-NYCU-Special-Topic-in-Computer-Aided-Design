
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

demo1g:
	time ./$(EXE_FILE) ./HW1_golden/testcase/input1.txt output1.txt
	./verify -t ./HW1_golden/testcase/input1.txt -g ./HW1_golden/golden/output1.txt -s ./output1.txt

demo2g:
	time ./$(EXE_FILE) ./HW1_golden/testcase/input2.txt output2.txt
	./verify -t ./HW1_golden/testcase/input2.txt -g ./HW1_golden/golden/output2.txt -s ./output2.txt

demo3g:
	time ./$(EXE_FILE) ./HW1_golden/testcase/input3.txt output3.txt
	./verify -t ./HW1_golden/testcase/input3.txt -g ./HW1_golden/golden/output3.txt -s ./output3.txt

demo4g:
	time ./$(EXE_FILE) ./HW1_golden/testcase/input4.txt output4.txt
	./verify -t ./HW1_golden/testcase/input4.txt -g ./HW1_golden/golden/output4.txt -s ./output4.txt

demo5g:
	time ./$(EXE_FILE) ./HW1_golden/testcase/input5.txt output5.txt
	./verify -t ./HW1_golden/testcase/input5.txt -g ./HW1_golden/golden/output5.txt -s ./output5.txt
demo6g:
	time ./$(EXE_FILE) ./HW1_golden/testcase/input6.txt output6.txt
	./verify -t ./HW1_golden/testcase/input6.txt -g ./HW1_golden/golden/output6.txt -s ./output6.txt


clean:
	rm $(MAIN_FILE) $(LINK_FILES) $(EXE_FILE) check.log output.txt

check:
	$(CHECKCC) $(CHECKFLAGS) ./$(EXE_FILE) ./testcase/input_1.dat output.txt > check.log
