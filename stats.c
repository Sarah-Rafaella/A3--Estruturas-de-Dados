#include <stdio.h>
#include <string.h>
#include "stats.h"

/*
  Lê o arquivo stats.txt gerado pela simulação
  e escreve relatorio_final.txt contendo:
  - os dados do stats.txt
  - algoritmo de ordenação escolhido (passado como argumento)
  - estrutura de dados usada (passado como argumento)
  - complexidade média esperada (baseada no algoritmo)
*/
void gerarRelatorioFinal(const char *arquivoStats,
                         const char *algoritmoOrdenacao,
                         const char *estruturaUsada)
{
    FILE *in = fopen(arquivoStats, "r");
    if (!in) {
        printf("Aviso: nao foi possivel abrir %s (talvez a simulacao nao tenha gerado ainda).\n", arquivoStats);
        return;
    }

    FILE *out = fopen("relatorio_final.txt", "w");
    if (!out) {
        printf("Erro ao criar relatorio_final.txt\n");
        fclose(in);
        return;
    }

    fprintf(out, "===== RELATORIO FINAL DA SIMULACAO =====\n\n");

    fprintf(out, "== Estatisticas (extraido de %s) ==\n", arquivoStats);
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), in)) {
        fputs(buffer, out);
    }

    fprintf(out, "\n== Configuracoes utilizadas ==\n");
    fprintf(out, "Estrutura de dados: %s\n", estruturaUsada);
    fprintf(out, "Algoritmo de ordenacao: %s\n", algoritmoOrdenacao);

    fprintf(out, "\n== Complexidade (teorica) ==\n");
    if (strcmp(algoritmoOrdenacao, "Bubble Sort") == 0 ||
        strcmp(algoritmoOrdenacao, "Insertion Sort") == 0 ||
        strcmp(algoritmoOrdenacao, "Selection Sort") == 0) {
        fprintf(out, "Complexidade esperada: O(n^2)\n");
    } else if (strcmp(algoritmoOrdenacao, "Quick Sort") == 0 ||
               strcmp(algoritmoOrdenacao, "Merge Sort") == 0 ||
               strcmp(algoritmoOrdenacao, "Heap Sort") == 0) {
        fprintf(out, "Complexidade esperada: O(n log n)\n");
    } else {
        fprintf(out, "Complexidade esperada: (nao especificada)\n");
    }

    fprintf(out, "\nRelatorio gerado com sucesso: relatorio_final.txt\n");

    fclose(in);
    fclose(out);
}
