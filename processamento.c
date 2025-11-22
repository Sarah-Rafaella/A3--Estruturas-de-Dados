#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fila.h"   


// Converte HH:MM para minutos totais
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


// Processa o atendimento dos clientes usando as três filas
void processarAtendimento(Fila* comum, Fila* pref, Fila* corp,
                          const char* nomeSaida,
                          const char* nomeStats) 
{
    FILE* out = fopen(nomeSaida, "w");
    if (!out) {
        printf("Erro ao criar arquivo de saída!\n");
        return;
    }

    fprintf(out, "id,nome,tipo,chegada,inicio,fim,espera,servico\n");

    int tempoAtual = hhmm_to_minutes("08:00");

    int totalEspera = 0;
    int totalServico = 0;
    int atendidos = 0;

    Cliente c;

    // Enquanto pelo menos 1 fila ainda tiver clientes
    while (comum->tamanho > 0 || pref->tamanho > 0 || corp->tamanho > 0)
    {
        int temCliente = 0;
        
        if (corp->tamanho > 0) {
            if (desenfileirar(corp, &c))
                temCliente = 1;
        }
        else if (pref->tamanho > 0) {
            if (desenfileirar(pref, &c))
                temCliente = 1;
        }
        else if (comum->tamanho > 0) {
            if (desenfileirar(comum, &c))
                temCliente = 1;
        }

        if (!temCliente)
            break;  

        // Ajustar início do atendimento
        int inicio = (tempoAtual < c.horarioChegada)
                     ? c.horarioChegada
                     : tempoAtual;

        int fim = inicio + c.tempoAtendimento;
        int espera = inicio - c.horarioChegada;

        totalEspera += espera;
        totalServico += c.tempoAtendimento;
        atendidos++;

        char chegada[10], s[10], e[10];
        minutes_to_hhmm(c.horarioChegada, chegada);
        minutes_to_hhmm(inicio, s);
        minutes_to_hhmm(fim, e);

        fprintf(out, "%d,%s,%d,%s,%s,%s,%d,%d\n",
                c.id, c.nome, c.tipo,
                chegada, s, e,
                espera, c.tempoAtendimento);

        tempoAtual = fim;
    }

    fclose(out);

    FILE* stats = fopen(nomeStats, "w");
    if (!stats) {
        printf("Erro ao criar arquivo de estatísticas!\n");
        return;
    }

    fprintf(stats, "clientes_atendidos: %d\n", atendidos);

    if (atendidos > 0)
        fprintf(stats, "media_espera: %.2f\n", (float)totalEspera / atendidos);
    else
        fprintf(stats, "media_espera: 0\n");

    fprintf(stats, "tempo_total_servico: %d\n", totalServico);
    fprintf(stats, "estrutura_usada: filas encadeadas\n");
    fprintf(stats, "ordenacao_usada: (definida pela equipe)\n");

    fclose(stats);

    printf("\nProcessamento concluido com sucesso.\n");
}
