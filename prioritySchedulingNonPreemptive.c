#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;
    int completionTime;
    int turnAroundTime;
    int waitingTime;
    int isCompleted;
} Process;

void calculatePriorityNonPreemptive(Process *p, int n);
void displayResults(Process *p, int n);

int main() {
    int n;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    Process *p = malloc(n * sizeof(Process));

    if (p == NULL) {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    printf("\nInput Process Details\n");

    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;

        printf("\nP%d Arrival Time: ", i + 1);
        scanf("%d", &p[i].arrivalTime);

        printf("P%d Burst Time: ", i + 1);
        scanf("%d", &p[i].burstTime);

        printf("P%d Priority: ", i + 1);
        scanf("%d", &p[i].priority);

        p[i].completionTime = 0;
        p[i].turnAroundTime = 0;
        p[i].waitingTime = 0;
        p[i].isCompleted = 0;
    }
    printf("\n");

    calculatePriorityNonPreemptive(p, n);
    displayResults(p, n);

    free(p);

    return 0;
}

void calculatePriorityNonPreemptive(Process *p, int n) {
    int currentTime = 0;
    int completed = 0;

    while (completed < n) {
        int highestPriority = INT_MIN;
        int selectedJobIndex = -1;

        for (int i = 0; i < n; i++) {
            if (p[i].arrivalTime <= currentTime && p[i].isCompleted == 0) {
                if (p[i].priority > highestPriority) {
                    highestPriority = p[i].priority;
                    selectedJobIndex = i;
                }

                else if (p[i].priority == highestPriority &&
                         p[i].arrivalTime < p[selectedJobIndex].arrivalTime) {
                    selectedJobIndex = i;
                }
            }
        }

        if (selectedJobIndex != -1) {
            int i = selectedJobIndex;

            p[i].completionTime = currentTime + p[i].burstTime;
            currentTime = p[i].completionTime;

            p[i].turnAroundTime = p[i].completionTime - p[i].arrivalTime;
            p[i].waitingTime = p[i].turnAroundTime - p[i].burstTime;

            p[i].isCompleted = 1;
            completed++;
        }

        else {
            int nextArrivalTime = INT_MAX;

            for (int i = 0; i < n; i++) {
                if (p[i].isCompleted == 0 &&
                    p[i].arrivalTime < nextArrivalTime) {
                    nextArrivalTime = p[i].arrivalTime;
                }
            }

            if (nextArrivalTime != -1) {
                currentTime = nextArrivalTime;
            }

            else {
                break;
            }
        }
    }
}

void displayResults(Process *p, int n) {
    float totalWt = 0.0;
    float totalTat = 0.0;

    printf("\n-----------------------------------------------------------------"
           "---------------\n");
    printf("| PID | ArrivalTime | BurstTime | Priority | CompletionTime | "
           "TurnaroundTime | WaitingTime |\n");
    printf("-------------------------------------------------------------------"
           "-------------\n");

    for (int i = 0; i < n; i++) {
        totalWt += p[i].waitingTime;
        totalTat += p[i].turnAroundTime;
        printf("| P%-3d | %11d | %9d | %8d | %14d | %14d | %11d |\n", p[i].pid,
               p[i].arrivalTime, p[i].burstTime, p[i].priority,
               p[i].completionTime, p[i].turnAroundTime, p[i].waitingTime);
    }
    printf("-------------------------------------------------------------------"
           "-------------\n");

    printf("\nAverage Turnaround Time: %.2f\n", totalTat / n);
    printf("Average Waiting Time: %.2f\n", totalWt / n);
}