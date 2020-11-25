/*
 *	Biblioteca para torproxy
 */
#include <string.h>

void trim(char *entrada){
	int flag=0, i=0;
	char c;

	//remove espaços no começo
	for(int k=0; entrada[k]!='\0'; k++){
		if(entrada[k]!=' ' || flag){
			flag=1;
			entrada[i++]=entrada[k];
		}
	}
	entrada[i]='\0';

	//inverte string
	for(int k=0; k<strlen(entrada)/2; k++){
		c=entrada[k];
		entrada[k]=entrada[strlen(entrada)-k-1];
		entrada[strlen(entrada)-k-1]=c;
	}

	//remove espaços no fim
	i=0;
	flag=0;
	for(int k=0; entrada[k]!='\0'; k++){
		if(entrada[k]!=' ' || flag){
			flag=1;
			entrada[i++]=entrada[k];
		}
	}
	entrada[i]='\0';

	//desinverte string
	for(int k=0; k<strlen(entrada)/2; k++){
		c=entrada[k];
		entrada[k]=entrada[strlen(entrada)-k-1];
		entrada[strlen(entrada)-k-1]=c;
	}
}
void print_data_int(char *entrada, int tam){
	for(int k=0; k<tam; k++)
		printf("%d ", entrada[k]);
	putchar('\n');
}
void print_data_chr(char *entrada, int tam){
	for(int k=0; k<tam; k++)
		putchar(entrada[k]);
	putchar('\n');
}
void httprint(char *entrada){
	for(int k=0; entrada[k]!='\0'; k++){
		if(entrada[k]=='\n')
			printf("\e[1;32m\\n\e[0m\n");
		else if(entrada[k]=='\r')
			printf("\e[1;32m\\r\e[0m");
		else
			putchar(entrada[k]);
	}
}
int ler_link(char *dominio, char *pag, char *orig){
	char *pont;
	strcpy(pag, "/\0");
	if( strstr(orig, ".onion")==NULL ){
		fprintf(stderr, "[\e[1;31mERRO\e[0m] Link incorreto\n");
		exit(1);
		return -1;
	}else{
		printf("[\e[32mINFO\e[0m] Filtrando link...\n");

		if( strncmp(orig ,"http://", 7)==0 ){
			orig=orig+7;
			pont=strchr(orig, '/');
			if(pont!=NULL){
				strncpy(dominio, orig, pont-orig);
			}else{
				strcpy(dominio, orig);
			}
		}else{
			pont=strchr(orig, '/');
			if(pont!=NULL){
				strncpy(dominio, orig, pont-orig);
			}else{
				strcpy(dominio, orig);
			}
		}
		printf("[\e[32mINFO\e[0m] Domínio: %s\n", dominio);
		if(pont!=NULL){
			printf("[\e[32mINFO\e[0m] Diretório: %s\n", pont);
			strcpy(pag, pont);
		}
	}
	return 0;
}
void atualiza(FILE *file_stream){
	long int pos;

	pos=ftell(file_stream);
	rewind(file_stream);
	fseek(file_stream, pos, SEEK_SET);
}