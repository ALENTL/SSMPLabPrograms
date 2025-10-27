#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int completionTime;
    int turnAroundTime;
    int waitingTime;
} Process;

void displayResults(Process *p, int n);


void calculateFcfsTimes(Process *p, int n) {
    int currentTime = 0;

    for (int i=0; i<n-1; i++) {
        for (int j=0; j<n-i-1; j++) {
            if (p[j].arrivalTime > p[j+1].arrivalTime) {
                Process temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
        }
    }

    for (int i=0; i<n; i++) {
        if (currentTime < p[i].arrivalTime) {
            currentTime = p[i].arrivalTime;
        }

        p[i].completionTime = currentTime + p[i].burstTime;
        p[i].turnAroundTime = p[i].completionTime - p[i].arrivalTime;
        p[i].waitingTime = p[i].turnAroundTime - p[i].burstTime;

        currentTime = p[i].completionTime;
    }

    displayResults(p, n);
}

void displayResults(Process *p, int n) {
    float totalWaitingTime = 0;
    float totalTurnAroundTime = 0;

    printf("\n------------------------------------------------------------------------\n");
    printf("| PID | ArrivalTime | BurstTime | CompletionTime | TurnaroundTime | WaitingTime |\n");
    printf("------------------------------------------------------------------------\n");

    for (int i=0; i<n; i++) {
        totalWaitingTime += p[i].waitingTime;
        totalTurnAroundTime += p[i].turnAroundTime;

    printf("| P%-3d | %11d | %9d | %14d | %14d | %11d |\n",
               p[i].pid, 
               p[i].arrivalTime, 
               p[i].burstTime, 
               p[i].completionTime, 
               p[i].turnAroundTime, 
               p[i].waitingTime);
    }
    printf("------------------------------------------------------------------------\n");

    printf("\nAverage Turnaround Time: %.2f\n", totalTurnAroundTime/n);
    printf("Average Waiting Time: %.2f\n", totalWaitingTime/n);
}

int main() {
    int n;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    Process *p = malloc(n * sizeof(Process));

    if (p == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    printf("\nInput Process Details: \n");

    for (int i=0; i<n; i++) {
        p[i].pid = i+1;

        printf("P%d Arrival Time: ", i+1);
        scanf("%d", &p[i].arrivalTime);

        printf("P%d Burst Time: ", i+1);
        scanf("%d", &p[i].burstTime);
        printf("\n");
    }

    calculateFcfsTimes(p, n);
    free(p);

    return 0;
}