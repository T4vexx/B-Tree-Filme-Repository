#include <stdio.h>
#include <string.h>
#include "./movies.h"
#define MAX 4
/////////////////////////////////// FUNCOES MOVIES //////////////////////////////////////


void carregarItitleParaMemoria(FILE *itit, ititle **it) {
    ititle *aux;
    fseek(itit, 0, SEEK_SET); 
    int flag;
    fscanf(itit, "%d", &flag);
    while (!feof(itit)) {
        char key[6],title[100];
        if (fscanf(itit, "#%[^@]@%5[^@]", title,key) == 2) {
            inserirOrdenado(it,key,title);
        }
    }
} 

void refazerIndiceSecundario(FILE *movies, ititle **it) {
    int rrn;
    char key[6];
    char title[70];
    *it = NULL;
    rrn = 0;

    fseek(movies, 0, SEEK_END);
    long tamanhoDoArquivo = ftell(movies);

    if (tamanhoDoArquivo == 0) {
        return;
    }

    fseek(movies, 0, SEEK_SET);
    do {
        fscanf(movies, "%5[^@]@%[^@]@", key,title);
        if(key[0] != '*' && key[1] != '|') {
            inserirOrdenado(it,key,title);
            printf("%s - %s  # ",title,key);
        }

        rrn++;
        fseek(movies, 192 * rrn, SEEK_SET);
    } while (ftell(movies) < tamanhoDoArquivo);
    return;
    
}

int verificarConsistencia(FILE *index) {
    int flag;

    fseek(index, 0, SEEK_END);
    if(ftell(index) == 0) {
        return 0;
    }
    fseek(index, 0, SEEK_SET);    
    fscanf(index, "%d", &flag);

    return flag;
}


void gerarChave(movies *movie) {
    char *last_comma = strrchr(movie->director, ',');

    if (last_comma != NULL) {
        last_comma += 2; 
        strncpy(movie->key, last_comma, 3);

        movie->key[3] = '\0';
    } else {
        memset(movie->key, ' ', 3);
        movie->key[3] = '\0'; 
    }
    strncpy(movie->key + 3, movie->year + strlen(movie->year) - 2, 2);
    movie->key[5] = '\0';
}

void adicionarFilme(FILE *moviesArq, ititle **it) {
    movies newMovie;
    int RRN;

    printf("Digite o título em português do filme:\n");
    scanf(" %62[^\n]",newMovie.brTitle);
    printf("Digite o título original do filme:\n");
    scanf(" %62[^\n]",newMovie.trueTitle);
    printf("Digite o nome do diretor do filme (ex: Welles, Orson):\n");
    scanf(" %30[^\n]",newMovie.director);
    printf("Digite o ano de lançamento:\n");
    scanf(" %4[^\n]",newMovie.year);
    printf("Digite o país de gravação do filme:\n");
    scanf(" %15[^\n]",newMovie.country);
    
    do {
        printf("Digite a nota do filme:\n");
        scanf(" %d",&newMovie.rate);
    }  while(newMovie.rate < 0 || newMovie.rate > 9);
    gerarChave(&newMovie);
    fseek(moviesArq,0,SEEK_END);
    long current_position = ftell(moviesArq);
    fprintf(moviesArq,"%s@%s@%s@%s@%s@%s@%d@",newMovie.key,newMovie.brTitle,newMovie.trueTitle,newMovie.director,newMovie.year,newMovie.country,newMovie.rate);
    long num_written_chars = ftell(moviesArq) - current_position;
    int padding_length = 192 - num_written_chars;
    for (int i = 0; i < padding_length; i++) {
        fprintf(moviesArq, "#");
    }

    RRN = (ftell(moviesArq) / 192) - 1;
    
    Indice ind;
    strcpy(ind.chave,newMovie.key);
    ind.rrn = RRN;
    insereArvore(ind);
    inserirOrdenado(it,newMovie.key,newMovie.brTitle);
    
}

