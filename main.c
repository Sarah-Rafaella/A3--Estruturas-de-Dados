#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stats.h"

typedef struct {
    int id;
    int tempoAtendimento;
} Cliente;

typedef struct No {
    Cliente dado;
    struct No *prox;
} No;

typedef struct {
    No *inicio;
    No *fim;
    int tamanho;
} Fila;

typedef struct {
    float tempoMedioEspera;
    int tempoTotalAtendimento;
    char estrutura[50];
    char algoritmo[50];
    char complexidade[20];
} Estatisticas;

void inicializarFila(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

void enfileirar(Fila *f, Cliente c) {
    No *novo = (No *) malloc(sizeof(No));
    novo->dado = c;
    novo->prox = NULL;

    if (f->fim == NULL)
        f->inicio = novo;
    else
        f->fim->prox = novo;

    f->fim = novo;
    f->tamanho++;
}

int filaVazia(Fila *f) {
    return f->inicio == NULL;
}

Cliente desenfileirar(Fila *f) {
    Cliente c = { -1, -1 };
    if (filaVazia(f)) return c;

    No *temp = f->inicio;
    c = temp->dado;
    f->inicio = temp->prox;
    if (f->inicio == NULL)
        f->fim = NULL;

    free(temp);
    f->tamanho--;
    return c;
}

void mostrarFila(Fila *f) {
    if (filaVazia(f)) {
        printf("\nFila vazia!\n");
        return;
    }

    printf("\n--- FILA ATUAL ---\n");
    No *aux = f->inicio;
    while (aux != NULL) {
        printf("Cliente %d | Atendimento: %d min\n", aux->dado.id, aux->dado.tempoAtendimento);
        aux = aux->prox;
    }
}


void cadastrarClientes(Fila *f) {
    int qtd;
    printf("\nQuantos clientes deseja cadastrar? ");
    scanf("%d", &qtd);

    for (int i = 0; i < qtd; i++) {
        Cliente c;
        c.id = f->tamanho + 1;
        printf("Tempo de atendimento do cliente %d: ", c.id);
        scanf("%d", &c.tempoAtendimento);
        enfileirar(f, c);
    }
    printf("\nClientes cadastrados com sucesso!\n");
}

Estatisticas rodarSimulacao(Fila *f) {
    Estatisticas e;
    int tempoTotal = 0, somaEspera = 0, esperaAtual = 0, contador = 0;
    Fila copia = *f;

    while (!filaVazia(&copia)) {
        Cliente c = desenfileirar(&copia);
        somaEspera += esperaAtual;
        esperaAtual += c.tempoAtendimento;
        tempoTotal += c.tempoAtendimento;
        contador++;
    }

    e.tempoMedioEspera = (contador == 0 ? 0 : (float)somaEspera / contador);
    e.tempoTotalAtendimento = tempoTotal;
    strcpy(e.estrutura, "Fila Encadeada");
    strcpy(e.algoritmo, "Nenhum (processamento sequencial)");
    strcpy(e.complexidade, "O(n)");

    return e;
}

void exportarRelatorio(Estatisticas e) {
    FILE *arq = fopen("relatorio.txt", "w");
    if (!arq) {
        printf("Erro ao gerar arquivo!\n");
        return;
    }

    fprintf(arq, "RELATORIO FINAL\n\n");
    fprintf(arq, "Tempo medio de espera: %.2f minutos\n", e.tempoMedioEspera);
    fprintf(arq, "Tempo total de atendimento: %d minutos\n", e.tempoTotalAtendimento);
    fprintf(arq, "Estrutura utilizada: %s\n", e.estrutura);
    fprintf(arq, "Algoritmo de ordenacao: %s\n", e.algoritmo);
    fprintf(arq, "Complexidade media: %s\n", e.complexidade);

    fclose(arq);
    printf("\nRelatório (relatorio.txt) gerado com sucesso!\n");

    gerarRelatorioFinal("stats.txt", e.algoritmo, e.estrutura);
}

int main() {
    Fila fila;
    inicializarFila(&fila);

    Estatisticas stats;
    int opc;

    do {
        printf("\n=========== MENU PRINCIPAL ===========\n");
        printf("1. Cadastrar clientes\n");
        printf("2. Mostrar filas\n");
        printf("3. Rodar simulacao\n");
        printf("4. Exportar relatorio\n");
        printf("5. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);

        switch (opc) {
            case 1: cadastrarClientes(&fila); break;
            case 2: mostrarFila(&fila); break;
            case 3:
                stats = rodarSimulacao(&fila);
                printf("\nSimulacao concluida!\n");
                break;
            case 4:
                exportarRelatorio(stats);
                break;
            case 5:
                printf("\nEncerrando...\n");
                break;
            default:
                printf("\nOpcao invalida!\n");
        }
    } while (opc != 5);

    return 0;
}
