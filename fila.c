#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==== STRUCT CLIENTE ==== */
typedef struct {
    int id;
    char nome[50];
    int tipo; // 0 comum, 1 preferencial, 2 corporativo
    int tempoAtendimento;
    int horarioChegada;
} Cliente;

/* ==== NÓ DA FILA ==== */
typedef struct No {
    Cliente cliente;
    struct No* prox;
} No;

/* ==== FILA ==== */
typedef struct {
    No* inicio;
    No* fim;
    int tamanho;
} Fila;

/* ==== FUNÇÕES DA FILA ==== */

void criarFila(Fila* f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

void enfileirar(Fila* f, Cliente c) {
    No* novo = (No*)malloc(sizeof(No));
    novo->cliente = c;
    novo->prox = NULL;

    if (f->fim == NULL) {
        f->inicio = novo;
        f->fim = novo;
    } else {
        f->fim->prox = novo;
        f->fim = novo;
    }

    f->tamanho++;
}

Cliente desenfileirar(Fila* f) {
    if (f->inicio == NULL) {
        Cliente vazio = {0};
        return vazio;
    }

    No* temp = f->inicio;
    Cliente c = temp->cliente;
    f->inicio = temp->prox;

    if (f->inicio == NULL)
        f->fim = NULL;

    free(temp);
    f->tamanho--;

    return c;
}

/* ==== CARREGAR DADOS DO ARQUIVO ==== */

void carregarDados(char* nomeArquivo, Fila* comum, Fila* pref, Fila* corp) {
    FILE* arq = fopen(nomeArquivo, "r");
    if (!arq) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    Cliente c;
    while (fscanf(arq, "%d %s %d %d %d",
                  &c.id, c.nome, &c.tipo,
                  &c.tempoAtendimento, &c.horarioChegada) != EOF) {

        if (c.tipo == 0)
            enfileirar(comum, c);
        else if (c.tipo == 1)
            enfileirar(pref, c);
        else if (c.tipo == 2)
            enfileirar(corp, c);
    }

    fclose(arq);
}
