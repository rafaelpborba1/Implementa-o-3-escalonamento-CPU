#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TASKS 32
#define LOGIN "rpb" 

typedef struct {
    char name[32];
    int c;
    int p;
    int d;
    int remaining_c;
    int next_release;
    int absolute_deadline;
    int completed_count;
    int lost_count;
    int killed_count;
} Task;

int selecionar_tarefa(Task tasks[], int num_tasks, const char *algoritmo) {
    int escolhida = -1;

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].remaining_c > 0) {
            if (escolhida == -1) {
                escolhida = i;
            } else if (strcmp(algoritmo, "rate") == 0) {
                if (tasks[i].p < tasks[escolhida].p) {
                    escolhida = i;
                }
            } else if (strcmp(algoritmo, "edf") == 0) {
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
        fprintf(stderr, "Erro: Uso correto: %s <rate|edf> <arquivo_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *algoritmo = argv[1];
    char *arquivo_entrada = argv[2];

    if (strcmp(algoritmo, "rate") != 0 && strcmp(algoritmo, "edf") != 0) {
        fprintf(stderr, "Erro: Algoritmo invalido '%s'. Escolha 'rate' ou 'edf'.\n", algoritmo);
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(arquivo_entrada, "r");
    if (!fp) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo '%s'.\n", arquivo_entrada);
        return EXIT_FAILURE;
    }

    int sim_time = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d", &sim_time) == 1 && sim_time > 0) {
            break;
        }
    }

    if (sim_time <= 0) {
        fprintf(stderr, "Erro: Tempo total de simulacao invalido ou nao positivo.\n");
        fclose(fp);
        return EXIT_FAILURE;
    }

    Task tasks[MAX_TASKS];
    int num_tasks = 0;

    while (fgets(line, sizeof(line), fp) && num_tasks < MAX_TASKS) {
        for (int i = 0; line[i]; i++) {
            if (line[i] == ',') line[i] = ' ';
        }

        char name[32];
        int p, d, c;

        int ret = sscanf(line, "%31s %d %d %d", name, &p, &d, &c);
        if (ret <= 0 || ret == EOF) {
            continue; 
        }

        if (ret != 4) {
            char upper[256];
            for (int i = 0; line[i]; i++) upper[i] = (char)toupper((unsigned char)line[i]);
            upper[strlen(line)] = '\0';

            if (strstr(upper, "NOME") || strstr(upper, "NAME") || 
                strstr(upper, "PERIODO") || strstr(upper, "DEADLINE") || 
                strstr(upper, "BURST")) {
                continue; 
            }

            fprintf(stderr, "Erro: Campo faltando ou formato invalido na linha: \"%s\"\n", line);
            fclose(fp);
            return EXIT_FAILURE;
        }

        if (p <= 0 || d <= 0 || c <= 0) {
            fprintf(stderr, "Erro: Os valores de C, D e P devem ser inteiros positivos.\n");
            fclose(fp);
            return EXIT_FAILURE;
        }

        if (c > d || d > p) {
            fprintf(stderr, "Erro: Tarefa '%s' viola a condicao C <= D <= P (C=%d, D=%d, P=%d).\n", name, c, d, p);
            fclose(fp);
            return EXIT_FAILURE;
        }

        strcpy(tasks[num_tasks].name, name);
        tasks[num_tasks].p = p;
        tasks[num_tasks].d = d;
        tasks[num_tasks].c = c;
        tasks[num_tasks].remaining_c = 0;
        tasks[num_tasks].next_release = 0;
        tasks[num_tasks].absolute_deadline = 0;
        tasks[num_tasks].completed_count = 0;
        tasks[num_tasks].lost_count = 0;
        tasks[num_tasks].killed_count = 0;
        num_tasks++;
    }
    fclose(fp);

    if (num_tasks == 0) {
        fprintf(stderr, "Erro: Nenhuma tarefa encontrada no arquivo.\n");
        return EXIT_FAILURE;
    }

    char out_filename[128];
    snprintf(out_filename, sizeof(out_filename), "%s_%s.out", algoritmo, LOGIN);

    FILE *out = fopen(out_filename, "w");
    if (!out) {
        fprintf(stderr, "Erro ao criar o arquivo de saida '%s'.\n", out_filename);
        return EXIT_FAILURE;
    }

    if (strcmp(algoritmo, "rate") == 0) {
        fprintf(out, "EXECUTION BY RATE\n");
    } else {
        fprintf(out, "EXECUTION BY EDF\n");
    }

    int current_task = -1;
    int block_len = 0;

    for (int t = 0; t < sim_time; t++) {
        for (int i = 0; i < num_tasks; i++) {
            if (t == tasks[i].next_release) {
                tasks[i].remaining_c = tasks[i].c;
                tasks[i].absolute_deadline = t + tasks[i].d;
                tasks[i].next_release = t + tasks[i].p;
            }
        }

        int sel = selecionar_tarefa(tasks, num_tasks, algoritmo);

        if (sel != current_task && block_len > 0) {
            if (current_task == -1) {
                fprintf(out, "idle for %d units\n", block_len);
            } else {
                fprintf(out, "[%s] for %d units H\n", tasks[current_task].name, block_len);
            }
            block_len = 0;
        }

        current_task = sel;
        block_len++;

        char block_end_tag = '\0';
        if (current_task != -1) {
            tasks[current_task].remaining_c--;
            if (tasks[current_task].remaining_c == 0) {
                block_end_tag = 'F';
                tasks[current_task].completed_count++;
            }
        }

        for (int i = 0; i < num_tasks; i++) {
            if (tasks[i].remaining_c > 0 && (t + 1) == tasks[i].absolute_deadline) {
                tasks[i].lost_count++;
                tasks[i].remaining_c = 0;
                if (i == current_task) {
                    block_end_tag = 'L';
                }
            }
        }

        if (block_end_tag != '\0') {
            fprintf(out, "[%s] for %d units %c\n", tasks[current_task].name, block_len, block_end_tag);
            block_len = 0;
            current_task = -1;
        }
    }

    if (block_len > 0) {
        if (current_task == -1) {
            fprintf(out, "idle for %d units\n", block_len);
        } else {
            fprintf(out, "[%s] for %d units H\n", tasks[current_task].name, block_len);
        }
    }

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].remaining_c > 0) {
            tasks[i].killed_count++;
        }
    }

    fprintf(out, "\nLOST DEADLINES\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(out, "[%s] %d\n", tasks[i].name, tasks[i].lost_count);
    }

    fprintf(out, "\nCOMPLETE EXECUTION\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(out, "[%s] %d\n", tasks[i].name, tasks[i].completed_count);
    }

    fprintf(out, "\nKILLED\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(out, "[%s] %d\n", tasks[i].name, tasks[i].killed_count);
    }

    fclose(out);
    return EXIT_SUCCESS;
}