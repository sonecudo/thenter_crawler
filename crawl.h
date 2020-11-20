/*
 * Utilitarios para crawler
 */
#define OCHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ234567"

int link_na_lista(int indice, char *link, char lista[10000][63]){
	int flag=0;

	for(int k=0; flag==0 && k<indice; k++){
		if( strcmp(lista[k], link)==0 )
			flag=1;
	}

	return flag;
}
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
	char linha[2000], *titulo, *codigo_fonte;

	codigo_fonte=malloc(sizeof(char)*1);
	memset(codigo_fonte, 0, sizeof(char)*1);
	titulo=malloc(sizeof(char)*50);
	memset(titulo, 0, sizeof(char)*50);

	page=fopen(local_page, "r");
	if(page==NULL){
		fprintf(stderr, "[\e[31mERRO\e[0m] Arquivo vazio\n");
		exit(1);
	}else{
		while( fgets(linha, 2000,page) ){
			codigo_fonte=realloc(codigo_fonte, (size_t) strlen(linha)+strlen(codigo_fonte)+1 );
			strcat(codigo_fonte, linha);
		}
		for(int i=0; i<strlen(codigo_fonte); i++){
			if( strstr(codigo_fonte, "<title>")==codigo_fonte+i-7 || strstr(codigo_fonte, "<TITLE>")==codigo_fonte+i-7 ){
				for(int k=0; (codigo_fonte[i+k]!='<' || codigo_fonte[i+k+1]!='/') && k<50; k++){
					// enquanto estiver antes do </
					if( (isprint(codigo_fonte[i+k]) || isspace(codigo_fonte[i+k])) && codigo_fonte[i+k]!='\n' ){
						*(titulo+l)=codigo_fonte[i+k];
						l++;
					}
				}
				*(titulo+l)='\0';
				break;
			}else if( strstr(codigo_fonte, "Location: ")!=NULL ){
				//printf("[\e[32mINFO\e[0m] Header => \e[33m%s\e[0m\n", strstr(codigo_fonte, "Location: "));
				break;
			}
		}

		free(codigo_fonte);
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
	char linha[10000], link[63];
	int k=0, a=0;
	memset(linha, 0, sizeof(char)*10000);

	while( fgets(linha, 10000, site) ){
		for(int i=0; linha[i]!='\0' && linha[i+16]!='\0'; i++){

			//onion v2
			if( strstr(linha, ".onion")==linha+i+16 && onion_chars(linha, i, 2) && strchr(">/. \'\"\n", linha[i-1])!=NULL ){
				for(k=0; k<22; k++){
					link[k]=linha[i+k];
				}
				link[22]='\0';

				// Evita links duplicados na buffer
				if( !link_na_lista(a, link,urls) ){
					strcpy(urls[a], link);
					a++;
				}
			}

			//onion v3
			else if( strstr(linha, ".onion")==linha+i+56 && onion_chars(linha, i, 3) && strchr(">/. \'\"\n", linha[i-1])!=NULL ){
			
				for(k=0; k<62; k++){
					link[k]=linha[i+k];
				}
				link[62]='\0';

				// Evita links duplicados na buffer
				if( !link_na_lista(a, link,urls) ){
					strcpy(urls[a], link);
					a++;
				}
			}
		}
	}
	return 0;
}