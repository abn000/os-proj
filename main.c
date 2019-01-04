#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int arrival;
    int burst;
    int remaining;
    int stay;
    int waiting;
    int response;
    char name[20];
} process_info;

int compare (const void* a, const void* b) {
    process_info* pa = *((process_info**) a);
    process_info* pb = *((process_info**) b);
    return pa->burst < pb->burst ? -1 : (pa->burst == pb->burst ? 0 : 1);
}

void round_robin(process_info* processes, int n, int q) {
    int t = 1;
    int rem = n;
    int j;
    int min_arr = -1;
    int total = 0;
    FILE *output = fopen("rr.txt", "w");
    for (j = 0; j < n; j++) {
        processes[j].remaining = processes[j].burst;
        total += processes[j].burst;
        if (min_arr == -1 || min_arr > processes[j].arrival) {
            min_arr = processes[j].arrival;
        }
    }
    total += min_arr;

    for (j = 1; j < total; j++) {
        fprintf(output, "%-2d|", j);
    }
    fprintf(output, "\n");
    int f = 0;
    int l = 0;
    int c = 0;
    process_info* queue[50];
    process_info* cur = NULL;

    int count = 0;
    while (rem > 0) {
        for (j = 0; j < n; j++) {
            if (processes[j].arrival == t) {
                queue[l++] = &processes[j];
            }
        }
        if (cur == NULL) {
            if (f < l) {
                cur = queue[f++];
            }
        }
        if (cur != NULL) {
            fprintf(output, "%s |", cur->name);
            if (cur->remaining == cur->burst) {
                cur->response = t - cur->arrival;
            }
            cur->remaining--;
            count++;
            if (cur->remaining == 0) {
                cur->stay = t - cur->arrival + 1;
                cur->waiting = cur->stay - cur->burst;
                cur = NULL;
                count = 0;
                rem--;
            } else if (count == q) {
                count = 0;
                queue[l++] = cur;
                cur = NULL;
            }
        } else {
            fprintf(output, "  |");
            c++;
        } 
        t++;
    }
    fprintf(output, "\n");

    char st[80];
    memset(st, ' ', 79);
    for (j = 0; j < n; j++) {
        char src[22];
        sprintf(src, "^%s ", processes[j].name);
        memcpy(st + (processes[j].arrival - 1) * 3, src, 3);
    }
    fprintf(output, "%s\n", st);

    fprintf(output, "cpu utilization: %.2f%%\n", 100.0 * (total - c) / total);

    for (j = 0; j < n; j++) {
        fprintf(output, "\nprocess %s:\n", processes[j].name);
        fprintf(output, "\tarrival time: %d\n", processes[j].arrival);
        fprintf(output, "\tburst time: %d\n", processes[j].burst);
        fprintf(output, "\tstay time: %d\n", processes[j].stay);
        fprintf(output, "\tresponse time: %d\n", processes[j].response);
        fprintf(output, "\twaiting time: %d\n\n", processes[j].waiting);
    }
    fclose(output);
}

void shortest_job_first(process_info* processes, int n) {
    int t = 1;
    int rem = n;
    int j;
    int min_arr = -1;
    int total = 0;
    FILE *output = fopen("sjf.txt", "w");
    for (j = 0; j < n; j++) {
        processes[j].remaining = processes[j].burst;
        total += processes[j].burst;
        if (min_arr == -1 || min_arr > processes[j].arrival) {
            min_arr = processes[j].arrival;
        }
    }

    total += min_arr;

    for (j = 1; j < total; j++) {
        fprintf(output, "%-2d|", j);
    }
    fprintf(output, "\n");
    int f = 0;
    int l = 0;
    int c = 0;
    process_info* queue[50];
    process_info* cur = NULL;

    while (rem > 0) {
        for (j = 0; j < n; j++) {
            if (processes[j].arrival == t) {
                queue[l++] = &processes[j];
            }
        }
        qsort(&queue[f], l - f, sizeof(process_info*), compare);
        if (cur == NULL) {
            if (f < l) {
                cur = queue[f++];
            }
        }
        if (cur != NULL) {
            fprintf(output, "%s |", cur->name);
            if (cur->remaining == cur->burst) {
                cur->response = t - cur->arrival;
            }
            cur->remaining--;
            if (cur->remaining == 0) {
                cur->stay = t - cur->arrival + 1;
                cur->waiting = cur->stay - cur->burst;
                cur = NULL;
                rem--;
            } 
        } else {
            fprintf(output, "  |");
            c++;
        } 
        t++;
    }
    fprintf(output, "\n");

    char st[80];
    memset(st, ' ', 79);
    for (j = 0; j < n; j++) {
        char src[22];
        sprintf(src, "^%s ", processes[j].name);
        memcpy(st + (processes[j].arrival - 1) * 3, src, 3);
    }
    fprintf(output, "%s\n", st);

    fprintf(output, "cpu utilization: %.2f%%\n", 100.0 * (total - c) / total);

    for (j = 0; j < n; j++) {
        fprintf(output, "\nprocess %s:\n", processes[j].name);
        fprintf(output, "\tarrival time: %d\n", processes[j].arrival);
        fprintf(output, "\tburst time: %d\n", processes[j].burst);
        fprintf(output, "\tstay time: %d\n", processes[j].stay);
        fprintf(output, "\tresponse time: %d\n", processes[j].response);
        fprintf(output, "\twaiting time: %d\n\n", processes[j].waiting);
    }
    fclose(output);

}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage %s <FILE>\n", argv[0]);
        return 1;
    }
    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("Can't open file %s\n", argv[1]);
        return 1;
    }
    int n; //number of processes
    if (fscanf(input, "%d",  &n) != 1) {
        printf("Can't read number of processes\n");
        return 1;
    }

    process_info* processes = malloc(n * sizeof(process_info));

    int i;
    for (i = 0; i < n; i++) {
        if(fscanf(input, "%s %d %d", processes[i].name, &processes[i].arrival, &processes[i].burst) != 3) {
            printf("Can't read process %d info\n", i);
            return 1;
        }
    }

    int q; //time quantum for RR
    if (fscanf(input, "%d", &q) != 1) {
        printf("Can't read quantum\n");
        return 1;
    }
    round_robin(processes, n, q);
    shortest_job_first(processes, n);

    free(processes);

    return 0;
}