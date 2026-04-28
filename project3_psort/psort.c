/***********************************************
 * Projet avril 2026 : Tri parallèle
 * 
 * Etudiants : ARTOIS Victor (Matricule ?) et MANSY Théo (Matricule 264194)
 * Groupe n°4
 * 
 * Compile with :
 * gcc -Wall -Werror -O2 psort.c -o psort
 *
 * Run with : 
 * ./psort input output nb_threads
 * time ./psort benchmarks/benchmark_5000000.txt output.txt 4
 * 
 * Fonctionnement du programme :
 * 1) Il lit d'abord entièrement le fichier d'entrée en mémoire
 * 2) Il découpe les données chargées en plusieurs parties (chunks) en fonction du nombre de threads
 * 3) Chaque thread trie une partie du tableau en utilisant le merge sort mais l'ensemble 
 * 4) Les différentes parties triées sont fusionnées : d'abord les paires de chunks puis des groupes de plus en plus grands jusqu'à obtenir le tableau complet trié
 * 5) Le tableau trié est écrit dans le fichier de sortie
 * 
 * Le parallélisme est utilisé à deux niveaux :
 * 1) Le tri des chunks : le tableau est divisé en plusieurs parties, et chaque thread trie une partie indépendamment.
 * 2) La fusion des chunks : plusieurs opérations de merge sont exécutées en parallèle.
 *
***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    char key[4];
    char value[96];
} entry_t;

typedef struct {
    entry_t *entries;
    size_t left;
    size_t right;
} thread_arg_t;

typedef struct {
    entry_t *entries;
    size_t left;
    size_t mid;
    size_t right;
} merge_arg_t;

/* ===================== UTIL ===================== */

int compare_entries(const entry_t *a, const entry_t *b) {
    return memcmp(a->key, b->key, 4);
}

/* ===================== MERGE SORT ===================== */

