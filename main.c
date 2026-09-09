#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[32];           
    int c;                   
    int p;                   
    int d;                   
    
    int remaining_c;        
    int next_release;       
    int absolute_deadline;   
} Task;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso incorreto. Sintaxe: %s <RM|EDF> <arquivo_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *algoritmo = argv[1];
    char *arquivo_entrada = argv[2];

    if (strcmp(algoritmo, "RM") != 0 && strcmp(algoritmo, "EDF") != 0) {
        fprintf(stderr, "Erro: Algoritmo '%s' invalido. Escolha 'RM' ou 'EDF'.\n", algoritmo);
        return EXIT_FAILURE;
    }

    printf("Algoritmo selecionado: %s\n", algoritmo);
    printf("Arquivo de entrada: %s\n", arquivo_entrada);
    printf("Estrutura Task inicializada com sucesso (tamanho: %lu bytes).\n", sizeof(Task));

    return EXIT_SUCCESS;
}