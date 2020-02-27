/*
 *	Interface de acesso à onion
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "funcs.c"

struct sockaddr_in remoto;

// meu timeout
int con_timeout(int fd,int sec){
	fd_set readfds;
	struct timeval timeout;
	
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);

	timeout.tv_sec=sec;
	timeout.tv_usec=0;
	return select(fd+1, &readfds, NULL, NULL, &timeout);
}
// fim do meu timeout

int torsocks(char *endereco, char *arq_saida){
	int sockfd, rlen;
	char *buf;
	buf = malloc( sizeof(char)*256); //255+'\0'
	memset(buf, 0, 256);
	char *texto_pag=malloc(sizeof(char)*1); //'\0'

	if( (sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1){
		fprintf(stderr, "[\e[31mERRO\e[0m] erro em socket()\n");
		return -1;
	}

	remoto.sin_family			=AF_INET;
	remoto.sin_port				=htons(9050);
	remoto.sin_addr.s_addr		=inet_addr("127.0.0.1");

	if( connect(sockfd, (struct sockaddr*) &remoto, sizeof remoto)==-1 ){
		fprintf(stderr, "[\e[31mERRO\e[0m] erro em connect()\n");
		return -1;
	}

	//			PRIMEIRA TROCA DE MENSAGENS - GREETING FROM CLIENT
	memcpy(buf, "\x05\x01\x00", 3);
	if( (rlen=send(sockfd, buf, 3, 0))==-1){
		fprintf(stderr, "[\e[31mERRO\e[0m] erro em send()\n");
		return -1;
	}
	//printf("Enviados: %d bytes\n", rlen);
	//printf("recebendo...\n");
	if( (rlen=recv(sockfd, buf, 256, 0))>0 ){
		printf("[\e[32mINFO\e[0m] Resposta SOCKS5: ");
		print_data_int(buf, rlen);
	}

	//			SEGUNDA TROCA DE MENSAGENS - CLIENT'S CONNECTION REQUEST
	char *link, *pagina;
	link=malloc(sizeof(char)*strlen(endereco));
	memset(link, 0, strlen(endereco));
	pagina=malloc(sizeof(char)*strlen(endereco));
	memset(pagina, 0, strlen(endereco));
	ler_link(link, pagina, endereco);

	int data_len=7+strlen(link);
	char con_req[data_len];

	memcpy(con_req, "\x05\x01\x00\x03", 4);
	*(con_req+4)=strlen(link);
	strcpy(con_req+5, link);
	memcpy(con_req+data_len-2, "\x00\x50", 2);

	memset(buf, 0, 256);
	memcpy(buf, con_req, data_len);
	if( (rlen=send(sockfd, buf, data_len, 0))==-1){
		fprintf(stderr, "[\e[31mERRO\e[0m] Erro em send()\n");
		return -1;
	}
	//printf("enviados: %d bytes\n", rlen);
	//printf("recebendo...\n");
	if( con_timeout(sockfd,10)==0 ){
		printf("[\e[33mINFO\e[0m] Pesquisa de DNS expirou\n");
		free(buf);
		free(texto_pag);
		return -1;
	}
	if( (rlen=recv(sockfd, buf, 255, 0))>0 ){
		printf("[\e[32mINFO\e[0m] Resposta SOCKS5: ");
		print_data_int(buf, rlen);
		if(buf[1]!='\x00'){
			// relato de erro de conexão
			if(buf[1]==3){
				fprintf(stderr, "[\e[31mERRO\e[0m] Rede indiponível!\n");
			}else if(buf[1]==2){
				fprintf(stderr, "[\e[31mERRO\e[0m] Conexão não permitida pelo ruleset!\n");
			}else if(buf[1]==4){
				fprintf(stderr, "[\e[31mERRO\e[0m] Host inalcançável!\n");
			}else if(buf[1]==5){
				fprintf(stderr, "[\e[31mERRO\e[0m] Conexão recusada!\n");
			}else if(buf[1]==6){
				fprintf(stderr, "[\e[31mERRO\e[0m] Tentativa de conexão expirou!\n");
			}else{
				fprintf(stderr, "[\e[31mERRO\e[0m] Requisição falhou!\n"
								"[\e[33mAVISO\e[0m] Confira o formato do link.\n");
			}
			free(buf);
			free(texto_pag);
			return -1;
		}
	}

	//			TERCEIRA TROCA DE MENSAGENS
	memset(buf, 0, 256);
	sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s\r\n"
	"User-Agent: Mozilla/5.0 (Windows NT 10.0; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
	"Accept: */*\r\n\r\n", pagina, link);

	free(link);
	free(pagina);

	if( (rlen=send(sockfd, buf, strlen(buf), 0))==-1){
		fprintf(stderr, "[\e[31mERRO\e[0m] Erro em send()\n");
		return -1;
	}
	//printf("enviados: %d bytes\n", rlen);
	//printf("recebendo...\n");
	memset(texto_pag, 0, 1);
	int leitura=0;
	do{
		memset(buf, 0, 256);
		if(leitura==0){
			if( con_timeout(sockfd, 10)==0 ){
				printf("[\e[33mINFO\e[0m] Conexão expirou\n");
				free(buf);
				free(texto_pag);
				return -1;
			}
		}
		if(leitura>0){ // timeout secundário (acenos mano caverno)
			if( con_timeout(sockfd, 1)==0 ){
				printf("[\e[33mINFO\e[0m] Conexão expirou(no fim)\n");
				break;
			}
		}
		rlen=recv(sockfd, buf, 255, 0);
		texto_pag=realloc(texto_pag, strlen(texto_pag)+1+rlen );
		strcat(texto_pag, buf);
		leitura++;
	}while( rlen>0 &&
			strstr(buf, "\r\n0\r\n\r\n")==NULL &&
			strstr(buf, "</html>")==NULL &&
			strstr(buf, "</body>")==NULL
			);
	if( strlen(texto_pag)==0 ){
		fprintf(stderr, "[\e[31mERRO\e[0m] Socket sem conteúdo!\n");
		return -1;
	}

	//httprint(texto_pag);
	printf("[\e[32mINFO\e[0m] Bytes recebidos: %ld\n", strlen(texto_pag));
	if( arq_saida!=NULL ){
		FILE *saida;
		if( strlen(texto_pag)>0 ){
			saida=fopen(arq_saida, "w+");
			fputs(texto_pag, saida);
			fclose(saida);
		}
	}
	free(buf);
	free(texto_pag);
	close(sockfd);
	putchar('\n');
	return 0;
}
