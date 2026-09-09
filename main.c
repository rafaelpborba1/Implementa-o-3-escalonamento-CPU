#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 32
#define SIM_TIME 20

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

int selecionar_tarefa(Task tasks[], int num_tasks, const char *algoritmo) {
    int escolhida = -1;

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].remaining_c > 0) {
            if (escolhida == -1) {
                escolhida = i;
            } else if (strcmp(algoritmo, "RM") == 0) {
                if (tasks[i].p < tasks[escolhida].p) {
                    escolhida = i;
                }
            } else if (strcmp(algoritmo, "EDF") == 0) {
                if (tasks[i].absolute_deadline < tasks[escolhida].absolute_deadline) {
                    escolhida = i;
                }
            }
        }
    }
    return escolhida;
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
    printf("Algoritmo: %s | Tarefas: %d | Utilizacao (U): %.2f%%\n\n", algoritmo, num_tasks, u * 100.0);

    int idle_ticks = 0;
    int context_switches = 0;
    int deadline_misses = 0;
    int prev_task_idx = -1;

    for (int t = 0; t < SIM_TIME; t++) {
        for (int i = 0; i < num_tasks; i++) {
            if (t == tasks[i].next_release) {
                tasks[i].remaining_c = tasks[i].c;
                tasks[i].absolute_deadline = t + tasks[i].d;
                tasks[i].next_release = t + tasks[i].p;
            }
        }
        int idx = selecionar_tarefa(tasks, num_tasks, algoritmo);

        if (idx != prev_task_idx && prev_task_idx != -1 && idx != -1) {
            context_switches++;
        }

        if (idx != -1) {
            printf("[%02d - %02d] Executando: %s (Restante: %d)\n", t, t + 1, tasks[idx].name, tasks[idx].remaining_c - 1);
            tasks[idx].remaining_c--;
        } else {
            printf("[%02d - %02d] CPU Ociosa\n", t, t + 1);
            idle_ticks++;
        }

        prev_task_idx = idx;

        for (int i = 0; i < num_tasks; i++) {
            if (tasks[i].remaining_c > 0 && (t + 1) >= tasks[i].absolute_deadline) {
                printf("  [AVISO] Perda de deadline na tarefa %s no tick %d!\n", tasks[i].name, t + 1);
                deadline_misses++;
            }
        }
    }

    printf("\n RESUMO DA SIMULACAO \n");
    printf("Tempo total simulado  : %d ticks\n", SIM_TIME);
    printf("Tempo de CPU Ociosa   : %d ticks (%.1f%%)\n", idle_ticks, ((double)idle_ticks / SIM_TIME) * 100.0);
    printf("Trocas de Contexto    : %d\n", context_switches);
    printf("Perdas de Deadline    : %d\n", deadline_misses);
    printf("\n");

    return EXIT_SUCCESS;
}