/***********************************************
 * Project: Reverse
 * 
 * Group Number : 4
 * Students     : MANSY Théo
 * 
 * gcc -Wall -Werror -O2 reverse.c -o reverse
 * ./reverse OR ./reverse input.txt OR ./reverse input.txt output.txt
 ***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **lines;
    size_t count;
    size_t capacity;
} LineArray;

int main(int argc, char *argv[]) {
    if (argc <= 3) {
        FILE *inputFile = fopen(argv[1], "r");
        FILE *outputFile = fopen(argv[2], "w");
        
        if (!inputFile || !outputFile) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }

        char *line = NULL;
        size_t len = 0;
        LineArray array = {NULL, 0, 0};

        while (getline(&line, &len, inputFile) != -1) {
            if (array.count == array.capacity) {
                size_t new_capacity = (array.capacity == 0) ? 1 : array.capacity * 2;
                char **new_lines = malloc(new_capacity * sizeof(char *));

                if (new_lines == NULL) {
                    fprintf(stderr, "malloc failed\n");
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

            if (argc == 3) {
                fprintf(outputFile, "%s", array.lines[i]);

                if (array.lines[i][length - 1] != '\n') {
                    fprintf(outputFile, "\n");
                }
            } else {
                fprintf(stdout, "%s", array.lines[i]);

                if (array.lines[i][length - 1] != '\n') {
                    fprintf(stdout, "\n");
                }
            }

            free(array.lines[i]);
        }

        free(array.lines);
        free(line); 
        fclose(inputFile);
        exit(0);
    } else {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }
}