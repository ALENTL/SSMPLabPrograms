#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int startTime;
    int remainingTime;
    int completionTime;
    int turnAroundTime;
    int waitingTime;
    int isCompleted;
} Process;

void calculateRoundRobinTimes(Process *p, int n, int timeQuantum);
void addArrivedProcesses(Process *p, int n, int endTime, int *readyQueue,
                         int *tail);
void displayResults(Process *p, int n);

int main() {
    int n;
    int timeQuantum;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    printf("Enter Time Quantum: ");
    scanf("%d", &timeQuantum);

    Process *p = malloc(n * sizeof(Process));

    if (p == NULL) {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;

        printf("\nP%d Arrival Time: ", i + 1);
        scanf("%d", &p[i].arrivalTime);

        printf("P%d Burst Time: ", i + 1);
        scanf("%d", &p[i].burstTime);
    }
    printf("\n");

    calculateRoundRobinTimes(p, n, timeQuantum);
    displayResults(p, n);

    free(p);

    return 0;
}

void calculateRoundRobinTimes(Process *p, int n, int timeQuantum) {
    int currentTime = 0;
    int completed = 0;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].arrivalTime > p[j + 1].arrivalTime) {
                Process temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        p[i].remainingTime = p[i].burstTime;
        p[i].isCompleted = 0;
        p[i].startTime = -1;
    }

    int *readyQueue = malloc(n * 5 * sizeof(int));
    int ganttOrder[200], ganttStart[200], ganttEnd[200];
    int ganttIndex = 0;

    int head = 0, tail = 0;
    addArrivedProcesses(p, n, 0, readyQueue, &tail);

    while (completed < n) {
        if (head < tail) {
            int i = readyQueue[head++];

            int execTime = (p[i].remainingTime < timeQuantum)
                               ? p[i].remainingTime
                               : timeQuantum;
            if (p[i].startTime == -1) {
                p[i].startTime = currentTime;
            }

            ganttOrder[ganttIndex] = p[i].pid;
            ganttStart[ganttIndex] = currentTime;

            currentTime += execTime;
            p[i].remainingTime -= execTime;

            ganttEnd[ganttIndex] = currentTime;
            ganttIndex++;

            addArrivedProcesses(p, n, currentTime, readyQueue, &tail);

            if (p[i].remainingTime == 0) {
                p[i].completionTime = currentTime;
                p[i].turnAroundTime = p[i].completionTime - p[i].arrivalTime;
                p[i].waitingTime = p[i].turnAroundTime - p[i].burstTime;
                p[i].isCompleted = 2;
                completed++;
            } else {
                readyQueue[tail++] = i;
            }
        } else {
            int nextArrival = INT_MAX;

            for (int i = 0; i < n; i++) {
                if (p[i].isCompleted != 2 && p[i].arrivalTime < nextArrival) {
                    nextArrival = p[i].arrivalTime;
                }

                if (nextArrival != INT_MAX) {
                    currentTime = nextArrival;
                    addArrivedProcesses(p, n, currentTime, readyQueue, &tail);
                } else {
                    break;
                }
            }
        }
    }

    printf("\n================ GANTT CHART ================\n");
    for (int i = 0; i < ganttIndex; i++) {
        printf("%d | P%d | ", ganttStart[i], ganttOrder[i]);
    }
    printf("%d\n", ganttEnd[ganttIndex - 1]);

    printf("---------------------------------------------\n");
    for (int i = 0; i < ganttIndex; i++) {
        printf("| P%d ", ganttOrder[i]);
    }
    printf("|\n");

    printf("%d", ganttStart[0]);
    for (int i = 0; i < ganttIndex; i++) {
        printf("    %d", ganttEnd[i]);
    }
    printf("\n=============================================\n");

    free(readyQueue);
}

void addArrivedProcesses(Process *p, int n, int endTime, int *readyQueue,
                         int *tail) {
    for (int i = 0; i < n; i++) {
        if (p[i].isCompleted == 0 && p[i].arrivalTime <= endTime) {
            readyQueue[(*tail)++] = i;
            p[i].isCompleted = 1;
        }
    }
}

void displayResults(Process *p, int n) {
    float totalWt = 0.0, totalTat = 0.0;

    printf("\n================================================================="
           "====================\n");
    printf("| PID | ArrivalTime | BurstTime | CompletionTime | TurnaroundTime "
           "| WaitingTime |\n");
    printf("==================================================================="
           "==================\n");

    for (int i = 0; i < n; i++) {
        totalWt += p[i].waitingTime;
        totalTat += p[i].turnAroundTime;
        printf("| %-3d | %-11d | %-9d | %-14d | %-14d | %-11d |\n", p[i].pid,
               p[i].arrivalTime, p[i].burstTime, p[i].completionTime,
               p[i].turnAroundTime, p[i].waitingTime);
    }

    printf("==================================================================="
           "==================\n");
    printf("\nAverage Turnaround Time = %.2f", totalTat / n);
    printf("\nAverage Waiting Time    = %.2f\n", totalWt / n);
}