void merge(entry_t *arr, size_t left, size_t mid, size_t right) {
    size_t n1 = mid - left + 1;
    size_t n2 = right - mid;

    entry_t *L = malloc(n1 * sizeof(entry_t));
    entry_t *R = malloc(n2 * sizeof(entry_t));

    if (!L || !R) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    memcpy(L, &arr[left], n1 * sizeof(entry_t));
    memcpy(R, &arr[mid + 1], n2 * sizeof(entry_t));

    size_t i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (compare_entries(&L[i], &R[j]) <= 0)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void merge_sort(entry_t *arr, size_t left, size_t right) {
    if (left >= right) return;

    size_t mid = left + (right - left) / 2;

    merge_sort(arr, left, mid);
    merge_sort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

/* ===================== THREAD ===================== */

void *thread_sort(void *arg) {
    thread_arg_t *t = (thread_arg_t *)arg;
    merge_sort(t->entries, t->left, t->right);
    return NULL;
}

void *thread_merge(void *arg) {
    merge_arg_t *m = (merge_arg_t *)arg;
    merge(m->entries, m->left, m->mid, m->right);
    return NULL;
}

/* ===================== FILE HANDLING ===================== */

void parse_arguments(int argc, char *argv[], char **input_path, char **output_path, int *threads_nb) {
    if (argc != 4) {
        fprintf(stderr, "usage: psort <input> <output> <nb_threads>\n");
        exit(1);
    }

    *input_path = argv[1];
    *output_path = argv[2];

    char *endptr;
    long tmp = strtol(argv[3], &endptr, 10);
    if (*endptr != '\0' || tmp < 1) {
        fprintf(stderr, "invalid number of threads\n");
        exit(1);
    }

    *threads_nb = (int) tmp;
}

void open_files(char *input_path, char *output_path, FILE **input, FILE **output) {
    *input = fopen(input_path, "r");
    if (!*input) {
        fprintf(stderr, "error: cannot open file '%s'\n", input_path);
        exit(1);
    }

    *output = fopen(output_path, "w");
    if (!*output) {
        fprintf(stderr, "error: cannot open file '%s'\n", output_path);
        fclose(*input);
        exit(1);
    }
}

void check_files_different(FILE *input, FILE *output) {
    struct stat s_input, s_output;

    if (fstat(fileno(input), &s_input) != 0 || fstat(fileno(output), &s_output) != 0) {
        fprintf(stderr, "error: pstat\n");
        exit(1);
    }

    if (s_input.st_ino == s_output.st_ino && s_input.st_dev == s_output.st_dev) {
        fprintf(stderr, "error: input and output file must differ\n");
        exit(1);
    }
}

entry_t *read_file(FILE *input, size_t *nb_entries) {
    struct stat s;

    if (fstat(fileno(input), &s) != 0) {
        perror("fstat");
        exit(1);
    }

    if (s.st_size % 100 != 0) {
        fprintf(stderr, "error: invalid file format\n");
        exit(1);
    }

    *nb_entries = s.st_size / 100;

    entry_t *entries = malloc(*nb_entries * sizeof(entry_t));
    if (!entries) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    size_t read = fread(entries, sizeof(entry_t), *nb_entries, input);
    if (read != *nb_entries) {
        free(entries);
        fprintf(stderr, "error: failed to read file\n");
        exit(1);
    }

    return entries;
}

void write_file(FILE *output, entry_t *entries, size_t nb_entries) {
    size_t written = fwrite(entries, sizeof(entry_t), nb_entries, output);
    if (written != nb_entries) {
        fprintf(stderr, "error: failed to write file\n");
        exit(1);
    }

    fsync(fileno(output));
}

/* ===================== MAIN ===================== */

int main(int argc, char *argv[]) {
    // Parse arguments + open input/output files + ensure that input and output files are different
    char *input_path;
    char *output_path;
    int threads_nb;
    FILE *input;
    FILE *output;

    parse_arguments(argc, argv, &input_path, &output_path, &threads_nb);
    open_files(input_path, output_path, &input, &output);
    check_files_different(input, output);

    // read input file
    size_t nb_entries;
    entry_t *entries = read_file(input, &nb_entries);

    // psort
    pthread_t threads[threads_nb];
    thread_arg_t args[threads_nb];

    size_t chunk = nb_entries / threads_nb;

    for (int i = 0; i < threads_nb; i++) {
        args[i].entries = entries;
        args[i].left = i * chunk;
        args[i].right = (i == threads_nb - 1)
            ? nb_entries - 1
            : (i + 1) * chunk - 1;

        pthread_create(&threads[i], NULL, thread_sort, &args[i]);
    }

    for (int i = 0; i < threads_nb; i++) {
        pthread_join(threads[i], NULL);
    }

    // pmerge
    size_t step = chunk;

    while (step < nb_entries) {
        pthread_t merge_threads[threads_nb];
        merge_arg_t merge_args[threads_nb];
        size_t active = 0;

        for (size_t i = 0; i < nb_entries; i += 2 * step) {
            size_t left = i;
            size_t mid = i + step - 1;
            size_t right = (i + 2 * step - 1 < nb_entries)
                ? i + 2 * step - 1
                : nb_entries - 1;

            if (mid >= right) continue;

            merge_args[active].entries = entries;
            merge_args[active].left = left;
            merge_args[active].mid = mid;
            merge_args[active].right = right;

            pthread_create(&merge_threads[active], NULL, thread_merge, &merge_args[active]);
            active++;

            if (active == threads_nb) {
                for (size_t j = 0; j < active; j++)
                    pthread_join(merge_threads[j], NULL);
                active = 0;
            }
        }

        for (size_t j = 0; j < active; j++) {
            pthread_join(merge_threads[j], NULL);
        }

        step *= 2;
    }

    // write output file
    write_file(output, entries, nb_entries);

    fclose(input);
    fclose(output);
    free(entries);
    exit(0);
}