void printarESeparar(char *input) {
    char cleanInput[200];
    int j = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] != '#') {
            cleanInput[j++] = input[i];
        }
    }
    cleanInput[j] = '\0'; 
    char chave[6];
    char tituloPT[63];
    char tituloOriginal[63];
    char diretor[31];
    int ano;
    char cidade[16];
    int nota;

    // Use sscanf para extrair os campos, tratando os caracteres especiais
    sscanf(cleanInput, "%6[^@]@%63[^@]@%63[^@]@%31[^@]@%d@%16[^@]@%d", chave, tituloPT, tituloOriginal, diretor, &ano, cidade, &nota);
    printf("\n----- filme: %s -----\n",tituloPT);
    printf("Chave: %s\n", chave);
    printf("Titulo em Portugues: %s\n", tituloPT);
    printf("Titulo Original: %s\n", tituloOriginal);
    printf("Diretor: %s\n", diretor);
    printf("Ano de Publicacao: %d\n", ano);
    printf("Pais de Gravacao: %s\n", cidade);
    printf("Nota: %d\n", nota);

}

void alterarNotaPelaChave(FILE *movies) {
    int rrnFilme;
    char input[193],key[6],cleanInput[200];
    int j = 0;

    printf("Digite a chave do filme: \n");
    scanf("%s",key);

    rrnFilme = buscarRrnPorChave(key);
    if(rrnFilme == -1) {
        printf("\nO Filme nao foi encontrado!\n");
        return;
    }

    fseek(movies,192 * rrnFilme, SEEK_SET);
    fgets(input,192,movies);
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] != '#') {
            cleanInput[j++] = input[i];
        }
    }
    cleanInput[j] = '\0'; 
    cleanInput[strlen(cleanInput)-2] = '\0';
    fseek(movies,(192 * rrnFilme) + strlen(cleanInput),SEEK_SET);
       
    do {
        printf("Digite a nova nota para o filme: \n");
        scanf(" %d", &j);
    } while(j < 0 || j > 9);
    
    fprintf(movies,"%d", j); 
}

void listarFilmePorChave(FILE *movies) {
    char key[6];
    int rrnFilme;
    char input[193];

    printf("Digite a chave do filme a ser buscada: \n");
    scanf("%s",key);

    rrnFilme = buscarRrnPorChave(key);
    if(rrnFilme == -1) {
        printf("\nO Filme nao foi encontrado!\n");
        return;
    }
    fseek(movies,192 * rrnFilme,SEEK_SET);
    fgets(input,192,movies);
    printarESeparar(input);
    printf("\n");
}

void listarFilmePorTitulo(FILE *movies, ititle *raiz2) {
    char titulo[100];
    char input[193];
    int rrnFilme;
    struct Stack stack;
    inicializarStack(&stack);

    printf("Digite o titulo do filme a ser buscada: \n");
    scanf(" %[^\n]",titulo);

    buscarPorTitulo2Teste(raiz2,&stack,titulo);

    
    if(isEmpty(&stack)) {
        printf("Nenhum filme encontrado com esse título!");
        return;
    }

    while (!isEmpty(&stack)) {
        struct equalTitle* item = pop(&stack);
        rrnFilme = buscarRrnPorChave(item->key);
        if(rrnFilme == -1) {
            printf("\nO Filme nao foi encontrado!\n");
            return;
        }
        fseek(movies,192 * rrnFilme,SEEK_SET);
        fgets(input,192,movies);
        printarESeparar(input);
        printf("\n");
        free(item);
    }
}

void buscarPorTitulo2Teste(ititle *raiz, struct Stack* stack, char *title) {
    ititle *atual = raiz;
    while (atual != NULL) {
        if(strcmp(atual->brTitle, title) == 0) {
            struct equalTitle* item1 = (struct equalTitle*)malloc(sizeof(struct equalTitle));
            strcpy(item1->key,atual->key);
            push(stack, item1);
        }
        atual = atual->prox;
    }
}

