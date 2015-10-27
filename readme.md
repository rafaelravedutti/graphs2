# Trabalho de Implementação 2 de Algoritmos e Teoria dos Grafos (CI065)
    Autores:
    - Rafael Ravedutti Lucio Machado - rafaelm - GRR20135958
    - Alan Peterson Carvalho Silva - alan - GRR20110556
---
## Informações sobre o trabalho:

>No trabalho foi implementada uma estrutura de grafos com a matriz de adjacência
e parâmetros com as caracteristicas do grafo (i.e. se é direcionado, ponderado).

>Para verificar se um grafo é conexo ou fortemente conexo (no caso de ser direcionado)
fazemos operações envolvendo sua matriz de adjacência, assim, sendo M a matriz,
calculamos M^0 + M^1 + ... + M^(n-1), onde n é o número de vértices do grafo.
Após termos a matriz M* do grafo, verificamos se há um valor 0 na mesma, ou seja,
se existirem vértices u e v em G tais que não exista passeio de u à v de tamanho
0 até n-1, então G não é conexo (ou fortemente conexo no caso de grafos direcionados).

>Na função de distâncias e diâmetros, primeiro definimos a matriz M^0 com 0 na diagonal
principal e infinito nos outros valores. Em seguida, começamos a partir de M^1, e onde há
valores maiores ou iguais a 1 em M^1, atribuimos como 1 na matriz de distâncias (pois há
passeio de tamanho 1 entre os vértices). Então obtemos M^2, e atribuimos como 2 na matriz de
distâncias apenas se o valor for infinito (i.e. ainda não foi encontrado passeio
entre os vértices, então a distância deles é 2). E assim sucessivamente, até chegarmos
à M^(n-1), onde n é o número de vértices do grafo.

>Pensamos em armazenar as matrizes M^0, M^1, ..., M^(n-1), para poder reusá-las na função
de gerar a matriz de distâncias, entretanto pensamos que caso o grafo fosse alterado, 
teríamos que recalculá-las.
