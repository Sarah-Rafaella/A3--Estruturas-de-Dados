#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmos_ordenacao.h"
#include "main.c"

/*Converter FILA → vetor*/
Cliente* fila_para_vetor(Fila* f, int* tamanho) {
    *tamanho = f->tamanho;
    Cliente* vet = malloc(sizeof(Cliente) * (*tamanho));

    No* atual = f->inicio;
    int i = 0;

    while (atual != NULL) {
        vet[i++] = atual->cliente;
        atual = atual->prox;
    }

    return vet;
}

/*Converter vetor → FILA*/
void vetor_para_fila(Fila* f, Cliente* vet, int n) {
    limparFila(f);
    for (int i = 0; i < n; i++) {
        enfileirar(f, vet[i]);
    }
}

/*BUBBLE SORT*/
EstatisticaOrdenacao bubble_sort(Cliente* vet, int n) {
    EstatisticaOrdenacao est = {0, 0};

    clock_t inicio = clock();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            est.comparacoes++;
            if (vet[j].horarioChegada > vet[j + 1].horarioChegada) {
                Cliente tmp = vet[j];
                vet[j] = vet[j + 1];
                vet[j + 1] = tmp;
            }
        }
    }

    clock_t fim = clock();

    est.tempoExecucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

    return est;
}

/*QUICK SORT (recursivo)*/
int particionar(Cliente* vet, int inicio, int fim, EstatisticaOrdenacao* est) {
    Cliente pivo = vet[fim];
    int i = inicio - 1;

    for (int j = inicio; j < fim; j++) {
        est->comparacoes++;
        if (vet[j].horarioChegada <= pivo.horarioChegada) {
            i++;
            Cliente temp = vet[i];
            vet[i] = vet[j];
            vet[j] = temp;
        }
    }

    Cliente temp = vet[i + 1];
    vet[i + 1] = vet[fim];
    vet[fim] = temp;

    return i + 1;
}

void quick_sort_rec(Cliente* vet, int inicio, int fim, EstatisticaOrdenacao* est) {
    if (inicio < fim) {
        int p = particionar(vet, inicio, fim, est);
        quick_sort_rec(vet, inicio, p - 1, est);
        quick_sort_rec(vet, p + 1, fim, est);
    }
}

EstatisticaOrdenacao quick_sort(Cliente* vet, int inicio, int fim) {
    EstatisticaOrdenacao est = {0, 0};

    clock_t tempoInicio = clock();

    quick_sort_rec(vet, inicio, fim, &est);

    clock_t tempoFim = clock();
    est.tempoExecucao = (double)(tempoFim - tempoInicio) / CLOCKS_PER_SEC;

    return est;
}

/*Função geral de ordenação*/
EstatisticaOrdenacao ordenar_fila(Fila* f, int algoritmo) {
    int n;
    Cliente* vet = fila_para_vetor(f, &n);
    EstatisticaOrdenacao est;

    if (algoritmo == 1) {
        est = bubble_sort(vet, n);
    }
    else if (algoritmo == 2) {
        est = quick_sort(vet, 0, n - 1);
    }
    else {
        printf("Algoritmo invalido!\n");
        exit(1);
    }

    vetor_para_fila(f, vet, n);
    free(vet);

    return est;
}

