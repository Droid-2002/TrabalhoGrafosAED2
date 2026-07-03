/* ============================================================================
 * Trabalho Pratico 2 - SCC0503 (Algoritmos e Estruturas de Dados II) - ICMC/USP
 *
 * Mateus Cardoso Vargas Saracuza - NUSP 13674087
 * Rafael Mendonça Duarte – NUSP 16817608 
 * 
 * funcionalidades.c - Implementacao das funcionalidades [10], [11], [12] e [13].
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "funcionalidades.h"
#include "leitura.h"
#include "grafo.h"
#include "fornecidas.h"

/*
 * Separador usado entre TODOS os valores impressos em uma mesma linha.
 * Isolado em uma unica constante para permitir troca imediata
  */
#define SEP ", "

/* Mensagem de erro geral, exibida quando o grafo nao pode ser construido. */
#define MSG_FALHA "Falha na execução da funcionalidade.\n"

/* Mensagem exibida quando nao existe caminho entre origem e destino. */
#define MSG_SEM_CAMINHO "Não existe caminho entre as estações solicitadas.\n"

/* ===========================================================================
 * [10] Impressao do grafo
 * =========================================================================== */

void funcionalidade10(const char *arquivo) {
    Grafo *g;
    int n, i, j, k;

    g = construirGrafo(arquivo);
    if (!g) {
        printf(MSG_FALHA);
        return;
    }

    n = grafoGetNumVertices(g);
    for (i = 0; i < n; i++) {
        int numArestas;

        numArestas = grafoGetNumArestas(g, i);
        if (numArestas == 0) continue;

        /* nome do vertice (primeiro valor da linha) */
        printf("%s", grafoGetNomeVertice(g, i));

        /* lista de adjacencias, ja ordenada por destino e por linha */
        for (j = 0; j < numArestas; j++) {
            const Aresta *a = grafoGetAresta(g, i, j);
            int numLinhas = arestaGetNumLinhas(a);

            printf("%s%s", SEP, arestaGetDestino(a));      /* nome do destino  */
            printf("%s%d", SEP, arestaGetDistancia(a));    /* distancia        */
            for (k = 0; k < numLinhas; k++) {              /* nomes das linhas */
                printf("%s%s", SEP, arestaGetLinha(a, k));
            }
        }
        printf("\n");
    }

    grafoDestruir(g);
}

/* ===========================================================================
 * [11] Caminho mais curto
 * =========================================================================== */

/*
 * Executa o algoritmo de Dijkstra a partir de 'origem'. Preenche os vetores
 * 'dist' (distancia minima da origem a cada vertice) e 'pred' (predecessor de
 * cada vertice no caminho minimo, -1 se nao houver).
 */
