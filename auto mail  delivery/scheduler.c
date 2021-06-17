#include "scheduler.h"

#include <string.h>
#include <math.h>

void updateStatistics(Scheduler* schedulers, Process* pro) {
    schedulers->stat.num_process += 1;
    int64_t tournament = pro->time_end - pro->time_arrived;
    schedulers->stat.tournament += tournament;
    double overhead = 1.0 * tournament / pro->time_executed;
    schedulers->stat.avg_tournament += overhead;
    if (schedulers->stat.max_tournament < overhead) {
        schedulers->stat.max_tournament = overhead;
    }
}

void displayStat(Scheduler* schedulers) {
    double res = 1.0 * schedulers->stat.tournament / schedulers->stat.num_process;
    printf("Turnaround time %.0lf\n", ceil(res));
    printf("Time overhead %g %g\n",
            round(schedulers->stat.max_tournament * 100) / 100.0,
            round(schedulers->stat.avg_tournament / schedulers->stat.num_process * 100) / 100.0);
    printf("Makespan %ld\n", schedulers->curr_time);
}

void updateAllProcess(Scheduler* schedulers, int64_t next_time) {
    Process** finished = malloc(schedulers->size * sizeof(Process*));
    int n = 0;

    int64_t diff = next_time - schedulers->curr_time;

    /* printf("diff = %ld\n", diff); */
    for (int i = 0; i < schedulers->size; ++i) {
        SingleScheduler* sch = &schedulers->sches[i];
        Process* curr = sch->current;
        if (curr) {
            sch->total_time -= diff;
            curr->time_need -= diff;
            /* printf("time_need = %ld\n", curr->time_need); */
            if (curr->time_need == 0) {
                /* printf("Finished %ld\n", curr->id); */
                curr->time_end = next_time;
                if (curr->main_process) {
                    curr->main_process->num_of_finished_proces += 1;
                    if (curr->main_process->num_of_finished_proces ==
                        curr->main_process->num_of_sub_process) {
                        curr->main_process->time_end = next_time;
                        schedulers->num_proc_left -= 1;
                        finished[n++] = curr->main_process;
                    }
                    free(curr);
                } else {
                    schedulers->num_proc_left -= 1;
                    finished[n++] = curr;
                }
                sch->current = NULL;
            }
        }
    }
    schedulers->curr_time = next_time;

    for (int i = 0; i < n; ++i) {
        printf("%ld,FINISHED,pid=%ld,proc_remaining=%d\n",
                next_time,
                finished[i]->id,
                schedulers->num_proc_left);
        updateStatistics(schedulers, finished[i]);
        free(finished[i]);
    }
    free(finished);
}

static int cmpSchedulerTotalTime(const void* a, const void* b) {
    const SingleScheduler* l = a;
    const SingleScheduler* r = b;
    int64_t ret = l->total_time - r->total_time;
    if (ret == 0) {
        ret = l->id - r->id;
    }

    if (ret < 0) {
        return 1;
    } else if (ret == 0) {
        return 0;
    }
    return -1;
}

void assginProcessToScheduler(Scheduler* schedulers, Process* pro) {
    Heap sch_que = newHeap(cmpSchedulerTotalTime, schedulers->size);
    for (int i = 0; i < schedulers->size; ++i) {
        pushHeap(sch_que, &schedulers->sches[i]);
    }

    Process** results = malloc(schedulers->size * sizeof(Process*));
    int k = splitProcess(pro, results, schedulers->size);
    for (int i = 0; i < k; ++i) {
        /* printf("assgin to cpu %d\n", i); */
        SingleScheduler* sch = topHeap(sch_que);
        popHeap(sch_que);
        pushHeap(sch->queue, results[i]);
        sch->total_time += results[i]->time_need;
    }

    schedulers->num_proc_left += 1;
    freeHeap(sch_que);
    free(results);
}

Scheduler* newScheduler(int n) {
    Scheduler* sch = malloc(sizeof (Scheduler));
    sch->num_proc_left = 0;
    sch->size = n;
    sch->curr_time = 0;
    sch->sches = malloc(sch->size * sizeof(SingleScheduler));
    for (int i = 0; i < sch->size; ++i) {
        SingleScheduler* s = &sch->sches[i];
        s->id = i;
        s->current = NULL;
        s->queue = newProcessQueue();
        s->total_time = 0;
    }

    memset(&sch->stat, 0, sizeof (Statictis));
    return sch;
}

int64_t findMinTimeInScheduler(Scheduler* schedulers, int64_t arrival_time) {
    int64_t ret = -1;
    for (int i = 0; i < schedulers->size; ++i) {
        SingleScheduler* sch = &schedulers->sches[i];
        Process* curr = sch->current;
        if (!curr) {
            continue;
        }
        int64_t time_end = curr->time_need + schedulers->curr_time;
        if (time_end < arrival_time) {
            if (ret == -1 || ret > time_end) {
                ret = time_end;
            }
        }
    }
    return ret;
}

void resumeAllProcess(Scheduler* schedulers) {
    for (int i = 0; i < schedulers->size; ++i) {
        SingleScheduler* sch = &schedulers->sches[i];
        if (isEmptyHeap(sch->queue)) {
            continue;
        }

        Process* top = topHeap(sch->queue);
        if (!sch->current ||
            (sch->current->time_need > top->time_need ||
             (sch->current->time_need == top->time_need &&
              sch->current->id > top->id))) {
            popHeap(sch->queue);
            if (sch->current) {
                pushHeap(sch->queue, sch->current);
            }
            sch->current = top;
            if (sch->current->main_process) {
                printf("%ld,RUNNING,pid=%ld.%d,remaining_time=%ld,cpu=%d\n",
                        schedulers->curr_time,
                        sch->current->main_process->id,
                        sch->current->sub_id,
                        sch->current->time_need,
                        i);
            } else {
                printf("%ld,RUNNING,pid=%ld,remaining_time=%ld,cpu=%d\n",
                        schedulers->curr_time,
                        sch->current->id,
                        sch->current->time_need,
                        i);
            }
        }
    }
}

void freeScheduler(Scheduler* scheduler) {
    for (int i = 0; i < scheduler->size; ++i) {
        freeHeap(scheduler->sches[i].queue);
    }
    free(scheduler->sches);
    free(scheduler);
}
