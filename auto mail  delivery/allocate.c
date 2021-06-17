#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "process.h"
#include "scheduler.h"
#include "heap.h"

static int cmpInputProc(const void* a, const void* b) {
    const Process* l = a;
    const Process* r = b;
    int64_t ret = l->time_arrived - r->time_arrived;
    if (ret == 0) {
        ret = l->time_executed - r->time_executed;
        if (ret == 0) {
            ret = l->id - r->id;
        }
    }
    
    if (ret < 0) {
        return 1;
    } else if (ret == 0) {
        return 0;
    }
    return -1;
}

int main(int argc, char *argv[]) {
    char* file = NULL;
    int n = 0;
    int ch = 0;
    while ((ch = getopt(argc, argv, "f:p:c")) != -1) {
        switch (ch) {
            case 'f':
                file = optarg;
                break;
            case 'p':
                n = atoi(optarg);
                break;
            case 'c':
                break;
            default:
                break;
        }
    }
    FILE* fp = fopen(file, "r");
    if (!fp) {
        return 1;
    }

    Scheduler* schedulers = newScheduler(n);

    Heap inputProcesss = newHeap(cmpInputProc, 1024);
    char line[1024];
    while (fgets(line, 1024, fp)) {
        pushHeap(inputProcesss, createProcessFromStr(line));
    }

    Process* top = NULL;
    while (!isEmptyHeap(inputProcesss)) {
        top = topHeap(inputProcesss);
        popHeap(inputProcesss);
        while (1) {
            int next_time = findMinTimeInScheduler(schedulers, top->time_arrived);
            /* printf("next_time = %d\n", next_time); */
            if (next_time == -1 || next_time == top->time_arrived) {
                break;
            }
            updateAllProcess(schedulers, next_time);
            resumeAllProcess(schedulers);
        }

        assginProcessToScheduler(schedulers, top);
        updateAllProcess(schedulers, top->time_arrived);
        resumeAllProcess(schedulers);
    }

    while (schedulers->num_proc_left > 0) {
        int next_time = findMinTimeInScheduler(schedulers, 1l << 32);
        /* printf("next_time = %d\n", next_time); */
        if (next_time == -1) {
            break;
        }
        updateAllProcess(schedulers, next_time);
        resumeAllProcess(schedulers);
    }
    displayStat(schedulers);
    freeScheduler(schedulers);
    freeHeap(inputProcesss);
    fclose(fp);
}
