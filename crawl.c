/*
 * Utilitarios para crawler
 */
#define OCHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ234567"

void rm_nova_linha(char *entrada){
	if( entrada[ strlen(entrada)-1 ]=='\n')
		entrada[ strlen(entrada)-1 ]='\0';
}
int onion_chars(char *m, int i, int type){
	//se a string m estiver no formato de url do hidden service v2 ou v3 retorna 1, se não retorna 0
	int cont, flag=1;

	if(type==2){ //onion v2
		for(cont=0; cont<16; cont++){
			if( index(OCHARS, m[i+cont])!=NULL ){
			}else{
				flag=0;
			}
		}
	}
	if(type==3){ //onion v3
		for(cont=0; cont<56; cont++){
			if( index(OCHARS, m[i+cont])!=NULL ){
			}else{
				flag=0;
			}
		}
	}
	return flag;
}
char *pega_titulo(char *local_page){
	FILE *page;
	int l=0;
	char linha[200], *titulo;
	titulo=malloc(sizeof(char)*50);
	memset(titulo, 0, sizeof(char)*50);

	page=fopen(local_page, "r");
	if(page==NULL){
		fprintf(stderr, "[\e[31mERRO\e[0m] Arquivo vazio");
		exit(1);
	}else{
		while(fgets(linha, 200,page) ){
			for(int i=0; linha[i]!='\0'; i++){
				if( strstr(linha, "<title>")==linha+i-7 || strstr(linha, "<TITLE>")==linha+i-7 ){
					for(int k=0; (linha[i+k]!='<' || linha[i+k+1]!='/') && k<50; k++){
						// enquanto estiver antes do </...  title> e
						if( isalnum(linha[i+k]) || isspace(linha[i+k]) ){
							*(titulo+l)=linha[i+k];
							l++;
						}
					}
					*(titulo+l)='\0';
				}
			}
		}
		fclose(page);
	}

	if( strlen(titulo)==0 )
		strcpy(titulo, "sem nome");

	printf("[\e[32mINFO\e[0m] Título: \e[1;32m%s \e[0m\n", titulo);
	return titulo;
}
int registrado(char *link){
	// confere se o site já está registrado
	FILE *lista;
	int flag=0;
	char linha[100];

	lista = fopen("lista.txt", "r");
	while( fgets(linha, 100, lista) ){
		rm_nova_linha(linha);

		if( strcmp(link, linha)==0 ){
		//se o link for encontrado no arquivo com a lista de links
			flag=1;
			puts("[\e[32mINFO\e[0m] Site já registrado!");
			break;
		}
	}
	fclose(lista);
	return flag;
}
int salvar_link(char *link, char *status){
	FILE *lista;
	lista = fopen("lista.txt", "a+");

	puts("\n[\e[32mINFO\e[0m] Site novo!");

	fputs(status, lista);

	if( strcmp(status, "(on)") == 0 ){
		fputs( pega_titulo(".site.html"), lista);
		fputc('\n', lista);
		remove(".site.html");
	}else
		putc('\n', lista);

	fputs(link, lista);
	fputc('\n', lista);

	fclose(lista);
	return 0;
}
int filtrar_urls(FILE *site, char urls[][63]){
	char linha[10000];
	int k=0, a=0;
	memset(linha, 0, sizeof(char)*10000);

	while( fgets(linha, 10000, site) ){
		for(int i=0; linha[i]!='\0' && linha[i+16]!='\0'; i++){
			if( strstr(linha, ".onion")==linha+i+16 && onion_chars( linha, i, 2) ){
			//onion v2
				for(k=0; k<22; k++){
					urls[a][k]=linha[i+k];
				}
				urls[a][22]='\0';
				a++;
				// selecionar links desejáveis nessa parte
			}else if( strstr(linha, ".onion")==linha+i+56 && onion_chars( linha, i, 3) ){
			//onion v3
				for(k=0; k<62; k++){
					urls[a][k]=linha[i+k];
				}
				urls[a][62]='\0';
				a++;
				// selecionar links desejáveis nessa parte
			}
		}
	}
	return 0;
}
