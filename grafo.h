//------------------------------------------------------------------------------
// (apontador para) estrutura de dados que representa um grafo simples
// através de sua matriz de adjacência
// 
// o grafo pode ser
// - direcionado ou não
// - com pesos nas arestas ou não
// 
// além de vértices e arestas, o grafo tem um nome que é uma "string"
// 
// num grafo com pesos nas arestas todas as arestas tem peso, que 
// 
// o peso de uma aresta é um long int e seu valor default é zero

typedef struct grafo *grafo;

//------------------------------------------------------------------------------
// (apontador para) estrutura de dados que representa um vértice do grafo
// 
// os vértices do grafo tem nome
// 
// os nomes dos vértices são strings quaisquer

typedef struct vertice *vertice;

//------------------------------------------------------------------------------
// valor que representa "infinito"

const long int infinito;

//------------------------------------------------------------------------------
// lê um grafo no formato dot de input, usando as rotinas de libcgraph
// 
// desconsidera todos os atributos do grafo lido
// exceto o atributo "peso" nas arestas onde ocorra
// 
// devolve o grafo lido, ou
//         NULL em caso de erro 

grafo le_grafo(FILE *input);  

//------------------------------------------------------------------------------
// desaloca toda a memória usada em *g
// 
// devolve 1 em caso de sucesso ou
//         0 caso contrário

int destroi_grafo(grafo g);

//------------------------------------------------------------------------------
// escreve o grafo g em output usando o formato dot, de forma que
// 
// 1. todos os vértices são escritos antes de todas as arestas (arcos)
// 2. se uma aresta (arco) tem peso, este deve ser escrito como um atributo
//
// devolve o grafo escrito ou
//         NULL, em caso de erro 

grafo escreve_grafo(FILE *output, grafo g);

//------------------------------------------------------------------------------
// devolve o nome do grafo g

char *nome(grafo g);

//------------------------------------------------------------------------------
// devolve o número de vértices do grafo g

unsigned int n_vertices(grafo g);

//------------------------------------------------------------------------------
// devolve 1, se g é direcionado, ou
//         0, caso contrário

int direcionado(grafo g);

//------------------------------------------------------------------------------
// devolve 1, se g é conexo, ou
//         0, caso contrário

int conexo(grafo g);

//------------------------------------------------------------------------------
// devolve 1, se g é fortemente conexo, ou
//         0, caso contrário

int fortemente_conexo(grafo g);

//------------------------------------------------------------------------------
// devolve o diâmetro do grafo g

long int diametro(grafo g);

//------------------------------------------------------------------------------
// devolve um grafo com pesos, onde
//
//     - os vértices são "os mesmos" (tem os mesmos nomes)
//       que os vértices de g
//
//     - a aresta {u,v} (arco (u,v)) ocorre se v é alcançável a partir
//       de u em g
//
//       neste caso, seu peso é a distância de u a v em g, entendida
//       como tamanho (e não peso) do menor caminho de u a v em g

grafo distancias(grafo g);

