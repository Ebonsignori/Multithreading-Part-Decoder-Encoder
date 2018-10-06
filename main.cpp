#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#include <iostream>
#include <fstream>

using namespace std;

void *part1(void *section1);
void *part2(void *section1);

int main() {
    pthread_t thread_1;
    pthread_t thread_2;

    char * thread1Pointer;
    char * thread2Pointer;

    int result;
    char* message = const_cast<char *>("hello");

    result = pthread_create(&thread_1, NULL, part1, (void*)message);
    pthread_create(&thread_2, NULL, part2, (void*)message);

    // 0 if thread has been created successfully, otherwise 1 if failure
    if (result == 0) {
        // TODO
        cout << "Thread Created!" << endl;
    }

    // Join threads back together
    result = pthread_join(thread_1, (void **)&thread1Pointer);
    pthread_join(thread_2, (void **)&thread2Pointer);
    cout << thread1Pointer << endl;
    cout << thread2Pointer << endl;


    return 0;
}

void* part1(void* section1) {
    pthread_exit((void *)"part1");
}

void* part2(void* section1) {
    pthread_exit((void *)"part2");
}
