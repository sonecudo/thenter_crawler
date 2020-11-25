/*
 *  Thenter-Crawler para Hidden Service
 *	@utor: soneca
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "torproxy.h"
#include "crawl.h"

void banner(){
	printf(	"\n"
			"  __   _          _   _   _                      _   _  \n"
			" (_   / \\  |\\ |  |_  /   |_)   /\\   \\    /  |   |_  |_) \n"
			" __)  \\_/  | \\|  |_  \\_  | \\  /--\\   \\/\\/   |_  |_  | \\ \n"
			"                                            \e[3;32mtinyurl.com/thenterBr\e[0m\n\n");
}

int main(int argc, char *argv[]){
	FILE *lista, *site;
	char linha_lista[200]="\0", urls[10000][63], linha_ant_lista[200]="\0";
	unsigned int cont_links, inicio=1;
	char link[300];

	banner();

/* Gestão de argumentos de entrada */
	if(argc == 2){
		strcpy(link, argv[1]);
	}else if(argc>2){
		printf("[\e[31mERRO\e[0m] Erro : Argumentos demais!\n");
		return -1;
	}else{
		strcpy(link, "n5dxszob62pha3q5yz3k5akya4stx5c2ydvwdp5smc7y7foq3ple5zyd.onion/thenter/onion.php");
	}
//
/* Abertura de arquivo */
	lista = fopen("lista.txt", "a+");
	if (lista==NULL){
		printf("[\e[31mERRO\e[0m] Erro ao abrir a lista de links.");
		return -1;
	}
//

	/* Salta links da lista, vai para o final do arquivo */
	while( fgets(linha_lista, 200, lista) );

	/* Laço principal */
	do{
	// Casos a ignorar linha do arquivo
		if( (strcmp(linha_lista, "\0")==0 && !inicio) || strstr(linha_ant_lista, "(off)")!=NULL ){
			strcpy(linha_ant_lista, linha_lista);
			continue;
	//

		//caso de usar link do arquivo ou início
		}else if
		(
			inicio ||
			((strlen(linha_lista)==23 || strlen(linha_lista)==63) && strstr(linha_lista, ".onion")!=NULL)
		){
			if(inicio){
				inicio=0;
			}else{
				rm_nova_linha(linha_lista);
				strcpy(link, linha_lista);
				printf("[\e[32mINFO\e[0m] Entramos em \e[33mretroalimentação\e[0m! ~> %s\n", link);
			}

			if( torsocks(link, ".temp.html")!=0 )
				continue;

			site=fopen(".temp.html", "r+");
			if(site==NULL){
				fprintf(stderr, "[\e[31mERRO\e[0m] Arquivo vazio\n");
				exit(1);
			}
			filtrar_urls(site, urls); //		trás os links no vertor de strings url[][63]
			fclose(site);
			remove(".temp.html");

			/* Conta quantos links foram encontrados */
			for(cont_links=0; strlen(urls[cont_links])>0; cont_links++);
			printf("\e[32m[\e[32mINFO\e[0m] Links encontrados: %d\n\e[0m", cont_links);

			for(int cont=0; strlen( urls[cont] ) ; cont++){
				if( !registrado(urls[cont]) ){
					if( torsocks(urls[cont], ".site.html")==0 )
						salvar_link(urls[cont], "(on)");
					else
						salvar_link(urls[cont], "(off)");
				}
				printf("[\e[32mINFO\e[0m] %.3d|%.3d %s\n", cont+1, cont_links, urls[cont] );
			}

			// Limpa urls
			memset(&urls, 0, sizeof(char)*10000*63);
		}

		// faz o que o fflush() deveria fazer
		atualiza(lista);
		// fim do flush()

		strcpy(linha_ant_lista, linha_lista);
		fflush(lista);
	}while( fgets(linha_lista, 200, lista) );

	puts("[\e[33mINFO\e[0m] Tarefa concluída!");
	system("notify-send -i tor Crawler \"Tarefa concluída\".");
	puts("\e[1;33m\tacenos!\e[0m");
	return 0;
}
