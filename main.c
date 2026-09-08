#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    printf("Algoritmo: %s | Arquivo: %s\n", algoritmo, arquivo_entrada);

    return EXIT_SUCCESS;
}