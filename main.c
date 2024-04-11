//
//  main.c
//  Banker’s-Algorithm
//
//  Created by Behrouz Barati on 3/19/24.
//

#include <stdio.h>
#include <stdlib.h>

// Declare dynamic arrays/vectors and global variables
int *resource, *available;
int **maxClaim, **allocated, **need;
int numResources, numProcesses;

//*********************************************************
// Print a vector with a title
void printVector(int *vector, int size, char *title) {
    // Declare local variables
    
    printf("%s:\n", title);
    // For loop j: print each resource index
    for(int j = 0; j < size; j++)
        printf("\t\tr%d ", j);
    printf("\n");
    // For loop j: print value of vector[j]
    for(int j = 0; j < size; j++)
        printf("\t\t%d", vector[j]);
    printf("\n\n");
}

//*************************************************************
// Print a matrix with a title
void printMatrix(int **matrix, int numRows, int numCols, char *title) {
    // Declare local variables
    printf("%s:\n   ", title);
    // For loop j: print each resource index
    for(int j = 0; j < numCols; j++)
        printf("\t\tr%d ", j);
    printf("\n");
    // For each process i:
    for(int i = 0; i < numRows; i++) {
        printf("p%d ", i);
        // For each resource j:
        for(int j = 0; j < numCols; j++)
            // Print value of matrix[i][j]
            printf("\t\t%d", matrix[i][j]);
        printf("\n");
    }
    printf("\n");
}

//**************************************************************
// Option 1: Enter resource claim graph data
void option1(void) {
    // Declare local variables
    printf("Enter number of resources: ");
    scanf("%d", &numResources);
    resource = (int *)malloc(numResources * sizeof(int));
    available = (int *)malloc(numResources * sizeof(int));

    printf("Enter number of units for resources (r0 to r%d): ", numResources - 1);
    for(int i = 0; i < numResources; i++) {
        scanf("%d", &resource[i]);
        available[i] = resource[i]; // Initially, all resources are available
    }

    printf("Enter number of processes: ");
    scanf("%d", &numProcesses);
    maxClaim = (int **)malloc(numProcesses * sizeof(int *));
    allocated = (int **)malloc(numProcesses * sizeof(int *));
    need = (int **)malloc(numProcesses * sizeof(int *));
    for(int i = 0; i < numProcesses; i++) {
        maxClaim[i] = (int *)malloc(numResources * sizeof(int));
        allocated[i] = (int *)malloc(numResources * sizeof(int));
        need[i] = (int *)malloc(numResources * sizeof(int));
    }
    // First, collect all the maximum claim data
    for(int i = 0; i < numProcesses; i++) {
        printf("Enter maximum number of units process p%d will claim from each resource (r0 to r%d): ", i, numResources - 1);
        for(int j = 0; j < numResources; j++) {
            scanf("%d", &maxClaim[i][j]);
            // Initialize need array to maxClaim since allocation is 0 at this point
            need[i][j] = maxClaim[i][j];
        }
    }
        
    // Then, collect all the allocation data and adjust the available and need matrices
    for(int i = 0; i < numProcesses; i++) {
        printf("Enter number of units of each resource (r0 to r%d) currently allocated to process p%d: ", numResources - 1, i);
        for(int j = 0; j < numResources; j++) {
            scanf("%d", &allocated[i][j]);
            need[i][j] -= allocated[i][j];
            available[j] -= allocated[i][j];
        }
    }

    // Print resource vector, available vector, maxclaim array, allocated array, need array
    printVector(resource, numResources, "Resources");
    printVector(available, numResources, "Available");
    printMatrix(maxClaim, numProcesses, numResources, "Max Claim");
    printMatrix(allocated, numProcesses, numResources, "Allocated");
    printMatrix(need, numProcesses, numResources, "Need");
}

//**************************************************************
// Option 2: Request resource by a process
void option2(void) {
    // Declare local variables
    int process, resourceIndex, units;
    // Prompt for process, resource, and number of units requested
    printf("Enter requesting process: p");
    scanf("%d", &process);
    printf("Enter requested resource: r");
    scanf("%d", &resourceIndex);
    printf("Enter number of units process p%d is requesting from resource r%d: ", process, resourceIndex);
    scanf("%d", &units);

    // If enough units available and request is less than need
    if(units <= available[resourceIndex] && units <= need[process][resourceIndex]) {
        // Reduce number of available units
        available[resourceIndex] -= units;
        // Increase number of allocated units
        allocated[process][resourceIndex] += units;
        // Reduce number of needed units
        need[process][resourceIndex] -= units;
        // Print updated available, allocated, and need vectors/matrices
        printf("\nRequest granted.\n");
    } else {
        // Print message that request was denied
        printf("\nRequest denied.\n");
    }

    printVector(available, numResources, "Available after request");
    printMatrix(allocated, numProcesses, numResources, "Allocated after request");
    printMatrix(need, numProcesses, numResources, "Need after request");
}

