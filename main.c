#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ==== STRUCT CLIENTE ==== */
typedef struct {
    int id;
    char nome[50];
    int tipo;               // 0 = comum, 1 = preferencial, 2 = corporativo
    int tempoAtendimento;
    int horarioChegada;     // em minutos (ex: 08:30 = 510 min)
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

/* ==== CRIA FILA ==== */
void criarFila(Fila* f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

/* ==== ENFILEIRAR ==== */
void enfileirar(Fila* f, Cliente c) {
    No* novo = (No*)malloc(sizeof(No));
    if (!novo) {
        perror("Falha malloc");
        exit(1);
    }
    novo->cliente = c;
    novo->prox = NULL;

    if (f->fim == NULL) {
        f->inicio = f->fim = novo;
    } else {
        f->fim->prox = novo;
        f->fim = novo;
    }
    f->tamanho++;
}

/* ==== DESENFILEIRAR ==== */
int desenfileirar(Fila* f, Cliente* out) {
    if (f->inicio == NULL) return 0;

    No* temp = f->inicio;
    *out = temp->cliente;

    f->inicio = temp->prox;
    if (f->inicio == NULL)
        f->fim = NULL;

    free(temp);
    f->tamanho--;
    return 1;
}

/* ==== LIMPAR FILA ==== */
void limparFila(Fila* f) {
    Cliente tmp;
    while (desenfileirar(f, &tmp));
}



/* Converte HH:MM para minutos */
int hhmm_to_minutes(const char* hhmm) {
    int h, m;
    sscanf(hhmm, "%d:%d", &h, &m);
    return h * 60 + m;
}

/* Converte minutos para HH:MM */
void minutes_to_hhmm(int minutes, char* out) {
    int h = minutes / 60;
    int m = minutes % 60;
    sprintf(out, "%02d:%02d", h, m);
}


void carregarCSV(const char* filename, Fila* comum, Fila* pref, Fila* corp) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Erro ao abrir arquivo %s\n", filename);
        exit(1);
    }

    char line[256];
    fgets(line, sizeof(line), fp); // Ignorar cabeçalho

    while (fgets(line, sizeof(line), fp)) {

        Cliente c;
        char tipoStr[20];
        char chegadaStr[10];

        sscanf(line, "%d,%49[^,],%19[^,],%d,%9s",
               &c.id, c.nome, tipoStr,
               &c.tempoAtendimento, chegadaStr);

        c.horarioChegada = hhmm_to_minutes(chegadaStr);

        // Converter string para número
        if (strcmp(tipoStr, "common") == 0)
            c.tipo = 0;
        else if (strcmp(tipoStr, "preferential") == 0)
            c.tipo = 1;
        else
            c.tipo = 2;

        if (c.tipo == 0)
            enfileirar(comum, c);
        else if (c.tipo == 1)
            enfileirar(pref, c);
        else
            enfileirar(corp, c);
    }

    fclose(fp);
}

/* ============================
   SIMULAÇÃO DO ATENDIMENTO
   ============================ */

void simular(Fila* comum, Fila* pref, Fila* corp,
             const char* logfile, const char* statsfile)
{
    FILE* log = fopen(logfile, "w");
    fprintf(log, "id,name,type,arrival,start,end,wait,service\n");

    int current = hhmm_to_minutes("08:00");
    int total_wait = 0;
    int total_service = 0;
    int count = 0;

    while (comum->inicio || pref->inicio || corp->inicio) {
        Cliente c;
        int achou = 0;

        // Regras de prioridade
        if (corp->inicio && corp->inicio->cliente.horarioChegada <= current) {
            desenfileirar(corp, &c);
            achou = 1;
        }
        else if (pref->inicio && pref->inicio->cliente.horarioChegada <= current) {
            desenfileirar(pref, &c);
            achou = 1;
        }
        else if (comum->inicio && comum->inicio->cliente.horarioChegada <= current) {
            desenfileirar(comum, &c);
            achou = 1;
        }
        else {
            int next_arr = 1e9;
            if (comum->inicio && comum->inicio->cliente.horarioChegada < next_arr)
                next_arr = comum->inicio->cliente.horarioChegada;
            if (pref->inicio && pref->inicio->cliente.horarioChegada < next_arr)
                next_arr = pref->inicio->cliente.horarioChegada;
            if (corp->inicio && corp->inicio->cliente.horarioChegada < next_arr)
                next_arr = corp->inicio->cliente.horarioChegada;

            current = next_arr;
            continue;
        }

        int start = (current < c.horarioChegada) ? c.horarioChegada : current;
        int end = start + c.tempoAtendimento;
        int wait = start - c.horarioChegada;

        total_wait += wait;
        total_service += c.tempoAtendimento;
        count++;

        char a[10], s[10], e[10];
        minutes_to_hhmm(c.horarioChegada, a);
        minutes_to_hhmm(start, s);
        minutes_to_hhmm(end, e);

        char tipoStr[15];
        if (c.tipo == 0) strcpy(tipoStr, "common");
        else if (c.tipo == 1) strcpy(tipoStr, "preferential");
        else strcpy(tipoStr, "corporate");

        fprintf(log, "%d,%s,%s,%s,%s,%s,%d,%d\n",
                c.id, c.nome, tipoStr, a, s, e, wait, c.tempoAtendimento);

        current = end;
    }

    fclose(log);

    FILE* stats = fopen(statsfile, "w");
    fprintf(stats, "clients_attended: %d\n", count);
    fprintf(stats, "average_wait: %.2f\n", (count == 0 ? 0 : (float)total_wait / count));
    fprintf(stats, "total_service_minutes: %d\n", total_service);
    fprintf(stats, "structure_used: Filas encadeadas\n");
    fprintf(stats, "sorting_algorithm: (definir pela equipe)\n");
    fclose(stats);

    printf("Processamento concluído! Estatísticas em %s\n", statsfile);
}

/* ======================
         MAIN
   ====================== */

int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("Uso: programa <input.csv> <log.csv>\n");
        return 1;
    }

    Fila comum, pref, corp;
    criarFila(&comum);
    criarFila(&pref);
    criarFila(&corp);

    carregarCSV(argv[1], &comum, &pref, &corp);

    simular(&comum, &pref, &corp, argv[2], "stats.txt");

    limparFila(&comum);
    limparFila(&pref);
    limparFila(&corp);

    return 0;
}
