#include "process.h"
#include <stdlib.h>
#include <string.h>

Process* createProcessFromStr(char* line) {
    Process* p = malloc(sizeof (Process));
    memset(p, 0, sizeof (Process));
    char parallel[2];
    sscanf(line, "%ld%ld%ld%s", &p->time_arrived, &p->id, &p->time_executed, parallel);
    p->time_need = p->time_executed;
    if (parallel[0] == 'p') {
        p->parallel = true;
    }
    return p;
}

static int cmpProcess(const void* a, const void* b) {
    const Process* l = a;
    const Process* r = b;
    int64_t ret = l->time_need - r->time_need;
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

ProcessQueue newProcessQueue() {
    return newHeap(cmpProcess, 2);
}

int splitProcess(Process* p, Process** results, int n) {
    int k = 1;
    if (n == 1 || !p->parallel) {
        results[0] = p;
    } else {
        k = p->time_executed > n ? n : p->time_arrived;
        for (int i = 0; i < k; ++i) {
            Process* sub = malloc(sizeof (Process));
            memset(sub, 0, sizeof (Process));
            sub->time_arrived = p->time_arrived;
            sub->main_process = p;
            sub->time_need = (p->time_need + k - 1) / k + 1;
            sub->id = p->id;
            sub->sub_id = i;
            results[i] = sub;
        }
        p->num_of_sub_process = k;
    }
    return k;
}
