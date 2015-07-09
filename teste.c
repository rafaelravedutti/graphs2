#include <stdio.h>
#include "grafo.h"

int main(void) {

  grafo g = le_grafo(stdin);

  long int d = diametro(g);

  printf("%s\n", nome(g));

  printf("%sdirecionado\n", direcionado(g) ? "" : "não ");

  if ( direcionado(g) )
    
    printf("%sfortemente conexo\n", fortemente_conexo(g) ? "" : "não ");
  
  else
    
    printf("%sconexo\n", conexo(g) ? "" : "des");
  
  if ( d == infinito )

    printf("diâmetro: oo\n");

  else

    printf("diâmetro: %ld\n", d);

  destroi_grafo(escreve_grafo(stdout, distancias(g)));

  printf("\n");

    return ! destroi_grafo(g);
}
