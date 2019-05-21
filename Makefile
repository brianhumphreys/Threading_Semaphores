all: sample_threading

# sample_grader:autograder_main_3.c thread_lib 
# 	gcc -g autograder_main_3.c threads.o -o sample_grader

sample_threading: autotester_main_3.c threads.c linkedList.c semaphore.c
	g++  -o sample_grader autotester_main_3.c threads.c linkedList.c semaphore.c

# thread_lib:fiber_manager.c
# 	gcc -g -c fiber_manager.c -o threads.o

# all: sample_grader

# sample_grader:autograder_main.c thread_lib 
# 	g++ autograder_main.c threads.o -o sample_grader

# thread_lib:threads.c linkedList.o
# 	g++ -c threads.c -o threads.o linkedList.o

# linkedList.o: linkedList.c 
# 	gcc -c linkedList.c linkedList.o

# thread_run2: autograder_main_2.o threads.o linkedList.o
# 	g++ autograder_main_2.o threads.o linkedList.o -o thread_run2
#***************************************************************************************************/

# thread_lib: fibers.o linkedList.o semaphore.o
# 	ld -r fibers.o linkedList.o semaphore.o -o threads.o

#***************************************************************************************************/
# thread_run3: extratests.o threads.o linkedList.o semaphore.o
# 	g++ extratests.o threads.o linkedList.o semaphore.o -o thread_run3

# # autograder_main_3.o: autograder_main_3.c
# # 	g++ -c -o autograder_main_3.o autograder_main_3.c

# # autograder_main_2.o: autograder_main_2.c
# # 	g++ -c -o autograder_main_2.o autograder_main_2.c

# extratests.o: extratests.cpp
# 	g++ -c -o extratests.o extratests.cpp

# autograder_main_mine.o: autograder_main_mine.c
# 	g++ -c -o autograder_main_mine.o autograder_main_mine.c

# threads.o: threads.c
# 	g++ -g -Wall -c threads.c

# linkedList.o: linkedList.c
# 	g++ -g -Wall -c linkedList.c

# semaphore.o: semaphore.c
# 	g++ -g -Wall -c semaphore.c

#***************************************************************************************************/

# fiber_struct.o: fiber_struct.h
# 	g++ -c fiber_struct.h

clean: 
	rm *.o sample_grader
