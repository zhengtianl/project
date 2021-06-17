#ifndef SCHEDULER_H
#define SCHEDULER_H 

#include "heap.h"
#include "process.h"

typedef struct SingleScheduler {
    Process* current;
    ProcessQueue queue;
    int id;
    int64_t total_time;
} SingleScheduler;

typedef struct Statitis {
    int64_t num_process;
    int64_t tournament;
    double max_tournament;
    double avg_tournament;
} Statictis;

typedef struct Scheduler {
    SingleScheduler* sches;
    int64_t curr_time;
    int size;
    int num_proc_left;
    Statictis stat;
} Scheduler;


Scheduler* newScheduler(int n);
void freeScheduler(Scheduler* scheduler);
void updateAllProcess(Scheduler* schedulers, int64_t next_time);
void resumeAllProcess(Scheduler* schedulers);
void assginProcessToScheduler(Scheduler* schedulers, Process* pro);
int64_t findMinTimeInScheduler(Scheduler* schedulers, int64_t arrival_time);

void displayStat(Scheduler* schedulers);

#endif /* ifndef SCHEDULER_H */