void listarTodosOsFilmes(FILE *movies) {
    char chave[2], input[200];
    FILE *arvore;
    Node *cursor;
    FILE *headers;
    arvore = fopen("./ibtree.idx","r+");
    headers = fopen("./headers.txt", "r+");
    strcpy(chave, "0");
    cursor = lerESubirRaiz(arvore,headers);
    while (cursor->IS_LEAF == 0) {
        int i;
        for (i = 0; i < cursor->size; i++) {
            if (strcmp(chave, cursor->key[i].chave) == 0) {
                return cursor->key[i].rrn;
            }
            if (strcmp(chave, cursor->key[i].chave) < 0) {
                cursor = lerNo(cursor->ptr[i],arvore);
                break;
            }
            if (i == cursor->size - 1) {
                cursor = lerNo(cursor->ptr[i + 1],arvore);
                break;
            }
        }
    }

    do {
        for(int i=0; i < cursor->size; i++) {
            if(cursor->key[i].rrn != -1) {
                fseek(movies,192 * cursor->key[i].rrn,SEEK_SET);
                fgets(input,192,movies);
                printarESeparar(input);
                printf("\n");
            }
        }
        
        cursor = lerNo(cursor->prox,arvore);
        if(cursor->rrn < 0 || cursor->rrn > 200) 
        return; 
        
    } while(cursor != NULL);
    

    return;
}

/////////////////////////////////// ÍNDICE SECUNDÁRIO LISTA ////////////////////////////////////////////////////////
// Função para criar um novo nó
ititle *criarNo2(char key[6], char brTitle[100]) {
    ititle *novoNo = (ititle *)malloc(sizeof(ititle));
    if (novoNo == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(EXIT_FAILURE);
    }
    strcpy(novoNo->key, key);
    strcpy(novoNo->brTitle, brTitle);
    novoNo->prox = NULL;
    return novoNo;
}

// Função para inserir um item ordenado na lista
void inserirOrdenado(ititle **lista, char key[6], char brTitle[100]) {
    ititle *novoNo = criarNo2(key, brTitle);

    // Se a lista estiver vazia ou o primeiro elemento for maior, insere no início
    if (*lista == NULL || strcmp((*lista)->brTitle, brTitle) > 0) {
        novoNo->prox = *lista;
        *lista = novoNo;
    } else {
        ititle *atual = *lista;

        // Encontra o ponto de inserção
        while (atual->prox != NULL && strcmp(atual->prox->brTitle, brTitle) <= 0) {
            atual = atual->prox;
        }

        // Insere o novo nó no ponto de inserção
        novoNo->prox = atual->prox;
        atual->prox = novoNo;
    }
}

// Função para remover um item da lista pela chave
void removerPorChave(ititle **lista, char *key) {
    if (*lista == NULL) {
        return; // Lista vazia, nada a fazer
    }

    ititle *atual = *lista;
    ititle *anterior = NULL;

    // Encontra o nó a ser removido
    while (atual != NULL && strcmp(atual->key, key) != 0) {
        anterior = atual;
        atual = atual->prox;
    }

    // Se o nó foi encontrado
    if (atual != NULL) {
        // Remove o nó da lista
        if (anterior == NULL) {
            *lista = atual->prox; // Remover o primeiro nó
        } else {
            anterior->prox = atual->prox;
        }
        free(atual);
    }
}

// Função que busca um item pela chave na lista de índice secundário
ititle *buscarPorChaveNoItitle(ititle *raiz, char *chave) {
    ititle *atual = raiz;
    while (atual != NULL) {
        if(strcmp(atual->key, chave) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
}

// Função para listar todos os elementos da lista no arquivo de índice secundário 
void escreverEmOrdemSecundario(FILE *ititles, ititle *raiz) {
    ititle *atual = raiz;
    while (atual != NULL) {
        fprintf(ititles,"%s@%s#",atual->brTitle, atual->key);
        atual = atual->prox;
    }
}

/////////////////////////////////// STACK AUXILIAR //////////////////////////////////////////////////////////////////
// Função para inicializar a pilha
void inicializarStack(struct Stack* stack) {
    stack->top = NULL;
}

// Função para empilhar um elemento
void push(struct Stack* stack, struct equalTitle* dataPtr) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        fprintf(stderr, "Falha na alocacao de memoria para o no da pilha.\n");
        exit(1);
    }

    newNode->dataPtr = dataPtr;
    newNode->next = stack->top;
    stack->top = newNode;
}

// Função para desempilhar um elemento
struct equalTitle* pop(struct Stack* stack) {
    if (stack->top == NULL) {
        fprintf(stderr, "A pilha esta vazia.\n");
        exit(1);
    }

    struct Node* poppedNode = stack->top;
    struct equalTitle* poppedData = poppedNode->dataPtr;
    stack->top = poppedNode->next;
    free(poppedNode);

    return poppedData;
}

