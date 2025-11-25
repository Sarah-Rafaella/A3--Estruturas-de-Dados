#ifndef ALGORITMOS_ORDENACAO_H
#define ALGORITMOS_ORDENACAO_H
 
#include <time.h>

/* Estrutura usada para benchmark */
typedef struct {
    double tempoExecucao;     // tempo em segundos
    int comparacoes;          // número de comparações feitas
} EstatisticaOrdenacao;

/* Funções expostas */

/* Converte a FILA encadeada em um vetor */
Cliente* fila_para_vetor(Fila* f, int* tamanho);

/* Reconstrói a fila a partir de um vetor */
void vetor_para_fila(Fila* f, Cliente* vet, int n);

/* Bubble Sort */
EstatisticaOrdenacao bubble_sort(Cliente* vet, int n);

/* Quick Sort */
EstatisticaOrdenacao quick_sort(Cliente* vet, int inicio, int fim);

/* Função geral que chama o algoritmo escolhido */
EstatisticaOrdenacao ordenar_fila(Fila* f, int algoritmo);
/* algoritmo = 1 → Bubble Sort */
/* algoritmo = 2 → Quick Sort */

#endif

