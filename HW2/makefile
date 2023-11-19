
CC = g++
CFLAGS =  -std=c++11 -O3
MAIN_FILE = main.o
LINK_FILES = Parser_lib.o Parser_v.o STA.o
EXE_FILE = 109511272

CHECKCC = valgrind
CHECKFLAGS = --leak-check=full -s --show-leak-kinds=all --track-origins=yes 
#---------------------

all: $(LINK_FILES) $(MAIN_FILE)
	$(CC) -o $(EXE_FILE) $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

example:
	time ./$(EXE_FILE) example.v -l ./Test_Case/test_lib.lib
	diff -B -w ./109511272_example_delay.txt ./Test_Case/golden/golden_example_delay.txt
	diff -B -w ./109511272_example_load.txt ./Test_Case/golden/golden_example_load.txt
	diff -B -w ./109511272_example_path.txt ./Test_Case/golden/golden_example_path.txt

demo1:
	time ./$(EXE_FILE) c17.v -l ./Test_Case/test_lib.lib
	diff -B -w ./109511272_c17_delay.txt ./Test_Case/golden/golden_c17_delay.txt
	diff -B -w ./109511272_c17_load.txt ./Test_Case/golden/golden_c17_load.txt
	diff -B -w ./109511272_c17_path.txt ./Test_Case/golden/golden_c17_path_1.txt

demo2:
	time ./$(EXE_FILE) c432.v -l ./Test_Case/test_lib.lib
	diff -B -w ./109511272_c432_delay.txt ./Test_Case/golden/golden_c432_delay.txt
	diff -B -w ./109511272_c432_load.txt ./Test_Case/golden/golden_c432_load.txt
	diff -B -w ./109511272_c432_path.txt ./Test_Case/golden/golden_c432_path.txt

test00:
	time ./$(EXE_FILE) test_00.v -l ./Test_Case/test_lib.lib
	diff -B -w ./109511272_test_00_delay.txt ./109511190_test_00_delay.txt
	diff -B -w ./109511272_test_00_load.txt ./109511190_test_00_load.txt
	diff -B -w ./109511272_test_00_path.txt ./109511190_test_00_path.txt

test01:
	time ./$(EXE_FILE) test_01.v -l ./Test_Case/test_lib.lib
	diff -B -w ./109511272_test_01_delay.txt ./109511190_test_01_delay.txt > delay_log.txt
	diff -B -w ./109511272_test_01_load.txt ./109511190_test_01_load.txt > load_log.txt
	diff -B -w ./109511272_test_01_path.txt ./109511190_test_01_path.txt > path_log.txt

clean:
	rm $(MAIN_FILE) $(LINK_FILES) $(EXE_FILE) check.log *path.txt *load.txt *delay.txt 

check:
	$(CHECKCC) $(CHECKFLAGS) ./$(EXE_FILE) example.v -l ./Test_Case/test_lib.lib > check.log