// Função para verificar se a pilha está vazia
int isEmpty(struct Stack* stack) {
    return stack->top == NULL;
}
////////////////////// ARVORE B+ ///////////////////////////////////////
// Função que le um nó pelo rrn
Node *lerNo(int rrn,FILE *arvore) {
    Node *root;
    char buffer[101], cleanInput[101];
    fseek(arvore, 0, SEEK_END);
    if(100 * rrn > ftell(arvore) ) {
        return NULL;
    }
    fseek(arvore, 100 * rrn, SEEK_SET);
    
    size_t result = fread(buffer, 1, 100, arvore); // Lê 100 caracteres
    buffer[result] = '\0'; // Adiciona o caractere nulo no final
    int j = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] != '*') {
            cleanInput[j++] = buffer[i];
        }
    }
    cleanInput[j] = '\0'; 
    root = (Node *)malloc(sizeof(Node));
    sscanf(cleanInput,"%d#%d#%d#%d#%5[^@]@%5[^@]@%5[^@]@%5[^@]#%d@%d@%d@%d#%d@%d@%d@%d@%d",&(root->rrn),&(root->IS_LEAF),&(root->size),&(root->prox),root->key[0].chave,root->key[1].chave,root->key[2].chave,root->key[3].chave,&(root->key[0].rrn),&(root->key[1].rrn),&(root->key[2].rrn),&(root->key[3].rrn),&(root->ptr[0]),&(root->ptr[1]),&(root->ptr[2]),&(root->ptr[3]),&(root->ptr[4]));
    return root;
    
    
}

// Função que le a raiz e puxa para memória
Node *lerESubirRaiz(FILE *arvore, FILE *headers) {
    int rrnRaiz;
    Node *root;
    char buffer[101];
    fseek(headers, 0, SEEK_SET);
    fscanf(headers, "%d", &rrnRaiz);
    fseek(arvore, 100 * rrnRaiz, SEEK_SET);
    
    size_t result = fread(buffer, 1, 100, arvore); // Lê 100 caracteres
    buffer[result] = '\0'; // Adiciona o caractere nulo no final
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '*') {
            buffer[i] = '\0';
            break;
        }
    }
    root = (Node *)malloc(sizeof(Node));
    sscanf(buffer,"%d#%d#%d#%d#%5[^@]@%5[^@]@%5[^@]@%5[^@]#%d@%d@%d@%d#%d@%d@%d@%d@%d",&(root->rrn),&(root->IS_LEAF),&(root->size),&(root->prox),root->key[0].chave,root->key[1].chave,root->key[2].chave,root->key[3].chave,&(root->key[0].rrn),&(root->key[1].rrn),&(root->key[2].rrn),&(root->key[3].rrn),&(root->ptr[0]),&(root->ptr[1]),&(root->ptr[2]),&(root->ptr[3]),&(root->ptr[4]));
    
    return root;
}

// Função que acha o pai de um nó
Node* findParent(Node* cursor, Node* child, FILE *arvore) {
    Node* parent = NULL,*filho;

    // Encontra o pai usando a travessia em profundidade e ignora os nós folha, pois eles não podem ser pais
    // Também ignora o penúltimo nível porque nunca encontraremos o pai de um nó folha durante a inserção usando esta função
    filho = lerNo(cursor->ptr[0],arvore);
    if (cursor->IS_LEAF || filho->IS_LEAF) {
        return NULL;
    }

    for (int i = 0; i < cursor->size + 1; i++) {
        filho = lerNo(cursor->ptr[i],arvore);
        if (filho == child) {
            parent = cursor;
            return parent;
        } else {
            parent = findParent(cursor->ptr[i], child,arvore);
            if (parent != NULL) return parent;
        }
    }

    return parent;
}

