/*
 *  Thenter-Crawler para Hidden Service
 *	@utor: soneca
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "torproxy.c"
#include "crawl.c"

int main(int argc, char *argv[]){
 	FILE *lista, *site;
 	char linha_lista[200]="\0", urls[10000][63], linha_ant_lista[200]="\0";
 	int total, cont_links, inicio=1;

 	/* Gestão de argumentos de entrada */
 	char link[300]="n5dxszob62pha3q5yz3k5akya4stx5c2ydvwdp5smc7y7foq3ple5zyd.onion/thenter/onion.php";
	if(argc == 2){
		if( torsocks(argv[1], ".temp.html")!=0 )
			inicio=0;
	}else if(argc>2){
		printf("[\e[31mERRO\e[0m] Erro : Argumentos demais!\n");
		return -1;
	}else
		torsocks(link, ".temp.html");

	/* Abertura de arquivo */
 	lista = fopen("lista.txt", "a+");
 	if (lista==NULL){
		printf("[\e[31mERRO\e[0m] Erro ao abrir a lista de links.");
		return -1;
	}

	/* Salta links da lista, vai para o final do arquivo */
	while( fgets(linha_lista, 200, lista) );

	/* Laço principal */
 	do{
 		if(inicio){
 			inicio=0;

			/* Pega links e remove arquivo temporário */
			site = fopen(".temp.html", "r+");
			if(site==NULL){
				fprintf(stderr, "[\e[31mERRO\e[0m] Arquivo vazio");
				continue;
			}
			filtrar_urls(site, urls);
			fclose(site);
			remove(".temp.html");

			/* Conta quantos links foram achados */
 			for( total=0; strlen(urls[total]); total++ );
 			printf("[\e[32mINFO\e[0m] Links encontrados: %d\n\e[0m", total);

 			for(int cont=0; strlen( urls[cont] ); cont++){
 				if( !registrado(urls[cont]) ){
 					if( torsocks(urls[cont], ".site.html")==0 )
 						salvar_link(urls[cont], "(on)");
 					else
 						salvar_link(urls[cont], "(off)");
 				}

 				printf("[\e[32mINFO\e[0m] %.3d/%.3d | %s\n", cont+1, total, urls[cont] );
			}

			/* Limpa urls */
			memset(&urls, 0, sizeof(char)*10000*63);
 		}
 		//se o link da retro alimentação estiver (off)
 		if( strstr(linha_ant_lista, "(off)")!=NULL ){
 			rm_nova_linha(linha_lista);
 			printf("[\e[32mINFO\e[0m] \e[33mSaltamos link\e[0m de retroalimentação, site (off) ~> %s\n", linha_lista);
 		}else{
 			//se for link .onion
 			if( strlen(linha_lista)==23 && strstr(linha_lista, ".onion")!=NULL ){
 				rm_nova_linha(linha_lista);
 				printf("[\e[32mINFO\e[0m] Entramos em \e[33mretroalimentação\e[0m! ~> %s\n", linha_lista);
 				
 				if( torsocks(linha_lista, ".temp.html")!=0 )
 					continue;

 				site=fopen(".temp.html", "r+");
 				if(site==NULL){
					fprintf(stderr, "[\e[31mERRO\e[0m] Arquivo vazio");
					exit(1);
				}
 				filtrar_urls(site, urls); //		trás os links no vertor de strings url[][22]
 				fclose(site);
 				remove(".temp.html");

 				/* Conta quantos links foram encontrados */
 				for(cont_links=0; strlen(urls[cont_links]); cont_links++);
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
 			}else if( strcmp(linha_lista, "\0")==0 ){
 			}
 		}
 		strcpy(linha_ant_lista, linha_lista);
	}while( fgets(linha_lista, 200, lista) );

	fclose(lista);

	puts("[\e[33mINFO\e[0m] Tarefa concluída!");
	system("notify-send -i terminal Crawler \"Tarefa concluída\".");
	puts("\e[1;33m\tacenos!\e[0m");

	return 0;
}
