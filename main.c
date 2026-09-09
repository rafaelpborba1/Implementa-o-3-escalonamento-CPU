#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 32

typedef struct {
    char name[32];
    int c;
    int p;
    int d;
    int remaining_c;
    int next_release;
    int absolute_deadline;
} Task;

double calcular_utilizacao(Task tasks[], int num_tasks) {
    double u = 0.0;
    for (int i = 0; i < num_tasks; i++) {
        u += (double)tasks[i].c / tasks[i].p;
    }
    return u;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <RM|EDF> <arquivo_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *algoritmo = argv[1];
    char *arquivo_entrada = argv[2];

    if (strcmp(algoritmo, "RM") != 0 && strcmp(algoritmo, "EDF") != 0) {
        fprintf(stderr, "Erro: Algoritmo '%s' invalido. Escolha 'RM' ou 'EDF'.\n", algoritmo);
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(arquivo_entrada, "r");
    if (!fp) {
        perror("Erro ao abrir o arquivo de tarefas");
        return EXIT_FAILURE;
    }

    Task tasks[MAX_TASKS];
    int num_tasks = 0;

    while (num_tasks < MAX_TASKS && 
           fscanf(fp, "%31s %d %d %d", 
                  tasks[num_tasks].name, 
                  &tasks[num_tasks].c, 
                  &tasks[num_tasks].p, 
                  &tasks[num_tasks].d) == 4) {
        
        tasks[num_tasks].remaining_c = 0;
        tasks[num_tasks].next_release = 0;
        tasks[num_tasks].absolute_deadline = 0;
        num_tasks++;
    }

    fclose(fp);

    double u = calcular_utilizacao(tasks, num_tasks);
    printf("Algoritmo: %s | Tarefas: %d | Utilizacao (U): %.2f%%\n", algoritmo, num_tasks, u * 100.0);

    if (u > 1.0) {
        printf("Aviso: Sistema NAO escalonavel (U > 100%%).\n");
    } else {
        printf("Sistema potencialmente escalonavel.\n");
    }

    return EXIT_SUCCESS;
}