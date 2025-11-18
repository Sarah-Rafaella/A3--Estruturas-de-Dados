#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// Converte HH:MM para minutos
int hhmm_to_minutes(const char *hhmm) {
    int h, m;
    sscanf(hhmm, "%d:%d", &h, &m);
    return h * 60 + m;
}

// Converte minutos para HH:MM
void minutes_to_hhmm(int minutes, char *out) {
    int h = minutes / 60;
    int m = minutes % 60;
    sprintf(out, "%02d:%02d", h, m);
}

// - ESTRUTURA DE CLIENTE ----
typedef struct Client {
    int id;
    char name[50];
    char type[20];
    int arrival;      
    int service;      
    struct Client *next;
} Client;

// ---- FILA SIMPLES ----

typedef struct Queue {
    Client *front;
    Client *rear;
} Queue;

void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

int is_empty(Queue *q) {
    return q->front == NULL;
}

void enqueue(Queue *q, Client *c) {
    c->next = NULL;
    if (q->rear == NULL)
        q->front = q->rear = c;
    else {
        q->rear->next = c;
        q->rear = c;
    }
}

Client *dequeue(Queue *q) {
    if (is_empty(q)) return NULL;
    Client *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    return temp;
}

// -Leitura do arquivo

void load_clients(const char *filename, Queue *common, Queue *pref, Queue *corp) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Erro ao abrir input: %s\n", filename);
        exit(1);
    }

    char line[256];
    fgets(line, sizeof(line), fp); // ignorar cabeçalho

    while (fgets(line, sizeof(line), fp)) {
        Client *c = malloc(sizeof(Client));
        char arrival_str[10];

        sscanf(line, "%d,%49[^,],%19[^,],%d,%9s",
               &c->id, c->name, c->type, &c->service, arrival_str);

        c->arrival = hhmm_to_minutes(arrival_str);
        c->next = NULL;

        if (strcmp(c->type, "common") == 0)
            enqueue(common, c);
        else if (strcmp(c->type, "preferential") == 0)
            enqueue(pref, c);
        else
            enqueue(corp, c);
    }

    fclose(fp);
}

// -- PROCESSAMENTO --

void simulate(Queue *common, Queue *pref, Queue *corp,
              const char *logfile, const char *statsfile) {

    FILE *log = fopen(logfile, "w");
    fprintf(log, "id,name,type,arrival,start,end,wait,service\n");

    int current = hhmm_to_minutes("08:00");
    int total_wait = 0;
    int total_service = 0;
    int count = 0;

    while (!is_empty(common) || !is_empty(pref) || !is_empty(corp)) {

        Client *c = NULL;

      
        if (!is_empty(corp) && corp->front->arrival <= current)
            c = dequeue(corp);
        else if (!is_empty(pref) && pref->front->arrival <= current)
            c = dequeue(pref);
        else if (!is_empty(common) && common->front->arrival <= current)
            c = dequeue(common);
        else {
            
            int next_arr = 1e9;
            if (!is_empty(common)) next_arr = common->front->arrival;
            if (!is_empty(pref) && pref->front->arrival < next_arr) next_arr = pref->front->arrival;
            if (!is_empty(corp) && corp->front->arrival < next_arr) next_arr = corp->front->arrival;

            current = next_arr;
            continue;
        }

        int start = (current < c->arrival) ? c->arrival : current;
        int end = start + c->service;
        int wait = start - c->arrival;

        total_wait += wait;
        total_service += c->service;
        count++;

        char a[10], s[10], e[10];
        minutes_to_hhmm(c->arrival, a);
        minutes_to_hhmm(start, s);
        minutes_to_hhmm(end, e);

        fprintf(log, "%d,%s,%s,%s,%s,%s,%d,%d\n",
                c->id, c->name, c->type, a, s, e, wait, c->service);

        current = end;

        free(c);
    }

    fclose(log);

    FILE *stats = fopen(statsfile, "w");
    fprintf(stats, "clients_attended: %d\n", count);
    fprintf(stats, "average_wait: %.2f\n", (count == 0 ? 0 : (float)total_wait / count));
    fprintf(stats, "total_service_minutes: %d\n", total_service);
    fprintf(stats, "structure_used: Filas simples encadeadas\n");
    fprintf(stats, "sorting_algorithm: (definir pela equipe)\n");
    fclose(stats);

    printf("Processamento concluído! Estatísticas salvas em %s\n", statsfile);
}

// -MAIN-

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso correto: sim_person2 <input.csv> <output.csv>\n");
        return 1;
    }

    Queue common, pref, corp;
    init_queue(&common);
    init_queue(&pref);
    init_queue(&corp);

    load_clients(argv[1], &common, &pref, &corp); 

    simulate(&common, &pref, &corp, argv[2], "stats.txt"); 
    return 0;
}

