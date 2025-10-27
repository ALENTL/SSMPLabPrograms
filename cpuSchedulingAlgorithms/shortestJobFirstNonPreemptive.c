#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int pid;
    int arrivalTime;
    int burstTime;
    int completionTime;
    int turnAroundTime;
    int waitingTime;
    int isCompleted;
} Process;

void calculateSJFTimes(Process* p, int n);
void displayResults(Process* p, int n);

int main()
{
    int n;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    Process* p = malloc(n * sizeof(Process));

    if (p == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    printf("\nEnter Process Details\n");

    for (int i = 0; i < n; i++)
    {
        p[i].pid = i + 1;
        printf("\nP%d Arrival Time: ", i + 1);
        scanf("%d", &p[i].arrivalTime);

        printf("P%d Burst Time: ", i + 1);
        scanf("%d", &p[i].burstTime);

        p[i].completionTime = 0;
        p[i].turnAroundTime = 0;
        p[i].turnAroundTime = 0;
        p[i].waitingTime = 0;
        p[i].isCompleted = 0;
    }

    printf("\n");

    calculateSJFTimes(p, n);
    displayResults(p, n);

    free(p);

    return 0;
}

void calculateSJFTimes(Process* p, int n)
{
    int currentTime = 0;
    int completed = 0;

    while (completed != n)
    {
        int shortestBurst = INT_MAX;
        int shortestJobIndex = -1;

        for (int i = 0; i < n; i++)
        {
            if (p[i].arrivalTime <= currentTime && p[i].isCompleted == 0)
            {
                if (p[i].burstTime < shortestBurst)
                {
                    shortestBurst = p[i].burstTime;
                    shortestJobIndex = i;
                }

                else if (p[i].burstTime == shortestBurst)
                {
                    if (shortestJobIndex != -1 &&
                        p[i].arrivalTime < p[shortestJobIndex].arrivalTime)
                    {
                        shortestJobIndex = i;
                    }
                }
            }
        }

        if (shortestJobIndex != -1)
        {
            int i = shortestJobIndex;
            p[i].completionTime = currentTime + p[i].burstTime;
            currentTime = p[i].completionTime;
            p[i].turnAroundTime = p[i].completionTime - p[i].arrivalTime;
            p[i].waitingTime = p[i].turnAroundTime - p[i].burstTime;
            p[i].isCompleted = 1;

            completed++;
        }

        else
        {
            int nextArrivalTime = INT_MAX;

            for (int i = 0; i < n; i++)
            {
                if (p[i].isCompleted == 0 && p[i].arrivalTime < nextArrivalTime)
                {
                    nextArrivalTime = p[i].arrivalTime;
                }
            }

            if (nextArrivalTime != -1)
            {
                currentTime = nextArrivalTime;
            }

            else
            {
                break;
            }
        }
    }
}

void displayResults(Process* p, int n)
{
    float totalWt = 0.0;
    float totalTat = 0.0;

    printf("\n------------------------------------------------------------------------\n");
    printf("| PID | ArrivalTime | BurstTime | CompletionTime | TurnaroundTime | WaitingTime |\n");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++)
    {
        totalWt += p[i].waitingTime;
        totalTat += p[i].turnAroundTime;
        printf("| P%-3d | %11d | %9d | %14d | %14d | %11d |\n", p[i].pid, p[i].arrivalTime,
               p[i].burstTime, p[i].completionTime, p[i].turnAroundTime, p[i].waitingTime);
    }
    printf("------------------------------------------------------------------------\n");

    printf("\nAverage Turnaround Time: %.2f\n", totalTat / n);
    printf("Average Waiting Time: %.2f\n", totalWt / n);
}