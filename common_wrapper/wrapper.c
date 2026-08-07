/*
 * CS509 Common Wrapper (Individual Repo)
 *
 * Repo-level menu: lists assignments, compiles the selected one, and
 * runs either one test file or all test files for it. Calls each
 * assignment's own driver rather than reimplementing algorithm logic.
 *
 * Build:  gcc -o wrapper wrapper.c
 * Run:    ./wrapper     (from the repo root)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

typedef struct {
    const char *name;         /* Assignment name */
    const char *build_cmd;    /* Command to build the assignment */
    const char *driver_path;  /* Path to the assignment's driver executable */
    const char *tests_dir;    /* Directory containing test files */
} Assignment;

static Assignment assignments[] = {
    {
        "Assignment 01 - GEMM (Simple + Blocking)",
        "gcc -Wall -O2 -o Assignment01/driver/driver "
            "Assignment01/driver/driver.c "
            "Assignment01/src/matrix_multiplication.c "
            "Assignment01/src/tiling_multiplication.c "
            "Assignment01/src/readmatrix.c",
        "Assignment01/driver/driver",
        "Assignment01/tests"
    }
    /* Add one entry per future assignment */
};

#define NUM_ASSIGNMENTS (int)(sizeof(assignments) / sizeof(assignments[0]))

int build_assignment(const Assignment *asg) {
    printf("\n[build] %s\n", asg->build_cmd);
    int rc = system(asg->build_cmd);
    if (rc != 0) {
        fprintf(stderr, "Error: build failed for '%s'\n", asg->name);
        return -1;
    }
    return 0;
}

void run_single_test(const Assignment *asg, const char *test_file) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", asg->tests_dir, test_file);

    FILE *check = fopen(path, "r");
    if (!check) {
        fprintf(stderr, "Error: test file '%s' not found\n", path);
        return;
    }
    fclose(check);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "%s \"%s\"", asg->driver_path, path);
    printf("\n--- Running %s ---\n", test_file);
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: run failed for '%s'\n", test_file);
    }
}

void run_all_tests(const Assignment *asg) {
    DIR *d = opendir(asg->tests_dir);
    if (!d) {
        fprintf(stderr, "Error: could not open tests directory '%s'\n", asg->tests_dir);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len > 4 && strcmp(entry->d_name + len - 4, ".txt") == 0) {
            run_single_test(asg, entry->d_name);
        }
    }
    closedir(d);
}

static void print_menu(void) {
    printf("\n===== CS509 Common Wrapper =====\n");
    for (int i = 0; i < NUM_ASSIGNMENTS; i++) {
        printf("  %d) %s\n", i + 1, assignments[i].name);
    }
    printf("  %d) Compile and run ALL assignments\n", NUM_ASSIGNMENTS + 1);
    printf("  0) Exit\n");
    printf("Select: ");
}

int main(void) {
    int choice;

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Error: invalid input\n");
            while (getchar() != '\n');
            continue;
        }
        if (choice == 0) break;

        if (choice == NUM_ASSIGNMENTS + 1) {
            for (int i = 0; i < NUM_ASSIGNMENTS; i++) {
                if (build_assignment(&assignments[i]) == 0) {
                    run_all_tests(&assignments[i]);
                }
            }
            continue;
        }

        if (choice < 1 || choice > NUM_ASSIGNMENTS) {
            fprintf(stderr, "Error: no such assignment\n");
            continue;
        }

        Assignment *asg = &assignments[choice - 1];
        if (build_assignment(asg) != 0) continue;

        printf("\n  1) Run one test file\n");
        printf("  2) Run all test files\n");
        printf("Select: ");
        int sub;
        if (scanf("%d", &sub) != 1) { while (getchar() != '\n'); continue; }

        if (sub == 1) {
            char fname[256];
            printf("Enter test file name (e.g. gemm_test_01.txt): ");
            if (scanf("%255s", fname) == 1) {
                run_single_test(asg, fname);
            } else {
                fprintf(stderr, "Error: invalid test file name\n");
            }
        } else if (sub == 2) {
            run_all_tests(asg);
        } else {
            fprintf(stderr, "Error: invalid option\n");
        }
    }

    printf("Exiting.\n");
    return EXIT_SUCCESS;
}