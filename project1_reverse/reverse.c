/***********************************************
 * Project: Reverse
 * 
 * Group Number : 4
 * Students     : ARTOIS Victor et MANSY Théo (264194)
 * 
 * gcc -Wall -Werror -O2 reverse.c -o reverse
 * ./reverse OR ./reverse input.txt OR ./reverse input.txt output.txt
 ***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    char **lines;
    size_t count;
    size_t capacity;
} LineArray;

int main(int argc, char *argv[]) {    
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    FILE *input = stdin;
    FILE *output = stdout;
    
    if (argc >= 2) {
        input = fopen(argv[1], "r");

        if (!input) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    if (argc == 3) {
        output = fopen(argv[2], "w");

        if (!output) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            fclose(input);
            exit(1);
        }

        struct stat s_input, s_output;

        if (fstat(fileno(input), &s_input) != 0 || fstat(fileno(output), &s_output) != 0) {
            fprintf(stderr, "error: pstat\n");
            fclose(input);
            fclose(output);
            exit(1);
        }

        if (s_input.st_ino == s_output.st_ino && s_input.st_dev == s_output.st_dev) {
            fprintf(stderr, "error: input and output file must differ\n");
            fclose(input);
            fclose(output);
            exit(1);
        }
    }

    char *line = NULL;
    size_t len = 0;
    LineArray array = {NULL, 0, 0};

    while (getline(&line, &len, input) != -1) {
        // Allocation dynamique avec malloc
        if (array.count == array.capacity) {
            size_t new_capacity = (array.capacity == 0) ? 1 : array.capacity * 2;
            char **new_lines = malloc(new_capacity * sizeof(char *));

            if (new_lines == NULL) {
                fprintf(stderr, "malloc failed\n");
                fclose(input);
                fclose(output);
                exit(1);
            }

            for (size_t i = 0; i < array.count; i++) {
                new_lines[i] = array.lines[i];
            }

            free(array.lines);
            array.lines = new_lines;
            array.capacity = new_capacity;
        }

        array.lines[array.count++] = strdup(line);
    }

    for (ssize_t i = array.count - 1; i >= 0; i--) {
        size_t length = strlen(array.lines[i]);
        fprintf(output, "%s", array.lines[i]);

        if (length == 0 || array.lines[i][length - 1] != '\n') {
            fprintf(output, "\n");
        }

        free(array.lines[i]);
    }

    free(array.lines);
    free(line);

    if (argc >= 2) {
        fclose(input);
    }

    if (argc == 3) {
        fclose(output);
    }
        
    exit(0);
}