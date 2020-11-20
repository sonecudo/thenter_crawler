/*
 * Para testar função de captura de títulos
 */

#include <stdio.h>
#include <ctype.h>
#include "torproxy.h"
#include "crawl.h"

int main(int argc, char *argv[]){

	//gerenciamento da argumentos
	if( argc!=2 ){
		fprintf(stderr, "Uso: %s [link]\n", argv[0]);
		return -1;
	}else{
		remove(".teste_titulo.html");
		torsocks(argv[1], ".teste_titulo.html");
		pega_titulo(".teste_titulo.html");
	}

	return 0;
}
