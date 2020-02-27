/*
 * Utilitarios para crawler
 */

#define OCHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ234567"
#define CHAR_FILTER "áâãàçéêíóôõú\'\""

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
		for(cont=0; cont<56; cont++)
		{
			if( index(OCHARS, m[i+cont])!=NULL ){
			}else{
				flag=0;
			}
		}
	}
	return flag;
}
int site_online(char *link){
	//se o *link estiver online retorna 1 se estiver off retorna 0
	int res;

	printf("\n############################################\n");
	res=torsocks(link, NULL);

	if(res==0){
		printf("[\e[32mINFO\e[0m]~> Site \e[32monline\e[0m(%d): http://%s\n", res, link);
		return 1;
	}else{
		printf("[\e[32mINFO\e[0m]~> Site \e[33moffline\e[0m(%d): http://%s\e[0m\n", res, link);
		return 0;
	}
}
int pega_titulo(FILE *lista, char *link){
	FILE *page;
	char linha[200],titulo[50]="\0";
	int l=0,res;

	res=torsocks(link, ".titulo.html");

	if(res==0){
		page = fopen(".titulo.html", "r");
		while( fgets( linha, 200,page) ){
			for(int i=0; linha[i]!='\0'; i++){
				if( strstr(linha, "<title>")==linha+i-7 || strstr(linha, "<TITLE>")==linha+i-7 ){
					for(int k=0; (linha[i+k]!='<' || linha[i+k+1]!='/') && k<50; k++){
						// enquanto estiver antes do </...  title> e
						if( (linha[i+k]>=32 && linha[i+k]<=126)	/*ASCII printável*/
						 || strchr(CHAR_FILTER, linha[i+k])!=NULL/*CARACTERES PT*/ ){
							titulo[l]=linha[i+k];
							l++;
						}
					}
					titulo[l]='\0';
				}
			}
		}
		fclose(page);
		remove(".titulo.html");
	}

	if(titulo[0]=='\0'){
		fputs("sem nome", lista);
		fputc('\n', lista);
		strcpy(titulo, "sem nome");
	}else{
		fputs(titulo, lista);
		fputc('\n', lista);
	}
	printf("\e[1;32m%s \e[0m\n", titulo);
	return 0;
}
int registro( char *link ){
	// confere se o site já está registrado
	FILE *lista;
	int flag=1;
	char linha[100];

	lista = fopen("lista.txt", "r");
	while( fgets(linha, 100, lista) ){
		rm_nova_linha(linha);

		if( strcmp(link, linha)==0 ){
		//se o link for encontrado no arquivo com a lista de links
			flag=0;
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

	if( strcmp(status, "(on)") == 0 )
		pega_titulo(lista, link);
	else
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
