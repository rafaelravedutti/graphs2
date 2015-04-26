#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <graphviz/cgraph.h>
#include "grafo.h"

/* Protótipos das funções utilizadas */
int encontra_vertice(vertice, unsigned int, const char *);
vertice obter_vertices(Agraph_t *, unsigned int *);
long int *obter_matriz_adjacencia(Agraph_t *, vertice, int, int, unsigned int);
void multiplicar_matriz_quadrada(long int **, long int *, unsigned int);
void somar_matriz_quadrada(long int *, long int *, unsigned int);
long int _obter_distancia(grafo, unsigned int, unsigned int, long int);
long int obter_distancia(grafo, unsigned int, unsigned int);
long int *gerar_matriz_distancias(grafo);

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
      return (int)i;
    }
  }

  return -1;
}

//------------------------------------------------------------------------------
vertice obter_vertices(Agraph_t *g, unsigned int *n_vertices) {
  Agnode_t *v;
  vertice vertices = NULL;
  unsigned int i;

  /* Número de vértices do grafo */
  *n_vertices = (unsigned int) agnnodes(g);

  if(*n_vertices > 0) {
    /* Aloca a quantidade de memória necessária para armazenar todos os vértices */
    vertices = (vertice) malloc(sizeof(struct vertice) * (*n_vertices));

    if(vertices != NULL) {
      /* Percorre todos os vértices do grafo */
      for(i = 0, v = agfstnode(g); i < *n_vertices; ++i, v = agnxtnode(g, v)) {
        /* Duplica na memória o nome do vértice e o atribui na estrutura.
           A duplicação é feita para evitar erros (por exemplo, se o espaço for desalocado) */
        vertices[i].vertice_nome = strdup(agnameof(v));
      }
    }
  }

  return vertices;
}

