#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "./movies.c"


int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");
    ititle *it = NULL;
    FILE *movies,*ititle,*newMovieCompact;

    int opt;
    char moviesName[] = "./movies.dat", newMoviesName[] = "./tempFile.dat";

    // Abertura do arquivo de dados de filme
    movies = fopen(moviesName,"r+");
    if(movies == NULL) {
        movies = fopen(moviesName,"w+");  
    }

    newMovieCompact = fopen("./ibtree.idx", "r+");
    if(newMovieCompact == NULL) {
        newMovieCompact = fopen("./ibtree.idx", "w+");
        fprintf(newMovieCompact,"0#1#0#-1#.....@.....@.....@.....#-1@-1@-1@-1#-1@-1@-1@-1@-1******************************************");
    } 
    fclose(newMovieCompact);

    //Abertura / Carregamento / Recriação do arquivo de índice secundário
    ititle = fopen("./ititle.idx","r+");
    if(ititle == NULL) {
        ititle = fopen("./ititle.idx","w+");
        fprintf(ititle, "0#");
        refazerIndiceSecundario(movies,&it);
    } else {
        if(!verificarConsistencia(ititle)) {
            refazerIndiceSecundario(movies,&it);
        } else {
            fseek(ititle,0,SEEK_SET);
            carregarItitleParaMemoria(ititle,&it);
        } 
    }
    fclose(ititle);

    // Looping que gera o menu do sistema inteiro
    do {
        printf("|-------------------------------|\n");
        printf("|   Gerenciador de Filmes       |\n");
        printf("|-------------------------------|\n");
        printf("| 1 - Adicionar filme           |\n");
        printf("| 2 - Alterar nota pela chave   |\n");
        printf("| 3 - Listar filme pela chave   |\n");
        printf("| 4 - Listar filme pelo titulo  |\n");
        printf("| 5 - Listar todos os filmes    |\n");
        printf("|-------------------------------|\n");
        printf("Escolha uma opção (1-5) (* - quit):\n");
        printf("R: ");
        scanf("%d", &opt);
        switch (opt) {
            case 1:
                adicionarFilme(movies,&it);
                break;
            case 2:
                alterarNotaPelaChave(movies);
                break;
            case 3:
                listarFilmePorChave(movies);
                break;
            case 4:
                listarFilmePorTitulo(movies,it);
                break;
            case 5:
                printf("\n----- Todos os Filmes -----\n");
                listarTodosOsFilmes(movies);
                break;
            default:
                break;
        }
    } while(opt < 6 && opt > 0);
 
    // Gravação do arquivo de índice secundário atualizado
    ititle = NULL;
    ititle = fopen("./ititle.idx","w");
    fprintf(ititle,"1#");
    escreverEmOrdemSecundario(ititle,it);

    //fechamento dos arquivos
    fclose(ititle);
    fclose(movies);

    return 0;
}