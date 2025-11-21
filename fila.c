#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==== STRUCT CLIENTE ==== */
typedef struct {
    int id;
    char nome[50];  // espaço para até 49 chars + '\0'
    int tipo;       // 0 comum, 1 preferencial, 2 corporativo
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

/* ==== CRIA UMA FILA VAZIA ==== */
void criarFila(Fila* f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

/* ==== ENFILEIRAR COM CHECAGEM DE ERRO NO MALLOC ==== */
void enfileirar(Fila* f, Cliente c) {
    No* novo = (No*)malloc(sizeof(No));

    if (!novo) {
        perror("Erro: falha ao alocar memoria para novo no");
        exit(1); // erro fatal
    }

    novo->cliente = c;
    novo->prox = NULL;

    if (f->fim == NULL) {  // fila vazia
        f->inicio = novo;
        f->fim = novo;
    } else {
        f->fim->prox = novo;
        f->fim = novo;
    }

    f->tamanho++;
}

/* ==== DESENFILEIRAR (RETORNA 1 SE SUCESSO, 0 SE FILA VAZIA) ==== */
int desenfileirar(Fila* f, Cliente* out) {
    if (f->inicio == NULL) {
        return 0; // fila vazia
    }

    No* temp = f->inicio;
    *out = temp->cliente;

    f->inicio = temp->prox;
    if (f->inicio == NULL)
        f->fim = NULL;

    free(temp);
    f->tamanho--;

    return 1;
}

/* ==== FUNÇÃO PARA ESVAZIAR A FILA (LIBERAR MEMÓRIA) ==== */
void limparFila(Fila* f) {
    Cliente temp;
    while (desenfileirar(f, &temp)) { }
}

/* ==== CARREGAR DADOS DE ARQUIVO COM SEGURANÇA ==== */
void carregarDados(char* nomeArquivo, Fila* comum, Fila* pref, Fila* corp) {
    FILE* arq = fopen(nomeArquivo, "r");

    if (!arq) {
        perror("Erro ao abrir arquivo");
        return;
    }

    Cliente c;

    while (fscanf(arq, "%d %49s %d %d %d",
                  &c.id, c.nome, &c.tipo,
                  &c.tempoAtendimento, &c.horarioChegada) == 5) {

        if (c.tipo == 0)
            enfileirar(comum, c);
        else if (c.tipo == 1)
            enfileirar(pref, c);
        else if (c.tipo == 2)
            enfileirar(corp, c);
        else
            printf("Aviso: cliente com tipo invalido (%d) ignorado.\n", c.tipo);
    }

    fclose(arq);
}