//------------------------------------------------------------------------------
long int *obter_matriz_adjacencia(Agraph_t *g, vertice lista_vertices, int grafo_ponderado, int grafo_direcionado, unsigned int n_vertices) {
  Agedge_t *a;
  Agnode_t *v;
  long int *matriz = NULL;
  char *peso;
  char peso_string[] = "peso";
  unsigned i;
  int cauda_indice, cabeca_indice;

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
            peso = agget(a, peso_string);
            matriz[cauda_indice * (int) n_vertices + cabeca_indice] = (peso != NULL && *peso != '\0') ? atol(peso) : 0;
          } else {
            matriz[cauda_indice * (int) n_vertices + cabeca_indice] = 1;
          }

          /* Se o grafo não é direcionado, então M[i,j] = M[j,i] */
          if(grafo_direcionado != 1) {
            matriz[cabeca_indice * (int) n_vertices + cauda_indice] = matriz[cauda_indice * (int) n_vertices + cabeca_indice];
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

  /* Aloca espaço para matriz resultado */
  matriz_resultado = (long int *) malloc(sizeof(long int) * tamanho * tamanho);

  if(matriz_resultado != NULL) {
    /* Percorre todos os indices da matriz resultado */
    for(i = 0; i < tamanho; ++i) {
      for(j = 0; j < tamanho; ++j) {
        /* Define matriz_resultado[i,j] como 0 (valor neutro da adição) */
        matriz_resultado[i * tamanho + j] = 0;

        /* Soma em matriz_resultado[i,j] o produto de M[i,k] com M[k,j],
           para todo k inteiro no intervalo [0,tamanho] */
        for(k = 0; k < tamanho; ++k) {
          matriz_resultado[i * tamanho + j] += (*matriz)[i * tamanho + k] * mult[k * tamanho + j];
        }
      }
    }

    /* Guarda o endereço da matriz resultado no ponteiro, e libera a matriz anterior */
    *matriz = matriz_resultado;
    free(aux);
  }
}
//------------------------------------------------------------------------------
void somar_matriz_quadrada(long int *matriz, long int *soma, unsigned int tamanho) {
  unsigned int i;

  /* Percorre todas as posições da matriz */
  for(i = 0; i < tamanho * tamanho; ++i) {
    /* Soma em matriz[i] o valor de soma[i] */
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
long int *gerar_matriz_distancias(grafo g) {
  long int *matriz_potencia, *matriz_distancias;
  unsigned int n_vertices, i, j;

  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;
  /* Matriz de potência (exponencial), irá armazenar M^1, M^2, M^3, ... M^(n_vertices-1) */
  matriz_potencia = (long int *) malloc(sizeof(long int) * n_vertices * n_vertices);

  /* Se for nula, retorna erro de alocação dinâmica */
  if(matriz_potencia == NULL) {
    fprintf(stderr, "gerar_matriz_distancias(): Erro ao alocar memória para matriz exponencial!\n");
    return NULL;
  }

  /* Matriz de distâncias */
  matriz_distancias = (long int *) malloc(sizeof(long int) * n_vertices * n_vertices);

  /* Se for nula, retorna erro de alocação dinâmica */
  if(matriz_distancias == NULL) {
    free(matriz_potencia);
    fprintf(stderr, "gerar_matriz_distancias(): Erro ao alocar memória para matriz de distâncias!\n");
    return NULL;
  }

  /* Inicializa matriz_potencia <- M^1 e matriz_distancias <- M, sendo M a matriz de adjacência do grafo */
  for(i = 0; i < n_vertices * n_vertices; ++i) {
    matriz_potencia[i] = g->grafo_matriz[i];
    matriz_distancias[i] = infinito;
  }

  /* Zera diagonal principal da matriz de distâncias, pois d(v,v) = 0 */
  for(i = 0; i < n_vertices; ++i) {
    matriz_distancias[i * n_vertices + i] = 0;
  }

  /* Multiplica a matriz_potencia por M aumentando seu grau, ou seja, se matriz_potencia = M^x, então
     matriz_potencia <- M^(x+1) */
  for(i = 0; i < n_vertices - 1; ++i) {
    for(j = 0; j < n_vertices * n_vertices; ++j) {
      if(matriz_potencia[j] != 0 && matriz_distancias[j] > i + 1) {
        matriz_distancias[j] = i + 1;
      }
    }

    multiplicar_matriz_quadrada(&matriz_potencia, g->grafo_matriz, n_vertices);
  }

  /* Libera espaço ocupado pela matriz exponencial */
  free(matriz_potencia);

  return matriz_distancias;
}
//------------------------------------------------------------------------------
grafo le_grafo(FILE *input) {
  Agraph_t *g;
  grafo grafo_lido;
  char peso_string[] = "peso";

  /* Aloca estrutura do grafo lido */
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
    if(agattr(g, AGEDGE, peso_string, (char *) NULL) != NULL) {
      grafo_lido->grafo_ponderado = 1;
    } else {
      grafo_lido->grafo_ponderado = 0;
    }
    
    /* Define o nome do grafo e se ele é direcionado */
    grafo_lido->grafo_direcionado = agisdirected(g);
    grafo_lido->grafo_nome = strdup(agnameof(g));

    /* Carrega na estrutura a matriz de adjacência de g */
    if((grafo_lido->grafo_matriz = obter_matriz_adjacencia(g, grafo_lido->grafo_vertices, grafo_lido->grafo_ponderado, grafo_lido->grafo_direcionado, grafo_lido->grafo_n_vertices)) == NULL) {
      agclose(g);
      destroi_grafo(grafo_lido);
      return NULL;
    }

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
  //struct vertice *v;
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
          if(valor_matriz == infinito) {
            fprintf(output, " [peso=oo]");
          } else {
            fprintf(output, " [peso=%ld]", valor_matriz);
          }
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
  char empty[] = "";

  return g ? g->grafo_nome : empty;
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
  for(i = 0; i < n_vertices - 1; ++i) {
    multiplicar_matriz_quadrada(&matriz_potencia, g->grafo_matriz, n_vertices);
    somar_matriz_quadrada(matriz_soma, matriz_potencia, n_vertices);
  }

  /* Incrementa os valores da diagonal principal do grafo, pois todos os vetores são
     alcançáveis por si mesmos, aqui se soma M^0 */
  for(i = 0; i < n_vertices; ++i) {
    matriz_soma[i * n_vertices + i]++;
  }

  /* Libera memória ocupada pela matriz exponencial */
  free(matriz_potencia);

  /* Como M^n possui valores definidos em i,j onde existe passeio de tamanho n entre os vértices i e j,
     então M^1 + M^2 + ... + M^(n_vertices-1) = matriz_soma possui valores definidos entre vértices onde
     existe passeio entre os mesmos. Logo se há um valor nulo, o grafo não é conexo */
  for(i = 0; i < n_vertices; ++i) {
    for(j = 0; j < n_vertices; ++j) {
      if(matriz_soma[i * n_vertices + j] == 0) {
        /* Libera memória ocupada pela matriz acumulativa */
        free(matriz_soma);
        return 0;
      }
    }
  }

  /* Libera memória ocupada pela matriz acumulativa */
  free(matriz_soma);
  return 1;
}
//------------------------------------------------------------------------------
int fortemente_conexo(grafo g) {
  /* O procedimento usado é o mesmo para verificar se um grafo não direcionado é conexo,
     a diferença é que a matriz é assimétrica pois cada arco é utilizado apenas em um sentido
     (e não nos dois como nas arestas), portanto, utilizando a função conexo é possível verificar
     se o grafo também é fortemente conexo */
  return conexo(g);
}
//------------------------------------------------------------------------------
long int diametro(grafo g) {
  long int max = 0;
  long int *matriz_distancias;
  unsigned int n_vertices, i;

  /* Gera matriz de distâncias */
  matriz_distancias = gerar_matriz_distancias(g);
  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;

  /* Percorre toda a matriz de distâncias e obtêm o valor máximo (exceto infinito) */
  if(matriz_distancias != NULL) {
    for(i = 0; i < n_vertices * n_vertices; ++i) {
      if(matriz_distancias[i] != infinito && max < matriz_distancias[i]) {
        max = matriz_distancias[i];
      }
    }

    /* Libera memória ocupada pela matriz de distâncias */
    free(matriz_distancias);
  }

  /* Retorna o valor máximo encontrado */
  return max;
}
//------------------------------------------------------------------------------
grafo distancias(grafo g) {
  grafo grafo_distancias;
  unsigned int n_vertices, i;

  /* Estrutura do grafo de distâncias */
  grafo_distancias = (grafo) malloc(sizeof(struct grafo));
  /* Número de vértices do grafo */
  n_vertices = g->grafo_n_vertices;

  if(grafo_distancias != NULL) {
    /* Inicializa grafo de distâncias */
    grafo_distancias->grafo_direcionado = g->grafo_direcionado;
    grafo_distancias->grafo_ponderado = 1;
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
    grafo_distancias->grafo_matriz = gerar_matriz_distancias(g);
  }

  return grafo_distancias;
}