static void dijkstra(const Grafo *g, int origem, int *dist, int *pred) {
    int n = grafoGetNumVertices(g);
    char *visitado;
    int it, i;

    /* inicializa antes de qualquer retorno para nunca deixar dist/pred sujos */
    for (i = 0; i < n; i++) {
        dist[i] = INT_MAX;   /* INT_MAX representa "infinito" */
        pred[i] = -1;
    }
    dist[origem] = 0;

    visitado = calloc(n, sizeof(char));
    if (!visitado) return;   /* sem memoria: dist permanece "infinito" */

    for (it = 0; it < n; it++) {
        int u = -1, numArestas, j;

        /* seleciona o vertice nao visitado de menor distancia; a ordem
         * crescente de indices garante o desempate por menor nome */
        for (i = 0; i < n; i++) {
            if (!visitado[i] && dist[i] != INT_MAX &&
                (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }
        if (u == -1) break;      /* vertices restantes sao inalcancaveis */
        visitado[u] = 1;

        /* relaxa as arestas de u (ja ordenadas por destino e linha) */
        numArestas = grafoGetNumArestas(g, u);
        for (j = 0; j < numArestas; j++) {
            const Aresta *a = grafoGetAresta(g, u, j);
            int v = grafoBuscarVertice(g, arestaGetDestino(a));
            long nova;

            if (v < 0 || visitado[v]) continue;
            nova = (long)dist[u] + arestaGetDistancia(a);

            if (nova < dist[v]) {
                dist[v] = (int)nova;
                pred[v] = u;
            } else if (nova == dist[v]) {
                /* mantem o predecessor de menor nome */
                if (pred[v] == -1 ||
                    strcmp(grafoGetNomeVertice(g, u),
                           grafoGetNomeVertice(g, pred[v])) < 0) {
                    pred[v] = u;
                }
            }
        }
    }

    free(visitado);
}

void funcionalidade11(const char *arquivo) {
    char campo[64];                 /* rotulo do campo (ex.: nomeEstacaoOrigem) */
    char origem[TAM_NOME];          /* valor da estacao de origem               */
    char destino[TAM_NOME];         /* valor da estacao de destino              */
    Grafo *g;
    int iOrigem, iDestino;

    /* leitura dos parametros: <rotulo> "valor" <rotulo> "valor" */
    if (scanf("%63s", campo) != 1) return;
    ScanQuoteString(origem);
    if (scanf("%63s", campo) != 1) return;
    ScanQuoteString(destino);

    g = construirGrafo(arquivo);
    if (!g) {
        printf(MSG_FALHA);
        return;
    }

    iOrigem  = grafoBuscarVertice(g, origem);
    iDestino = grafoBuscarVertice(g, destino);

    /* estacao inexistente no grafo: tratado como ausencia de caminho */
    if (iOrigem < 0 || iDestino < 0) {
        printf(MSG_SEM_CAMINHO);
        grafoDestruir(g);
        return;
    }

    /* origem e destino sao a mesma estacao: 0 estacoes, distancia 0 */
    if (iOrigem == iDestino) {
        printf("Numero de estacoes que serao percorridas: 0\n");
        printf("Distancia que sera percorrida: 0\n");
        printf("%s\n", origem);
        grafoDestruir(g);
        return;
    }

    {
        int n = grafoGetNumVertices(g);
        int *dist = malloc(sizeof(int) * n);
        int *pred = malloc(sizeof(int) * n);

        if (!dist || !pred) {       /* falha de alocacao */
            free(dist); free(pred);
            printf(MSG_FALHA);
            grafoDestruir(g);
            return;
        }

        dijkstra(g, iOrigem, dist, pred);

        if (dist[iDestino] == INT_MAX) {
            /* destino inalcancavel a partir da origem */
            printf(MSG_SEM_CAMINHO);
        } else {
            int *caminho = malloc(sizeof(int) * n);
            int cnt = 0, cur = iDestino, p;

            /* reconstroi o caminho do destino ate a origem via predecessores */
            while (cur != -1) {
                caminho[cnt++] = cur;
                if (cur == iOrigem) break;
                cur = pred[cur];
            }

            /* x = numero de estacoes sem contar a origem = (vertices - 1) */
            printf("Numero de estacoes que serao percorridas: %d\n", cnt - 1);
            printf("Distancia que sera percorrida: %d\n", dist[iDestino]);

            /* imprime o caminho na ordem origem -> ... -> destino */
            for (p = cnt - 1; p >= 0; p--) {
                printf("%s", grafoGetNomeVertice(g, caminho[p]));
                if (p > 0) printf("%s", SEP);
            }
            printf("\n");

            free(caminho);
        }

        free(dist);
        free(pred);
    }

    grafoDestruir(g);
}

/* ===========================================================================
 * [12] Arvore Geradora Minima (Prim) + busca em profundidade (DFS)
 *
 * A spec de [12] trata as linhas como de "ida e volta", logo o grafo e visto
 * como NAO-DIRECIONADO. Como o TAD so expoe as arestas de saida de cada
 * vertice e nao deve ser alterado, montamos aqui uma estrutura auxiliar
 * (baseada em indices) com as arestas nos dois sentidos.
 * =========================================================================== */

/* Aresta nao-direcionada, usada apenas na construcao da MST de [12].
 * 'a' e 'b' sao indices de vertices (a ordem alfabetica coincide com o
 * indice, pois o vetor de vertices do grafo e ordenado por nome). */
typedef struct {
    int a;   /* um extremo da aresta */
    int b;   /* o outro extremo      */
    int w;   /* peso (distancia)     */
} ArestaND;

/* Coleta em 'arestas' (previamente alocado com capacidade suficiente) todas
 * as arestas de saida do grafo como pares nao-direcionados (u, v, w). Cada
 * aresta u->v de peso w e registrada uma vez; para Prim tratamos qualquer
 * aresta como ligacao simetrica, entao nao e preciso duplicar o sentido.
 *
 * Uma estacao terminal (ultima de uma linha) so aparece como destino de uma
 * aresta e nao possui arestas de saida: sua lista de adjacencias fica vazia.
 * Assim como a funcionalidade [10] nao imprime vertices de lista vazia
 * (grafoGetNumArestas == 0), a arvore de [12] nao deve incluir esses
 * sumidouros puros. Por isso descartamos aqui qualquer aresta cujo DESTINO
 * nao tenha arestas de saida: como esse destino seria sempre uma folha (nunca
 * um vertice de passagem), remove-lo nao altera a MST entre as demais
 * estacoes, apenas evita que ele seja anexado como filho extra.
 * Retorna a quantidade de arestas efetivamente coletadas. */
static int coletarArestasND(const Grafo *g, int n, ArestaND *arestas) {
    int u, j, m = 0;

    for (u = 0; u < n; u++) {
        int numArestas = grafoGetNumArestas(g, u);
        for (j = 0; j < numArestas; j++) {
            const Aresta *a = grafoGetAresta(g, u, j);
            int v = grafoBuscarVertice(g, arestaGetDestino(a));
            if (v < 0) continue;                     /* destino inexistente: ignora */
            if (grafoGetNumArestas(g, v) == 0) continue; /* destino e sumidouro puro: ignora */

            arestas[m].a = u;
            arestas[m].b = v;
            arestas[m].w = arestaGetDistancia(a);
            m++;
        }
    }
    return m;
}

/*
 * Constroi a Arvore Geradora Minima pelo algoritmo de Prim, partindo de
 * 'origem'. Preenche 'parent' (vizinho da MST pelo qual cada vertice entrou
 * na arvore; -1 para a origem e para vertices fora do componente da origem)
 * e 'pesoPai' (peso da aresta ate o pai).
 *
 * A cada passo escolhe-se a aresta de fronteira (um extremo ja na arvore, o
 * outro fora) de menor peso; empate -> menor indice do extremo interno 'u';
 * novo empate -> menor indice do extremo externo 'v'. Como o vetor de
 * vertices e ordenado por nome, "menor indice" equivale a "menor nome".
 *
 * Partir da origem cobre naturalmente grafos desconexos (obtem-se a MST do
 * componente da origem) e equivale a MST global quando o grafo e conexo.
 * Retorna 0 em sucesso, -1 em falha de alocacao.
 */
static int primMST(int n, int origem, const ArestaND *arestas, int m,
                   int *parent, int *pesoPai) {
    char *naArvore;   /* naArvore[i] = 1 se o vertice i ja pertence a MST */
    int i, count;

    for (i = 0; i < n; i++) {
        parent[i]  = -1;
        pesoPai[i] = 0;
    }

    naArvore = calloc(n, sizeof(char));
    if (!naArvore) return -1;

    naArvore[origem] = 1;
    count = 1;                       /* a origem ja esta na arvore */

    while (count < n) {
        int bestW = 0, bestU = -1, bestV = -1, achou = 0;
        int e;

        /* varre todas as arestas em busca da melhor de fronteira */
        for (e = 0; e < m; e++) {
            int ea = arestas[e].a, eb = arestas[e].b, ew = arestas[e].w;
            int u, v;

            /* fronteira: exatamente um extremo esta na arvore */
            if (naArvore[ea] == naArvore[eb]) continue;
            if (naArvore[ea]) { u = ea; v = eb; }   /* u interno, v externo */
            else              { u = eb; v = ea; }

            if (!achou ||
                ew < bestW ||
                (ew == bestW && u < bestU) ||
                (ew == bestW && u == bestU && v < bestV)) {
                bestW = ew; bestU = u; bestV = v; achou = 1;
            }
        }

        if (!achou) break;   /* sem fronteira: componente da origem esgotado */

        naArvore[bestV] = 1;
        parent[bestV]   = bestU;
        pesoPai[bestV]  = bestW;
        count++;
    }

    free(naArvore);
    return 0;
}

/*
 * Percorre a MST em profundidade (pre-ordem) a partir de 'u', imprimindo uma
 * linha "atual, filho, distancia" para cada aresta arvore->filho. Os filhos
 * de 'u' sao os vertices c com parent[c] == u; percorre-los em ordem
 * crescente de indice equivale a ordem crescente de nome. Como 'parent'
 * define uma arvore enraizada na origem, nao ha necessidade de vetor de
 * visitados (cada vertice tem um unico pai).
 */
static void dfsMST(const Grafo *g, int u, const int *parent,
                   const int *pesoPai, int n) {
    int c;

    for (c = 0; c < n; c++) {
        if (parent[c] != u) continue;    /* c nao e filho de u */

        printf("%s%s%s%s%d\n",
               grafoGetNomeVertice(g, u), SEP,
               grafoGetNomeVertice(g, c), SEP,
               pesoPai[c]);
        dfsMST(g, c, parent, pesoPai, n);
    }
}

void funcionalidade12(const char *arquivo) {
    char campo[64];            /* rotulo do campo (ex.: nomeEstacaoOrigem) */
    char origem[TAM_NOME];     /* valor da estacao de origem               */
    Grafo *g;
    int iOrigem, n, m, u;
    ArestaND *arestas;
    int *parent, *pesoPai;

    /* leitura dos parametros: <rotulo> "valor" */
    if (scanf("%63s", campo) != 1) return;
    ScanQuoteString(origem);

    g = construirGrafo(arquivo);
    if (!g) {
        printf(MSG_FALHA);
        return;
    }

    iOrigem = grafoBuscarVertice(g, origem);
    if (iOrigem < 0) {                 /* origem inexistente no grafo */
        printf(MSG_FALHA);
        grafoDestruir(g);
        return;
    }

    n = grafoGetNumVertices(g);

    /* capacidade do vetor de arestas = total de arestas de saida do grafo */
    m = 0;
    for (u = 0; u < n; u++) m += grafoGetNumArestas(g, u);

    arestas = malloc(sizeof(ArestaND) * (m > 0 ? m : 1));
    parent  = malloc(sizeof(int) * (n > 0 ? n : 1));
    pesoPai = malloc(sizeof(int) * (n > 0 ? n : 1));
    if (!arestas || !parent || !pesoPai) {
        free(arestas); free(parent); free(pesoPai);
        printf(MSG_FALHA);
        grafoDestruir(g);
        return;
    }

    m = coletarArestasND(g, n, arestas);

    if (primMST(n, iOrigem, arestas, m, parent, pesoPai) != 0) {
        free(arestas); free(parent); free(pesoPai);
        printf(MSG_FALHA);
        grafoDestruir(g);
        return;
    }

    /* imprime a MST em profundidade a partir da origem */
    dfsMST(g, iOrigem, parent, pesoPai, n);

    free(arestas);
    free(parent);
    free(pesoPai);
    grafoDestruir(g);
}

/* ===========================================================================
 * [13] Contagem de ciclos simples a partir da origem
 *
 * Diferentemente de [12], a spec de [13] nao menciona ida e volta: o grafo e
 * usado como DIRECIONADO. Um ciclo simples parte da origem, retorna a ela e
 * nao repete vertices (exceto a origem no inicio/fim). Como o ciclo e
 * definido pela sequencia de vertices, arestas paralelas para o mesmo destino
 * (mesma dupla origem->destino com distancias/linhas diferentes) contam uma
 * unica vez -> deduplicamos os sucessores por vertice de destino.
 * =========================================================================== */

/*
 * Preenche 'suc' com os indices distintos dos vertices de destino
 * alcancaveis por uma aresta de saida de 'u', deduplicando arestas paralelas.
 * 'marca' e um buffer auxiliar de n chars que deve estar zerado na entrada;
 * a funcao o restaura antes de retornar. Retorna a quantidade de sucessores
 * distintos escritos em 'suc'.
 */
static int sucessoresDistintos(const Grafo *g, int u, int n,
                               int *suc, char *marca) {
    int numArestas = grafoGetNumArestas(g, u);
    int j, cnt = 0;
    (void)n;

    for (j = 0; j < numArestas; j++) {
        const Aresta *a = grafoGetAresta(g, u, j);
        int w = grafoBuscarVertice(g, arestaGetDestino(a));
        if (w < 0 || marca[w]) continue;   /* destino invalido ou ja contado */
        marca[w] = 1;
        suc[cnt++] = w;
    }

    for (j = 0; j < cnt; j++) marca[suc[j]] = 0;   /* restaura o buffer */
    return cnt;
}

/*
 * DFS com backtracking que conta, em '*contador', os ciclos simples que
 * fecham na 'origem'. Para cada sucessor distinto w de u:
 *   - w == u  : self-loop, ignorado (nao forma ciclo simples);
 *   - w == origem : fechou um ciclo -> incrementa o contador;
 *   - senao, se w ainda nao visitado : marca, aprofunda e desmarca.
 */
static void dfsCiclos(const Grafo *g, int u, int origem, int n,
                      char *visitado, char *marca, long *contador) {
    int *suc = malloc(sizeof(int) * (n > 0 ? n : 1));
    int k, i;

    if (!suc) return;
    k = sucessoresDistintos(g, u, n, suc, marca);

    for (i = 0; i < k; i++) {
        int w = suc[i];

        if (w == u) continue;                       /* ignora self-loop */
        if (w == origem) { (*contador)++; continue; }  /* fechou um ciclo */

        if (!visitado[w]) {
            visitado[w] = 1;
            dfsCiclos(g, w, origem, n, visitado, marca, contador);
            visitado[w] = 0;                        /* backtracking */
        }
    }

    free(suc);
}

void funcionalidade13(const char *arquivo) {
    char campo[64];            /* rotulo do campo (ex.: nomeEstacaoOrigem) */
    char origem[TAM_NOME];     /* valor da estacao de origem               */
    Grafo *g;
    int iOrigem, n;
    char *visitado, *marca;
    long contador = 0;

    /* leitura dos parametros: <rotulo> "valor" */
    if (scanf("%63s", campo) != 1) return;
    ScanQuoteString(origem);

    g = construirGrafo(arquivo);
    if (!g) {
        printf(MSG_FALHA);
        return;
    }

    iOrigem = grafoBuscarVertice(g, origem);
    if (iOrigem < 0) {                 /* origem inexistente no grafo */
        printf(MSG_FALHA);
        grafoDestruir(g);
        return;
    }

    n = grafoGetNumVertices(g);
    visitado = calloc(n, sizeof(char));   /* vertices no caminho atual */
    marca    = calloc(n, sizeof(char));   /* buffer para deduplicar sucessores */
    if (!visitado || !marca) {
        free(visitado); free(marca);
        printf(MSG_FALHA);
        grafoDestruir(g);
        return;
    }

    visitado[iOrigem] = 1;
    dfsCiclos(g, iOrigem, iOrigem, n, visitado, marca, &contador);

    /* sem ciclo (nao ha como retornar a origem): imprime -1 */
    if (contador == 0)
        printf("Quantidade de ciclos: -1\n");
    else
        printf("Quantidade de ciclos: %ld\n", contador);

    free(visitado);
    free(marca);
    grafoDestruir(g);
}