// Função de inserção de um nó interna
void insertInternal(Indice x, Node* cursor, Node* child) {
    FILE *arvore;
    int conta;
    arvore = fopen("./ibtree.idx","r+");

    if (cursor->size < MAX) {
        // Se o cursor não estiver cheio
        // Encontre a posição correta para a nova chave
        int i = 0;
        while (strcmp(x.chave, cursor->key[i].chave) > 0 && i < cursor->size)
            i++;

        // Crie espaço para a nova chave
        int j;
        for (j = cursor->size; j > i; j--) {
            cursor->key[j] = cursor->key[j - 1];
        }

        // Crie espaço para o novo ponteiro
        for (j = cursor->size + 1; j > i + 1; j--) {
            cursor->ptr[j] = cursor->ptr[j - 1];
        }

        cursor->key[i] = x;
        cursor->size++;
        cursor->ptr[i + 1] = child->rrn;

        fseek(arvore,100 * cursor->rrn,SEEK_SET);
        fprintf(arvore,"%d#%d#%d#%d#%s@%s@%s@%s#%d@%d@%d@%d#%d@%d@%d@%d@%d",cursor->rrn,cursor->IS_LEAF,cursor->size,cursor->prox,cursor->key[0].chave,cursor->key[1].chave,cursor->key[2].chave,cursor->key[3].chave,cursor->key[0].rrn,cursor->key[1].rrn,cursor->key[2].rrn,cursor->key[3].rrn,cursor->ptr[0],cursor->ptr[1],cursor->ptr[2],cursor->ptr[3],cursor->ptr[4]);
        conta = ftell(arvore) - (100 * cursor->rrn);
        conta = 100 - conta;
        for (i = 0; i < conta; i++) {
            fprintf(arvore, "*");
        }

        fclose(arvore);
        // printf("Inserted key in an Internal node successfully\n");
    } else {
        // printf("Inserted key in an Internal node successfully\n");
        // printf("Overflow in internal node!\nSplitting internal node\n");

        // Se houver overflow em um nó interno
        // Crie um novo nó interno
        Node* newInternal = (Node*)malloc(sizeof(Node));
        for(int i = 0; i < (MAX + 1); i++) {
            newInternal->ptr[i] = -1;
        }
        newInternal->key[0].rrn = -1;
        newInternal->key[1].rrn = -1;
        newInternal->key[2].rrn = -1;
        newInternal->key[3].rrn = -1;
        strcpy(newInternal->key[0].chave,"....");
        strcpy(newInternal->key[1].chave,"....");
        strcpy(newInternal->key[2].chave,"....");
        strcpy(newInternal->key[3].chave,"....");
        newInternal->prox = -1;

        // Crie um nó interno virtual
        Indice virtualKey[MAX + 1];
        int virtualPtr[MAX + 2];
        int i;
        for (i = 0; i < MAX; i++) {
            virtualKey[i] = cursor->key[i];
        }
        for (i = 0; i < MAX + 1; i++) {
            virtualPtr[i] = cursor->ptr[i];
        }

        i = 0;
        int j;
        while (strcmp(x.chave, virtualKey[i].chave) > 0 && i < MAX)
            i++;

        // Crie espaço para a nova chave
        for (j = MAX + 1; j > i; j--) {
            virtualKey[j] = virtualKey[j - 1];
        }
        virtualKey[i] = x;

        // Crie espaço para o novo ponteiro
        for (j = MAX + 2; j > i + 1; j--) {
            virtualPtr[j] = virtualPtr[j - 1];
        }
        virtualPtr[i + 1] = child->rrn;

        newInternal->IS_LEAF = 0;

        // Divida o cursor em dois nós
        cursor->size = (MAX + 1) / 2;
        newInternal->size = MAX - (MAX + 1) / 2;

        int tam = 0;
        fseek(arvore, 0, SEEK_END);
        tam = ftell(arvore);
        tam = (tam / 100) ;

        // Atribua elementos e ponteiros ao novo nó
        for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++) {
            newInternal->key[i] = virtualKey[j];
        }
        for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++) {
            newInternal->ptr[i] = virtualPtr[j];
        }
        newInternal->rrn = tam;
        // ATualização do cursor
        int conta = 0;
        fseek(arvore,100 * cursor->rrn,SEEK_SET);
        fprintf(arvore,"%d#%d#%d#%d#%s@%s@%s@%s#%d@%d@%d@%d#%d@%d@%d@%d@%d",cursor->rrn,cursor->IS_LEAF,cursor->size,cursor->prox,cursor->key[0].chave,cursor->key[1].chave,cursor->key[2].chave,cursor->key[3].chave,cursor->key[0].rrn,cursor->key[1].rrn,cursor->key[2].rrn,cursor->key[3].rrn,cursor->ptr[0],cursor->ptr[1],cursor->ptr[2],cursor->ptr[3],cursor->ptr[4]);
        conta = ftell(arvore) - (100 * cursor->rrn);
        conta = 100 - conta;
        for (i = 0; i < conta; i++) {
            fprintf(arvore, "*");
        }

        // ATualização do newInternall
        fseek(arvore,100 * tam,SEEK_SET);
        fprintf(arvore,"%d#%d#%d#%d#%s@%s@%s@%s#%d@%d@%d@%d#%d@%d@%d@%d@%d",newInternal->rrn,newInternal->IS_LEAF,newInternal->size,newInternal->prox,newInternal->key[0].chave,newInternal->key[1].chave,newInternal->key[2].chave,newInternal->key[3].chave,newInternal->key[0].rrn,newInternal->key[1].rrn,newInternal->key[2].rrn,newInternal->key[3].rrn,newInternal->ptr[0],newInternal->ptr[1],newInternal->ptr[2],newInternal->ptr[3],newInternal->ptr[4]);
        while(ftell(arvore) != 100 * (tam + 1)) {
            fprintf(arvore,"*");
        }
        fclose(arvore);
        arvore = fopen("./ibtree.idx","r+");


        FILE *headers;
        Node *treeRoot;
        headers = fopen("./headers.txt", "r+");
        treeRoot = lerESubirRaiz(arvore,headers);
        if (cursor == treeRoot) {
            // Se o cursor for um nó raiz, criamos uma nova raiz
            Node* newRoot = (Node*)malloc(sizeof(Node));
             for(int i = 0; i < (MAX + 1); i++) {
                newRoot->ptr[i] = -1;
            }
            newRoot->key[0].rrn = -1;
            newRoot->key[1].rrn = -1;
            newRoot->key[2].rrn = -1;
            newRoot->key[3].rrn = -1;
            strcpy(newRoot->key[0].chave,"....");
            strcpy(newRoot->key[1].chave,"....");
            strcpy(newRoot->key[2].chave,"....");
            strcpy(newRoot->key[3].chave,"....");
            newRoot->prox = -1;

            newRoot->key[0] = cursor->key[cursor->size];
            newRoot->ptr[0] = cursor->rrn;
            newRoot->ptr[1] = newInternal->rrn;
            newRoot->IS_LEAF = 0;
            newRoot->size = 1;
            treeRoot = newRoot;

            fseek(arvore, 0, SEEK_END);
            tam = ftell(arvore);
            tam = tam / 100;
            newRoot->rrn = tam;

            fseek(headers,0,SEEK_SET);
            fprintf(headers,"%d",tam);
            fclose(headers);

            fseek(arvore,100 * tam,SEEK_SET);
            fprintf(arvore,"%d#%d#%d#%d#%s@%s@%s@%s#%d@%d@%d@%d#%d@%d@%d@%d@%d",newRoot->rrn,newRoot->IS_LEAF,newRoot->size,newRoot->prox,newRoot->key[0].chave,newRoot->key[1].chave,newRoot->key[2].chave,newRoot->key[3].chave,newRoot->key[0].rrn,newRoot->key[1].rrn,newRoot->key[2].rrn,newRoot->key[3].rrn,newRoot->ptr[0],newRoot->ptr[1],newRoot->ptr[2],newRoot->ptr[3],newRoot->ptr[4]);
            while(ftell(arvore) != 100 * (tam + 1)) {
                fprintf(arvore,"*");
            }
            fclose(arvore);
        } else {
            // Recursão
            // Encontre a profundidade primeiro para encontrar o pai do cursor
            insertInternal(cursor->key[cursor->size], findParent(treeRoot,cursor,arvore), newInternal);
        }
    }
}

