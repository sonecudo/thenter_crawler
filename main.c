/*
 *  Thenter-Crawler para Hidden Service
 *	@utor: soneca
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "torproxy.c"
#include "crawl.c"

int main(int argc, char *argv[]){
 	FILE *lista, *site;
 	char linha_lista[200]="\0", urls[10000][63], linha_ant_lista[200]="\0";
 	int total, cont_links, inicio=1;

 	/* Gerencia argumentos de entrada */
 	char link[300]="pzqcmpphmomfvih2.onion/thenter/onion.php";
	if(argc == 2){
		if( torsocks(argv[1], ".temp.html")!=0 )
			inicio=0;
	}else if(argc>2){
		printf("\e[31m[!] Erro : Argumentos demais!\n \e[0m");
		return -1;
	}else
		torsocks(link, ".temp.html");

	/* Abertura de arquivo */
 	lista = fopen("lista.txt", "a+");
 	if (lista==NULL){
		printf("[!] Erro ao abrir a lista de links.");
		return -1;
	}

	/* Stalta links pré-existentes, para o final do arquivo */
	while( fgets(linha_lista, 200, lista) );

	/* Laço principal */
 	do{
 		if(inicio){
 			inicio=0;

			/* Pega links e remove arquivo temporário */
			site = fopen(".temp.html", "r+");
			if(site==NULL){
				fprintf(stderr, "arquivo vazio!");
				continue;
			}
			filtrar_urls(site, urls);
			fclose(site);
			remove(".temp.html");

			/* Conta quantos links foram achados */
 			for( total=0; strlen(urls[total]); total++ );
 			printf("[\e[32mINFO\e[0m] Links encontrados: %d\n\e[0m", total);

 			for(int cont=0; strlen( urls[cont] ); cont++){
 				if( registro(urls[cont]) ){
 					if( site_online(urls[cont]) ) //verificar se está oline
 						salvar_link( urls[cont] , "(on)" );//salvar em lista.txt
 					else
 						salvar_link( urls[cont] , "(off)" );
 				}

 				printf("[\e[32mINFO\e[0m] %.3d/%.3d | %s\n", cont+1, total, urls[cont] );
			}

			/* Limpa urls */
			memset(&urls, 0, sizeof(char)*10000*63);
			puts("ESTOU AQUI!");
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
					fprintf(stderr, "arquivo vazio!");
					exit(1);
				}
 				filtrar_urls(site, urls); //		trás os links no vertor de strings url[][22]
 				fclose(site);
 				remove(".temp.html");

 				/* Conta quantos links foram encontrados */
 				for(cont_links=0; strlen(urls[cont_links]); cont_links++);
 				printf("\e[32m[\e[32mINFO\e[0m] Links encontrados: %d\n\e[0m", cont_links);

 				for(int cont=0; strlen( urls[cont] ) ; cont++){
 					if( registro(urls[cont]) ){
 						if( site_online(urls[cont]) ) //verificar se está oline
 							salvar_link( urls[cont] , "(on)" );//salvar em lista.txt
 						else
 							salvar_link( urls[cont] , "(off)" );
 					}

 					printf("[\e[32mINFO\e[0m] %.3d| %s\n", cont+1, urls[cont] );
				}

				// Limpa urls
				memset(&urls, 0, sizeof(char)*10000*63);
 			}else if( strcmp(linha_lista, "\0")==0 ){
 			}
 		}
 		strcpy(linha_ant_lista, linha_lista);
	}while( fgets(linha_lista, 200, lista) );

	fclose(lista);

	puts("\e[1;33mTarefa concluída!\e[0m");
	system("notify-send -i terminal Crawler \"Tarefa concluída\".");
	puts("acenos!");

	return 0;
}
