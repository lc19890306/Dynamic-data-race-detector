=================================== README ====================================
########## BASIC INFO ##########
Simple data race detector
Written by Chang Liu
Course project for CS/ECE 5510 Multiprocessor Programming
This simple data race detector is written in C language, including a backend and three test cases.
my_thread.h is the head file.
my_thread.c is the static library.
test_case_01.c is used to test whether data race will not be reported in read-shared case and reader-writer case.
test_case_02.c is used to test whether data race can be reported in the most common double-writer case.
test_case_03.c is used to test whether data race can be reported regularly even if no lock protect all the shared variables.

########## EXECUTION ###########
OS: Linux (x86-32bit)
Compiler: gcc
Instruction: gcc -pthread test_case_0X.c my_thread.c
Instruction: ./a.out

########## CONTROLLED TEST ############
I used ThreadSnitizer and three test cases to conduct some controlled tests in order to justify the performance of my data race detector.
test_case_0X.o.c is the original version of each test case for test_case_0X.c.

########## EXECUTION ############
Tool: ThreadSanitizer (Copyright owned by Google)
OS: Linux (x86-32bit)
Compiler: gcc
Instruction: gcc -pthread test_case_0X.o.c
Instruction: ./tsan-r3700-x86-linux-self-contained.sh ./a.out