// Função de inserção padrao
void insereArvore(Indice ind) {
    FILE *arvore;
    Node *root,*cursor,*parent;
    FILE *headers;
    arvore = fopen("./ibtree.idx","r+");
    headers = fopen("./headers.txt", "r+");
    
    root = lerESubirRaiz(arvore,headers);
    cursor = root;
    // Loop while: cursor viajará para o nó folha que possivelmente contém a chave
    while (cursor->IS_LEAF == 0) {
        parent = cursor;
        int i;
        for (i = 0; i < cursor->size; i++) {
            if (strcmp(ind.chave, cursor->key[i].chave) < 0) {
                cursor = lerNo(cursor->ptr[i],arvore);
                break;
            }
            if (i == cursor->size - 1) {
                cursor = lerNo(cursor->ptr[i + 1],arvore);
                break;
            }
        }
    }

    if(cursor->size < MAX) {
        int conta;
        int i=0;
        while (strcmp(ind.chave,cursor->key[i].chave) > 0 && i < cursor->size)
        i++;
        
        int j;
        for (j = cursor->size; j > i; j--) {
            cursor->key[j] = cursor->key[j - 1];
        }

        cursor->key[i] = ind;
        (cursor->size)++;

        int tam = 0;
        fseek(arvore, 0, SEEK_END);
        tam = ftell(arvore);
        tam = tam / 100;

        fseek(arvore,100 * cursor->rrn,SEEK_SET);
        fprintf(arvore,"%d#%d#%d#%d#%s@%s@%s@%s#%d@%d@%d@%d#%d@%d@%d@%d@%d",cursor->rrn,cursor->IS_LEAF,cursor->size,cursor->prox,cursor->key[0].chave,cursor->key[1].chave,cursor->key[2].chave,cursor->key[3].chave,cursor->key[0].rrn,cursor->key[1].rrn,cursor->key[2].rrn,cursor->key[3].rrn,cursor->ptr[0],cursor->ptr[1],cursor->ptr[2],cursor->ptr[3],cursor->ptr[4]);
        conta = ftell(arvore) - (100 * cursor->rrn);
        conta = 100 - conta;
        for (i = 0; i < conta; i++) {
            fprintf(arvore, "*");
        }
        fclose(arvore);
    } else if (cursor->size == MAX) {
        Node* newLeaf = (Node*)malloc(sizeof(Node));
        for(int i = 0; i < (MAX + 1); i++) {
            newLeaf->ptr[i] = -1;
        }
        newLeaf->key[0].rrn = -1;
        newLeaf->key[1].rrn = -1;
        newLeaf->key[2].rrn = -1;
        newLeaf->key[3].rrn = -1;
        strcpy(newLeaf->key[0].chave,"....");
        strcpy(newLeaf->key[1].chave,"....");
        strcpy(newLeaf->key[2].chave,"....");
        strcpy(newLeaf->key[3].chave,"....");
        newLeaf->prox = -1;

        Indice virtualNode[MAX + 1];
        int i; 
        for (i = 0; i < MAX; i++) {
            virtualNode[i] = cursor->key[i];
        }

        int j;
        i = 0;
        while (strcmp(ind.chave, virtualNode[i].chave) > 0 && i < MAX)
            i++;

        // Crie espaço para a nova chave
        for (j = MAX + 1; j > i; j--) {
            virtualNode[j] = virtualNode[j - 1];
        }

        virtualNode[i] = ind;
        newLeaf->IS_LEAF = 1;
        // Divida o cursor em dois nós folha
        cursor->size = (MAX + 1) / 2; ///////////////////////// resolver aqui
        newLeaf->size = MAX + 1 - (MAX + 1) / 2;

        int tam = 0;
        fseek(arvore, 0, SEEK_END);
        tam = ftell(arvore);
        tam = (tam / 100) ;

        for (i = 0; i < cursor->size; i++) {
            cursor->key[i] = virtualNode[i];
        }

        for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
            newLeaf->key[i] = virtualNode[j];
        }
        newLeaf->rrn = tam;
        cursor->prox = tam;

        // ATualização do cursor
        int conta = 0;
        fseek(arvore,100 * cursor->rrn,SEEK_SET);
        fprintf(arvore,"%d#%d#%d#%d#%s@%s@%s@%s#%d@%d@%d@%d#%d@%d@%d@%d@%d",cursor->rrn,cursor->IS_LEAF,cursor->size,cursor->prox,cursor->key[0].chave,cursor->key[1].chave,cursor->key[2].chave,cursor->key[3].chave,cursor->key[0].rrn,cursor->key[1].rrn,cursor->key[2].rrn,cursor->key[3].rrn,cursor->ptr[0],cursor->ptr[1],cursor->ptr[2],cursor->ptr[3],cursor->ptr[4]);
        conta = ftell(arvore) - (100 * cursor->rrn);
        conta = 100 - conta;
        for (i = 0; i < conta; i++) {
            fprintf(arvore, "*");
        }

        // ATualização do newLeaf
        fseek(arvore,100 * tam,SEEK_SET);
        fprintf(arvore,"%d#%d#%d#%d#%s@%s@%s@%s#%d@%d@%d@%d#%d@%d@%d@%d@%d",newLeaf->rrn,newLeaf->IS_LEAF,newLeaf->size,newLeaf->prox,newLeaf->key[0].chave,newLeaf->key[1].chave,newLeaf->key[2].chave,newLeaf->key[3].chave,newLeaf->key[0].rrn,newLeaf->key[1].rrn,newLeaf->key[2].rrn,newLeaf->key[3].rrn,newLeaf->ptr[0],newLeaf->ptr[1],newLeaf->ptr[2],newLeaf->ptr[3],newLeaf->ptr[4]);
        while(ftell(arvore) != 100 * (tam + 1)) {
            fprintf(arvore,"*");
        }

        fclose(arvore);
        arvore = fopen("./ibtree.idx","r+");

        if (cursor == root) {
            // Se o cursor for um nó raiz, criamos uma nova raiz
            Node* newRoot = (Node*)malloc(sizeof(Node));
            for(int i = 0; i < (MAX + 1); i++) {
                newRoot->ptr[i] = -1;
            }
            newRoot->key[0].rrn = -1;
            newRoot->key[1].rrn = -1;
            newRoot->key[2].rrn = -1;
            newRoot->key[3].rrn = -1;
            strcpy(newRoot->key[0].chave,"....");
            strcpy(newRoot->key[1].chave,"....");
            strcpy(newRoot->key[2].chave,"....");
            strcpy(newRoot->key[3].chave,"....");
            newRoot->prox = -1;

            newRoot->key[0] = newLeaf->key[0];
            newRoot->ptr[0] = cursor->rrn;
            newRoot->ptr[1] = newLeaf->rrn;
            newRoot->IS_LEAF = 0;
            newRoot->size = 1;
            root = newRoot;
        
            fseek(arvore, 0, SEEK_END);
            tam = ftell(arvore);
            tam = tam / 100;
            newRoot->rrn = tam;

            fseek(headers,0,SEEK_SET);
            fprintf(headers,"%d",tam);
            fclose(headers);

            fseek(arvore,100 * tam,SEEK_SET);
            fprintf(arvore,"%d#%d#%d#%d#%s@%s@%s@%s#%d@%d@%d@%d#%d@%d@%d@%d@%d",newRoot->rrn,newRoot->IS_LEAF,newRoot->size,newRoot->prox,newRoot->key[0].chave,newRoot->key[1].chave,newRoot->key[2].chave,newRoot->key[3].chave,newRoot->key[0].rrn,newRoot->key[1].rrn,newRoot->key[2].rrn,newRoot->key[3].rrn,newRoot->ptr[0],newRoot->ptr[1],newRoot->ptr[2],newRoot->ptr[3],newRoot->ptr[4]);
            while(ftell(arvore) != 100 * (tam + 1)) {
                fprintf(arvore,"*");
            }
            fclose(arvore);
        } else {
            insertInternal(newLeaf->key[0], parent, newLeaf);
        }
    }

}

// Função que busca um RRN pela chave e retorna
int buscarRrnPorChave(char *chave) {
    FILE *arvore;
    Node *cursor;
    FILE *headers;
    arvore = fopen("./ibtree.idx","r+");
    headers = fopen("./headers.txt", "r+");
    
    cursor = lerESubirRaiz(arvore,headers);
    while (cursor->IS_LEAF == 0) {
        int i;
        for (i = 0; i < cursor->size; i++) {
            if (strcmp(chave, cursor->key[i].chave) == 0) {
                return cursor->key[i].rrn;
            }
            if (strcmp(chave, cursor->key[i].chave) < 0) {
                cursor = lerNo(cursor->ptr[i],arvore);
                break;
            }
            if (i == cursor->size - 1) {
                cursor = lerNo(cursor->ptr[i + 1],arvore);
                break;
            }
        }
    }

    for(int i=0; i < 5; i++) {
        if(strcmp(chave, cursor->key[i].chave) == 0) {
            return cursor->key[i].rrn;
        }
    }

    return -1;
}