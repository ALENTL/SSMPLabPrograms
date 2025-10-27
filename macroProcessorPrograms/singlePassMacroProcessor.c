#include <stdio.h>
#include <stdlib.h> // Needed for NULL and potential error handling
#include <string.h>

// --- DATA STRUCTURES (TABLES) ---

// DEFTAB (Definition Table)
struct DEFTAB {
    char opcode[10];
    char operand[50];
} dt[50]; // Global counter 'n'

// NAMTAB (Name Table)
struct NAMTAB {
    char name[10];
    int start;
    int end;
} nt[10]; // Global counter 'm'

// ARGTAB (Argument Table)
struct ARGTAB {
    char arg[10];
} at[10];

// PARAMTAB (Parameter Table)
struct PARAMTAB {
    char param[10];
    char val[5]; // e.g., "?1"
} pt[10];        // Global counter 'p'

// Global counters
int n = 0; // DEFTAB index
int p = 0; // PARAMTAB index
int m = 0; // NAMTAB index

// --- UTILITY FUNCTIONS ---

// Function to check if parameter already exists (Corrected)
int check_param(char x[50]) {
    for (int i = 0; i < p; i++) {
        // Corrected comparison: use '== 0' for string equality
        if (strcmp(pt[i].param, x) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to check if macro name exists (Fixed brace placement)
int check_macro(char x[20]) {
    for (int i = 0; i < m; i++) {
        if (strcmp(nt[i].name, x) == 0) {
            return i;
        }
    }
    return -1;
}

// --- MAIN PROCESSING LOGIC ---

void store() {
    FILE *f, *fp, *argfile;
    char a[20], b[20], c[50], temp[20];
    int i, j, x;

    f = fopen("./MacroProcessorPrograms/INPUT.txt", "r");
    fp = fopen("./MacroProcessorPrograms/EXP.txt", "w");
    argfile = fopen("./MacroProcessorPrograms/ARGTAB.txt", "w");

    // Fix: Corrected comparison (f == NULL)
    if (f == NULL) {
        printf("Error: Cannot open INPUT.txt\n");
        return;
    }

    // Main loop to read input line by line (a: Label, b: Opcode/MacroName, c:
    // Operand/Parameters)
    while (fscanf(f, "%s%s%s", a, b, c) != EOF) {

        // ======================================
        // MACRO DEFINITION
        // ======================================
        if (strcmp(b, "MACRO") == 0) {
            strcpy(nt[m].name, a);
            nt[m].start = n;

            // Reset parameter counter for each new macro
            p = 0;

            // Fix: Corrected and robust parameter parsing for comma-separated
            // list
            j = 0;
            i = 0;
            // Iterate through the parameter string 'c'
            while (c[i] != '\0') {
                if (c[i] != ',') {
                    temp[j++] = c[i]; // Store character in temp
                } else {              // Comma encountered, parameter complete
                    temp[j] = '\0';   // Null-terminate the parameter string
                    strcpy(pt[p].param, temp);
                    sprintf(pt[p].val, "?%d", p + 1); // Use '?n' format
                    p++;   // Move to next parameter slot
                    j = 0; // Reset temp buffer index
                }
                i++;
            }
            // Store the last parameter (since no comma follows it)
            temp[j] = '\0';
            if (j > 0) { // Only store if the last parameter was non-empty
                strcpy(pt[p].param, temp);
                sprintf(pt[p].val, "?%d", p + 1);
                p++;
            }
            // End of Parameter Parsing Fix

            // Store macro definition into DEFTAB
            fscanf(f, "%s%s%s", a, b, c); // Read first line of macro body
            while (strcmp(b, "MEND") != 0) {
                strcpy(dt[n].opcode, b);

                // Fix: Simplified substitution logic for single parameter
                // replacement This is a minimal fix for the original broken
                // logic (lines 83-89) In a real processor, this should handle
                // multiple parameters and surrounding text.
                int replaced = 0;
                for (i = 0; i < p; i++) {
                    // Check if the parameter is the ENTIRE operand
                    if (strcmp(c, pt[i].param) == 0) {
                        strcpy(dt[n].operand, pt[i].val);
                        replaced = 1;
                        break;
                    }
                }
                if (!replaced) {
                    strcpy(dt[n].operand, c);
                }

                n++;
                // Read next line
                fscanf(f, "%s%s%s", a, b, c);
            }
            // Store MEND
            strcpy(dt[n].opcode, "MEND");
            strcpy(dt[n].operand, "");
            nt[m].end = n;
            n++;
            m++;
        }
        // ======================================
        // MACRO EXPANSION OR NORMAL LINE
        // ======================================
        else {
            i = check_macro(b); // check 2nd field for macro name

            if (i == -1) {
                // Normal line (not a macro call)
                fprintf(fp, "%s\t%s\t%s\n", a, b, c);
            } else {
                // Macro Expansion
                int st = nt[i].start;
                int en = nt[i].end;
                x = 0;
                j = 0;
                i = 0; // x: ARGTAB index

                // Fix: Corrected and robust argument parsing
                // Parse arguments (c) into ARGTAB
                while (c[i] != '\0') {
                    if (c[i] != ',') {
                        temp[j++] = c[i];
                    } else { // Comma encountered, argument complete
                        temp[j] = '\0';
                        strcpy(at[x++].arg, temp);
                        j = 0; // Reset temp buffer index
                    }
                    i++;
                }
                // Store the last argument
                temp[j] = '\0';
                if (j > 0) {
                    strcpy(at[x++].arg, temp);
                }
                // End of Argument Parsing Fix

                // Print ARGTAB for the current invocation
                printf("\n------------------------------");
                printf("\nARGUMENT TABLE (ARGTAB) for %s", b);
                printf("\n------------------------------");
                printf("\nINDEX\tARGUMENT");
                fprintf(argfile, "\nARGTAB for %s\n", b);
                fprintf(argfile, "INDEX\tARGUMENT\n");
                for (int k = 0; k < x; k++) {
                    printf("\n%d\t%s", k, at[k].arg);
                    fprintf(argfile, "%d\t%s\n", k, at[k].arg);
                }

                // Expand macro (i is already holding the NAMTAB index, use k
                // for DEFTAB index) Start from st + 1 to skip the MACRO line
                for (int k = st + 1; k < en; k++) {
                    // MEND should not be reached if end index is correct, but
                    // check anyway
                    if (strcmp(dt[k].opcode, "MEND") == 0)
                        break;

                    fprintf(fp, "%s\t%s\t", a, dt[k].opcode);

                    // Check if operand is a positional parameter
                    if (dt[k].operand[0] == '?') {
                        // Positional index is ASCII value of '1' minus ASCII
                        // value of '1'
                        int y = dt[k].operand[1] - '1';
                        // Check for valid index to prevent segmentation fault
                        if (y >= 0 && y < x) {
                            fprintf(fp, "%s\n", at[y].arg);
                        } else {
                            // Error: Missing argument
                            fprintf(fp, "MISSING_ARG\n");
                        }
                    } else {
                        // Direct operand (not a parameter)
                        fprintf(fp, "%s\n", dt[k].operand);
                    }
                }
            }
        }
    }
    fclose(f);
    fclose(fp);
    fclose(argfile);
}

// --- DISPLAY LOGIC ---

void display_tables() {
    // Fix: Corrected file pointer declarations
    FILE *f1, *f2, *f3;
    int i;

    // NAMTAB
    printf("\n----------------------");
    printf("\nNAME TABLE (NAMTAB)");
    printf("\n----------------------");
    printf("\nNAME\tSTART\tEND");
    f1 = fopen("./MacroProcessorPrograms/NAMTAB.txt", "w");
    fprintf(f1, "NAME\tSTART\tEND\n");

    // Fix: Corrected loop condition (i < m)
    for (i = 0; i < m; i++) {
        printf("\n%s\t%d\t%d", nt[i].name, nt[i].start, nt[i].end);
        fprintf(f1, "%s\t%d\t%d\n", nt[i].name, nt[i].start, nt[i].end);
    }
    fclose(f1);

    // DEFTAB
    printf("\n\n-----------------------------");
    printf("\nDEFINITION TABLE (DEFTAB)");
    printf("\n-----------------------------");
    printf("\nINDEX\tOPCODE\tOPERAND");
    f2 = fopen("./MacroProcessorPrograms/DEFTAB.txt", "w");
    fprintf(f2, "INDEX\tOPCODE\tOPERAND\n");
    for (i = 0; i < n; i++) {
        printf("\n%d\t%s\t%s", i, dt[i].opcode, dt[i].operand);
        fprintf(f2, "%d\t%s\t%s\n", i, dt[i].opcode, dt[i].operand);
    }
    fclose(f2);

    // PARAMTAB
    printf("\n\n--------------------------------");
    printf("\nPARAMETER TABLE (PARAMTAB) (Last Macro Only)");
    printf("\n--------------------------------");
    printf("\nPARAM\tVAL");
    f3 = fopen("./MacroProcessorPrograms/PARAMTAB.txt", "w");
    fprintf(f3, "PARAM\tVAL\n");
    for (i = 0; i < p; i++) {
        printf("\n%s\t%s", pt[i].param, pt[i].val);
        fprintf(f3, "%s\t%s\n", pt[i].param, pt[i].val);
    }
    fclose(f3);
}

// --- EXECUTION ---

int main() {
    store();
    display_tables();
    printf("\n\nMacro expansion completed.\n");
    printf("Generated files: \n");
    printf("- EXP.txt (Expanded Code)\n");
    printf("- NAMTAB.txt\n- DEFTAB.txt\n- PARAMTAB.txt\n- ARGTAB.txt\n");
    return 0;
}