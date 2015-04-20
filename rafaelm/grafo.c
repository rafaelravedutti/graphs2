#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cgraph.h>
#include "grafo.h"

//------------------------------------------------------------------------------
typedef struct grafo {
  int grafo_direcionado;
  int grafo_ponderado;
  char *grafo_nome;
  long int *grafo_matriz;
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
long int *obter_matriz_adjacencia(Agraph_t *g, vertice lista_vertices, int grafo_ponderado) {
  Agedge_t *a;
  Agnode_t *v;
  long int *matriz = NULL;
  char *peso;
  unsigned int cauda_indice, cabeca_indice, i;
  int n_vertices;

  /* Número de vértices do grafo */
  n_vertices = agnnodes(g);

  if(n_vertices > 0) {
    /* Aloca a quantidade de memória necessária para armazenar todas as arestas */
    matriz = (long int *) malloc(sizeof(long int) * n_vertices * n_vertices);

    if(matriz != NULL) {
      /* Inicializa a matriz */
      for(i = 0; i < n_vertices * n_vertices; ++i) {
        matriz[i] = 0;
      }

      /* Percorre todas as arestas do grafo */
      for(v = agfstnode(g); v != NULL; v = agnxtnode(g, v)) {
        for(a = agfstedge(g, v); a != NULL; a = agnxtedge(g, a, v)) {
          /* Obtêm os vértices de origem e destino da aresta */
          cauda_indice = encontra_vertice(lista_vertices, n_vertices, agnameof(agtail(a)));
          cabeca_indice = encontra_vertice(lista_vertices, n_vertices, agnameof(aghead(a)));

          /* Caso não sejam encontrados, retorna com um erro */
          if(cauda_indice == -1 || cabeca_indice == -1) {
            free(matriz);
            return NULL;
          }

          /* Se o grafo é ponderado, armazena o valor do peso (se existir) na matriz,
             caso contrário armazena o valor 1 */
          if(grafo_ponderado == 1) {
            peso = agget(a, "peso");
            matriz[cauda_indice * n_vertices + cabeca_indice] = (peso != NULL && *peso != '\0') ? atoi(peso) : 0;
          } else {
            matriz[cauda_indice * n_vertices + cabeca_indice] = 1;
          }
        }
      }
    }
  }

  return matriz;
}
//------------------------------------------------------------------------------
void multiplicar_matriz_quadrada(long int **matriz, long int *mult, unsigned int tamanho) {
  long int *matriz_resultado, *aux = *matriz;
  unsigned int i, j, k;

  matriz_resultado = (long int *) malloc(sizeof(long int) * tamanho * tamanho);

  if(matriz_resultado != NULL) {
    for(i = 0; i < tamanho; ++i) {
      for(j = 0; j < tamanho; ++j) {
        matriz_resultado[i * tamanho + j] = 0;

        for(k = 0; k < tamanho; ++k) {
          matriz_resultado[i * tamanho + j] += (*matriz)[i * tamanho + k] * mult[k * tamanho + j];
        }
      }
    }

    *matriz = matriz_resultado;
    free(aux);
  }
}
//------------------------------------------------------------------------------
void somar_matriz_quadrada(long int *matriz, long int *soma, unsigned int tamanho) {
  unsigned int i;

  for(i = 0; i < tamanho * tamanho; ++i) {
    matriz[i] += soma[i];
  }
}
//------------------------------------------------------------------------------
long int _obter_distancia(grafo g, unsigned int i, unsigned int j, long int tentativa) {
  long int *matriz, min, distancia;
  unsigned int n_vertices, a;

  /* Matriz de adjacência do grafo */
  matriz = g->grafo_matriz;
  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;

  /* Se já tentou encontrar um passeio até j com n - 1 tentativas,
     sendo n o número de vértices do grafo, portanto não há passeio
     até j, logo retorna infinito */
  if(tentativa >= n_vertices - 1) {
    return infinito;
  }

  /* Se M[i,j] não é nulo, então M[i,j] é a distância entre i e j */
  if(matriz[i * n_vertices + j] != 0) {
    return matriz[i * n_vertices + j];
  }

  /* Percorre todos os vértices do grafo */
  for(a = 0; a < n_vertices && a != i; ++a) {
    /* Se a é um vértice adjacente a i, então busca a distância entre
       a e j, e assim, recursivamente, até encontrar um vértice adjacente
       a j e, posteriormente, retornar o menor valor encontrado */
    if(matriz[i * n_vertices + a] != 0) {
      distancia = _obter_distancia(g, a, j, tentativa + 1);

      /* É necessário somar a distância de i à a na distância de a à j, para
         assim obter a distância de i à j */
      if(distancia != infinito && min > matriz[i * n_vertices + a] + distancia) {
        min = matriz[i * n_vertices + a] + distancia;
      }
    }
  }

  return min;
}
//------------------------------------------------------------------------------
long int obter_distancia(grafo g, unsigned int i, unsigned int j) {
  /* Se i e j são iguais (mesmos vértices), então retorna 0 */
  if(i == j) {
    return 0;
  }

  /* Verifica distâncias na primeira tentativa (0) */
  return _obter_distancia(g, i, j, 0);
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

    /* Verifica se g é um grafo ponderado ou não */
    if(agattr(g, AGEDGE, "peso", (char *) NULL) != NULL) {
      grafo_lido->grafo_ponderado = 1;
    } else {
      grafo_lido->grafo_ponderado = 0;
    }

    /* Carrega na estrutura a matriz de adjacência de g */
    if((grafo_lido->grafo_matriz = obter_matriz_adjacencia(g, grafo_lido->grafo_vertices, grafo_lido->grafo_ponderado)) == NULL) {
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
  struct vertice *v;
  char caractere_aresta;
  long int valor_matriz;
  unsigned int n_vertices, i, j;

  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;

  /* Imprime na saida a definição do grafo, caso seja um grafo direcionado,
     é adicionado o prefixo "di" */
  fprintf(output, "strict %sgraph \"%s\" {\n\n", (g->grafo_direcionado) ? "di" : "", g->grafo_nome);

  /* Imprime os nomes dos vértices */
  for(i = 0; i < n_vertices; ++i) {
    fprintf(output, "    \"%s\"\n", g->grafo_vertices[i].vertice_nome);
  }

  fprintf(output, "\n");

  /* Se o grafo é direcionado, representamos as arestas por v -> u,
     sendo v o vértice de origem e u o vértice de destino de cada aresta.
     Caso contrário, representamos as arestas por v -- u */
  caractere_aresta = (g->grafo_direcionado) ? '>' : '-';

  /* Percorre toda a matriz de adjacência do grafo */
  for(i = 0; i < n_vertices; ++i) {
    for(j = 0; j < n_vertices; ++j) {
      /* Obtêm o valor de M[i,j], sendo M a matriz de adjacência do grafo */
      valor_matriz = g->grafo_matriz[i * n_vertices + j];

      /* Se o valor for diferente de 0 (padrão), então imprime a aresta */
      if(valor_matriz != 0) {
        fprintf(output, "    \"%s\" -%c \"%s\"", g->grafo_vertices[i].vertice_nome, caractere_aresta, g->grafo_vertices[j].vertice_nome);

        /* Se g é um grafo ponderado, imprime o peso da aresta */
        if(g->grafo_ponderado == 1) {
          fprintf(output, " [peso=%d]", valor_matriz);
        }

        fprintf(output, "\n");
      }
    }
  }

  fprintf(output, "}\n");
  return g;
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
  long int *matriz_potencia, *matriz_soma;
  unsigned int n_vertices, i, j;

  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;
  /* Matriz de potência (exponencial), irá armazenar M^1, M^2, M^3, ... M^(n_vertices-1) */
  matriz_potencia = (long int *) malloc(sizeof(long int) * n_vertices * n_vertices);

  /* Se for nula, retorna erro de alocação dinâmica */
  if(matriz_potencia == NULL) {
    fprintf(stderr, "conexo(): Erro ao alocar memória para matriz exponencial!\n");
    return -1;
  }

  /* Matriz de soma (acumulativa), irá armazenar M^1 + M^2 + M^3 + ... M^(n_vertices-1) */
  matriz_soma = (long int *) malloc(sizeof(long int) * n_vertices * n_vertices);

  /* Se for nula, retorna erro de alocação dinâmica */
  if(matriz_soma == NULL) {
    free(matriz_potencia);
    fprintf(stderr, "conexo(): Erro ao alocar memória para matriz acumulativa!\n");
    return -1;
  }

  /* Inicializa matriz_potencia <- M^1 e matriz_soma <- M, sendo M a matriz de adjacência do grafo */
  for(i = 0; i < n_vertices * n_vertices; ++i) {
    matriz_potencia[i] = g->grafo_matriz[i];
    matriz_soma[i] = g->grafo_matriz[i];
  }

  /* Multiplica a matriz_potencia por M aumentando seu grau, ou seja, se matriz_potencia = M^x, então
     matriz_potencia <- M^(x+1), e soma M^(x+1) à matriz_soma */
  for(i = 0; i < n_vertices; ++i) {
    multiplicar_matriz_quadrada(&matriz_potencia, g->grafo_matriz, n_vertices);
    somar_matriz_quadrada(matriz_soma, matriz_potencia, n_vertices);
  }

  /* Como M^n possui valores definidos em i,j onde existe passeio de tamanho n entre os vértices i e j,
     então M^1 + M^2 + ... + M^(n_vertices-1) = matriz_soma possui valores definidos entre vértices onde
     existe passeio entre os mesmos. Logo se há um valor nulo, o grafo não é conexo */
  for(i = 0; i < n_vertices; ++i) {
    for(j = 0; j < n_vertices; ++j) {
      if(matriz_soma[i * n_vertices + j] == 0) {
        return 0;
      }
    }
  }

  return 1;
}
//------------------------------------------------------------------------------
int fortemente_conexo(grafo g)  {
  unsigned int n_vertices, i, j;

  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;

  /* Percorre todos os valores da matriz de adjacência do grafo */
  for(i = 0; i < n_vertices; ++i) {
    for(j = 0; j < n_vertices; ++j) {
      /* Se há um valor nulo, então o grafo não é fortemente conexo */
      if(g->grafo_matriz[i * n_vertices + j] == 0) {
        return 0;
      }
    }
  }

  return 1;
}
//------------------------------------------------------------------------------
long int diametro(grafo g) {
  long int max = 0, distancia;
  unsigned int n_vertices, i, j;

  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;

  /* Percorre todos os valores da matriz de adjacência do grafo */
  for(i = 0; i < n_vertices; ++i) {
    for(j = 0; j < n_vertices; ++j) {
      /* Obtêm a distância entre os vértices */
      distancia = obter_distancia(g, i, j);

      /* Se a distância é maior que o valor máximo, então atribui ela ao valor máximo */
      if(max < distancia) {
        max = distancia;
      }
    }
  }

  return max;
}
//------------------------------------------------------------------------------
grafo distancias(grafo g) {
  grafo grafo_distancias;
  unsigned int n_vertices, i, j;

  /* Estrutura do grafo de distâncias */
  grafo_distancias = (grafo) malloc(sizeof(struct grafo));
  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;

  if(grafo_distancias != NULL) {
    /* Inicializa grafo de distâncias */
    grafo_distancias->grafo_direcionado = g->grafo_direcionado;
    grafo_distancias->grafo_ponderado = g->grafo_ponderado;
    grafo_distancias->grafo_nome = strdup(g->grafo_nome);
    grafo_distancias->grafo_n_vertices = n_vertices;

    /* Aloca vértices do grafo de distâncias */
    grafo_distancias->grafo_vertices = (vertice) malloc(sizeof(struct vertice) * n_vertices);

    /* Copia vértices do grafo de distâncias */
    if(grafo_distancias->grafo_vertices != NULL) {
      for(i = 0; i < n_vertices; ++i) {
        grafo_distancias->grafo_vertices[i].vertice_nome = strdup(g->grafo_vertices[i].vertice_nome);
      }
    }

    /* Aloca matriz de adjacência do grafo de distâncias */
    grafo_distancias->grafo_matriz = (long int *) malloc(sizeof(long int) * n_vertices * n_vertices);

    /* Define as distâncias na matriz de adjacência do grafo */
    if(grafo_distancias->grafo_matriz != NULL) {
      for(i = 0; i < n_vertices; ++i) {
        for(j = 0; j < n_vertices; ++j) {
          grafo_distancias->grafo_matriz[i * n_vertices + j] = obter_distancia(g, i, j);
        }
      }
    }
  }

  return grafo_distancias;
}
