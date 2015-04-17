#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cgraph.h>
#include "grafo.h"

//------------------------------------------------------------------------------
typedef struct grafo {
  int grafo_direcionado;
  char *grafo_nome;
  double *grafo_matriz;
  vertice grafo_vertices;
  unsigned int grafo_n_vertices;
} *grafo;

//------------------------------------------------------------------------------
typedef struct vertice {
  char *vertice_nome;
} *vertice;

//------------------------------------------------------------------------------
const long int infinito = LONG_MAX;

//------------------------------------------------------------------------------
int encontra_vertice(vertice vertices, unsigned int n_vertices, const char *nome) {
  unsigned int i;

  /* Percorre todos os vértices da estrutura */
  for(i = 0; i < n_vertices; ++i) {
    /* Se o nome do vértice é igual ao desejado, então o retorna */
    if(strcmp(vertices[i].vertice_nome, nome) == 0) {
      return i;
    }
  }

  return -1;
}

//------------------------------------------------------------------------------
vertice obter_vertices(Agraph_t *grafo, unsigned int *n_vertices) {
  Agnode_t *v;
  vertice vertices = NULL;
  int i;

  /* Número de vértices do grafo */
  *n_vertices = agnnodes(grafo);

  if(*n_vertices > 0) {
    /* Aloca a quantidade de memória necessária para armazenar todos os vértices */
    vertices = (vertice) malloc(sizeof(struct vertice) * (*n_vertices));

    if(vertices != NULL) {
      /* Percorre todos os vértices do grafo */
      for(i = 0, v = agfstnode(grafo); i < *n_vertices; ++i, v = agnxtnode(grafo, v)) {
        /* Duplica na memória o nome do vértice e o atribui na estrutura.
           A duplicação é feita para evitar erros (por exemplo, se o espaço for desalocado) */
        vertices[i].vertice_nome = strdup(agnameof(v));
      }
    }
  }

  return vertices;
}

//------------------------------------------------------------------------------
double *obter_matriz_adjacencia(Agraph_t *g, vertice lista_vertices) {
  Agedge_t *a;
  Agnode_t *v;
  double *matriz = NULL;
  char *peso;
  unsigned int cauda_indice, cabeca_indice, i;
  int n_vertices;

  /* Número de vértices do grafo */
  n_vertices = agnnodes(g);

  if(n_vertices > 0) {
    /* Aloca a quantidade de memória necessária para armazenar todas as arestas */
    matriz = (double *) malloc(sizeof(double) * n_vertices * n_vertices);

    if(matriz != NULL) {
      /* Inicializa a matriz */
      for(i = 0; i < n_vertices * n_vertices; ++i) {
        matriz[i] = 0.0;
      }

      /* Percorre todas as arestas do grafo */
      for(v = agfstnode(g); v != NULL; v = agnxtnode(g, v)) {
        for(a = agfstedge(g, v); a != NULL; a = agnxtedge(g, a, v)) {
          cauda_indice = encontra_vertice(lista_vertices, n_vertices, agnameof(agtail(a)));
          cabeca_indice = encontra_vertice(lista_vertices, n_vertices, agnameof(aghead(a)));

          if(cauda_indice == -1 || cabeca_indice == -1) {
            free(matriz);
            return NULL;
          }

          peso = agget(a, "peso");
          matriz[cauda_indice * n_vertices + cabeca_indice] = (peso != NULL && *peso != '\0') ? atof(peso) : 1.0;
        }
      }
    }
  }

  return matriz;
}

//------------------------------------------------------------------------------
grafo le_grafo(FILE *input) {
  Agraph_t *g;
  grafo grafo_lido;

  grafo_lido = (grafo) malloc(sizeof(struct grafo));

  if(grafo_lido != NULL) {
    /* Armazena em g o grafo lido da entrada */
    if((g = agread(input, NULL)) == NULL) {
      destroi_grafo(grafo_lido);
      return NULL;
    }

    /* Carrega na estrutura os vértices de g */
    if((grafo_lido->grafo_vertices = obter_vertices(g, &(grafo_lido->grafo_n_vertices))) == NULL) {
      agclose(g);
      destroi_grafo(grafo_lido);
      return NULL;
    }

    /* Carrega na estrutura a matriz de adjacência de g */
    if((grafo_lido->grafo_matriz = obter_matriz_adjacencia(g, grafo_lido->grafo_vertices)) == NULL) {
      agclose(g);
      destroi_grafo(grafo_lido);
      return NULL;
    }

    /* Define o nome do grafo e se ele é direcionado */
    grafo_lido->grafo_direcionado = agisdirected(g);
    grafo_lido->grafo_nome = strdup(agnameof(g));

    agclose(g);
  }

  return grafo_lido;
}
//------------------------------------------------------------------------------
int destroi_grafo(grafo g) {
  if(g != NULL) {
    /* Libera a região de memória ocupada pelo nome do grafo, se não for nula */
    if(g->grafo_nome != NULL) {
      free(g->grafo_nome);
    }

    /* Libera a região de memória ocupada pelos vértices do grafo, se não for nula */
    if(g->grafo_vertices != NULL) {
      unsigned int i;

      /* Libera os nomes de todos os vértices */
      for(i = 0; i < g->grafo_n_vertices; ++i) {
        if(g->grafo_vertices[i].vertice_nome != NULL) {
          free(g->grafo_vertices[i].vertice_nome);
        }
      }

      free(g->grafo_vertices);
    }

    /* Libera a região de memória ocupada pela matriz de adjacência do grafo, se não for nula */
    if(g->grafo_matriz != NULL) {
      free(g->grafo_matriz);
    }

    /* Libera a região de memória ocupada pela estrutura do grafo */
    free(g);
  }

  return 1;
}
//------------------------------------------------------------------------------
grafo escreve_grafo(FILE *output, grafo g) {

  return output ? g : NULL;
}
//------------------------------------------------------------------------------
char *nome(grafo g) {

  return g ? g->grafo_nome : "";
}
//------------------------------------------------------------------------------
unsigned int n_vertices(grafo g) {

  return g ? g->grafo_n_vertices : 0;
}

//------------------------------------------------------------------------------
int direcionado(grafo g) {

  return g ? g->grafo_direcionado : 0;
}
//------------------------------------------------------------------------------
int conexo(grafo g) {

  return g ? 0 : 0;
}
//------------------------------------------------------------------------------
int fortemente_conexo(grafo g)  {

  return g ? 0 : 0;
}
//------------------------------------------------------------------------------
long int diametro(grafo g) {

  return g ? 0 : infinito;
}
//------------------------------------------------------------------------------

grafo distancias(grafo g) {

  return g;
}
