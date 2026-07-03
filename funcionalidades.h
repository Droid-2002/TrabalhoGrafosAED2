#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

/*
 * funcionalidades.h
 *
 * Declara as funcionalidades do programa que operam sobre o grafo de
 * estacoes construido a partir do arquivo binario de entrada.
 *
 * Cada funcionalidade constroi o grafo internamente (via construirGrafo) e
 * escreve o resultado na saida padrao, seguindo estritamente o formato
 * exigido pela especificacao.
 */

/*
 * [10] Recupera os dados do arquivo binario, gera o grafo e o imprime na
 * saida padrao (uma linha por vertice, com sua lista de adjacencias).
 *   arquivo: nome do arquivo binario de entrada.
 */
void funcionalidade10(const char *arquivo);

/*
 * [11] Determina o caminho mais curto (algoritmo de Dijkstra) entre a
 * estacao de origem e a de destino, lidas da entrada padrao (rotulo do
 * campo seguido do valor entre aspas). Imprime o numero de estacoes, a
 * distancia total e o caminho, ou a mensagem de ausencia de caminho.
 *   arquivo: nome do arquivo binario de entrada.
 */
void funcionalidade11(const char *arquivo);

/*
 * [12] Constroi a Arvore Geradora Minima (Prim) do grafo tratado como
 * nao-direcionado, a partir da estacao de origem lida da entrada padrao
 * (rotulo do campo seguido do valor entre aspas). Percorre a arvore em
 * profundidade a partir da origem, imprimindo uma linha "atual, filho,
 * distancia" por aresta. Em caso de origem inexistente ou falha, imprime
 * a mensagem de erro.
 *   arquivo: nome do arquivo binario de entrada.
 */
void funcionalidade12(const char *arquivo);

/*
 * [13] Conta os ciclos simples que partem da estacao de origem (lida da
 * entrada padrao) e a ela retornam sem repetir vertices, no grafo tratado
 * como direcionado. Imprime "Quantidade de ciclos: n" (ou -1 quando nao ha
 * ciclo). Em caso de origem inexistente ou falha, imprime a mensagem de erro.
 *   arquivo: nome do arquivo binario de entrada.
 */
void funcionalidade13(const char *arquivo);

#endif /* FUNCIONALIDADES_H */