//**************************************************************
// Option 3: Release resource from a process
void option3(void) {
    // Declare local variables
    int process, resourceIndex, units;
    
    // Prompt for process, resource, and number of units requested
    printf("Enter releasing processor: p");
    scanf("%d", &process);
    printf("Enter released resource: r");
    scanf("%d", &resourceIndex);
    printf("Enter number of units process p%d is releasing from resource r%d: ", process, resourceIndex);
    scanf("%d", &units);

    // If enough units allocated
    if(units <= allocated[process][resourceIndex]) {
        // Increase number of available units
        available[resourceIndex] += units;
        // Reduce number of allocated units
        allocated[process][resourceIndex] -= units;
        // Increase number of needed units
        need[process][resourceIndex] += units;
        // Print updated available, allocated, and need vectors/matrices
        printf("\nRelease processed.\n");
    } else {
        // Print message that release cannot be performed
        printf("\nRelease cannot be performed.\n");
    }

    printVector(available, numResources, "Available after release");
    printMatrix(allocated, numProcesses, numResources, "Allocated after release");
    printMatrix(need, numProcesses, numResources, "Need after release");
}

//******************************************************************
// Option 4: Determine safe sequence of processes
void option4(void) {
    // Declare local variables
    int safeSequence[numProcesses];
    int finished[numProcesses]; // Tracks which processes have finished
    int work[numResources]; // Temporary array to hold available resources during algorithm execution
    for (int i = 0; i < numProcesses; i++)
        finished[i] = 0; // Initialize all processes as not finished
    for (int i = 0; i < numResources; i++)
        work[i] = available[i]; // Initialize work as a copy of available resources

    int numFinished = 0; // Count of finished (safe-sequence included) processes
    // While not all processes are processed
    while (numFinished < numProcesses) {
        int processFound = 0; // Flag to check if any process can proceed in current iteration
        // For each process
        for (int i = 0; i < numProcesses; i++) {
            if (!finished[i]) { // Process i hasn't finished yet
                int j;
                // Print current comparison between need and work vectors
                printf("Comparing: <");
                for (j = 0; j < numResources; j++)
                    printf(" %d", need[i][j]); // Print need vector for process i
                printf(" > <= <");
                for (j = 0; j < numResources; j++)
                    printf(" %d", work[j]); // Print current work (available resources)
                printf(" > : ");
                
                for (j = 0; j < numResources; j++) {
                    // Check for safe processing by comparing process' need vector to available vector
                    if (need[i][j] > work[j])
                        break; // Not enough resources for this process
                }
                if (j == numResources) { // All resources for this process are available
                    // For each resource
                    for (int k = 0; k < numResources; k++) {
                        // Update number of available units of resource
                        work[k] += allocated[i][k]; // Assume process releases resources
                    }
                    // Increment number of sequenced processes
                    safeSequence[numFinished++] = i; // Add this process to the safe sequence
                    finished[i] = 1; // Mark as finished
                    processFound = 1;
                    printf("Safe to process p%d\n", i);
                } else {
                    printf("Not safe to process p%d\n", i);
                }
            }
        }
        // If (no process was processed in the final round of the for-loop)
        if (!processFound) {
            // Print message of deadlock among processes not processed
            printf("Deadlock detected among processes not processed.\n");
            return; // If no process can proceed, a deadlock is present
        }
    }
    // Else print safe sequence of processes
    printf("Safe sequence of processes: ");
    for (int i = 0; i < numFinished; i++)
        printf("p%d ", safeSequence[i]);
    printf("\n\n");
}

//******************************************************************
// Option 5: Quit program and free memory
void option5(void) {
    // Check if vectors/array are not NULL--if so, free each vector/array
    if (resource != NULL) free(resource);
    if (available != NULL) free(available);
    for (int i = 0; i < numProcesses; i++) {
        if (maxClaim != NULL && maxClaim[i] != NULL) free(maxClaim[i]);
        if (allocated != NULL && allocated[i] != NULL) free(allocated[i]);
        if (need != NULL && need[i] != NULL) free(need[i]);
    }
    if (maxClaim != NULL) free(maxClaim);
    if (allocated != NULL) free(allocated);
    if (need != NULL) free(need);

    printf("Memory has been freed. Exiting program.\n");
}

//*************************************************************
// Main function
int main(void) {
    // Declare local vars
    int choice;
    // While user has not chosen to quit
    do {
        // Print menu of options
        printf("Banker's Algorithm\n------------------\n1) Enter resource claim graph data\n2) Request resource by a process\n3) Release resource from a process\n4) Determine safe sequence of processes\n5) Quit program\n\nEnter selection: ");
        // Prompt for menu selection
        scanf("%d", &choice);
        // Call appropriate procedure based on choice--use switch statement or series of if, else if, else statements
        switch(choice) {
            case 1:
                option1();
                break;
            case 2:
                option2();
                break;
            case 3:
                option3();
                break;
            case 4:
                option4();
                break;
            case 5:
                option5();
                break;
            default:
                printf("Invalid selection. Please try again.\n");
        }
    } while(choice != 5); // while loop end
    return 0; // Indicates success
} // End of procedure
