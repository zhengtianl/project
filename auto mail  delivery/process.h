#ifndef PROCESS_H
#define PROCESS_H 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "heap.h"

typedef struct Process {
    int64_t id;
    int64_t time_arrived;
    int64_t time_executed;
    bool parallel;

    int64_t time_need;
    int64_t time_end;

    int sub_id;
    int num_of_sub_process;
    int num_of_finished_proces;
    struct Process* main_process;
} Process;

typedef Heap ProcessQueue;

Process* createProcessFromStr(char* line);
ProcessQueue newProcessQueue();
int splitProcess(Process* p, Process** results, int n);

#endif /* ifndef PROCESS_H */
