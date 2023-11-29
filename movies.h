#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 4
// Estrutura que vai armazenar os filmes
typedef struct m {
    char key[6];
    char trueTitle[63];
    char brTitle[63];
    char director[31];
    char country[16];
    int rate;
    char year[5]; 
} movies;

///////////////////
typedef struct ind {
    char chave[6];
    int rrn;
} Indice;

typedef struct node {
    int rrn;
    Indice key[MAX];
    int size;
    int IS_LEAF;
    int ptr[MAX + 1];
    int prox;
} Node;

//Estrutura que armazena a árvore do índice secundário
typedef struct ititle {
    char key[6];
    char brTitle[100];
    struct ititle *prox;
} ititle;

////////////////////////////// STACK AUXILIAR //////////////////////////////////////////////////////////
struct equalTitle {
    char key[6];
};

// Defina o nó da pilha
struct Node {
    struct equalTitle* dataPtr;
    struct Node* next;
};

// Defina a estrutura da pilha
struct Stack {
    struct Node* top;
};

////////////////////////// FUNÇÕES USADAS NO GERAL ///////////////////////////////
////////////////////////// FUNCOES //////////////////////////]

// Carrega os registros secundários de um arquivo em memória.
void carregarItitleParaMemoria(FILE *itit, ititle **it);

// Refaz o índice secundário a partir do arquivo de índice primário e do arquivo de filmes.
void refazerIndiceSecundario(FILE *movies, ititle **it);

// Verifica a consistência de um arquivo de índice.
int verificarConsistencia(FILE *index);

// Gera uma chave com base nos dados de um filme.
void gerarChave(movies *movie);

// Adiciona um filme a um arquivo e aos índices primário e secundário.
void adicionarFilme(FILE *moviesArq, ititle **it);

// Arruma uma leitura de uma linha de 192 caracteres do arquivo de dados e printa ele formatado.
void printarESeparar(char *input);

// Altera a nota de um filme com base na chave.
void alterarNotaPelaChave(FILE *movies);

// Lista um filme com base em sua chave.
void listarFilmePorChave(FILE *movies);

// Lista filmes com base em seu título.
void listarFilmePorTitulo(FILE *movies, ititle *raiz2);

// Lista todos os filmes do arquivo.
void listarTodosOsFilmes(FILE *movies);

// Escreve os registros secundários em ordem.
void escreverEmOrdemSecundario(FILE *ititles, ititle *raiz);

// Função para inicializar a pilha
void inicializarStack(struct Stack* stack);

// Função para empilhar um elemento
void push(struct Stack* stack, struct equalTitle* dataPtr);

// Função para desempilhar um elemento
struct equalTitle* pop(struct Stack* stack);

// Função para verificar se a pilha está vazia
int isEmpty(struct Stack* stack);

// Insere um novo nó de título ordenadamente na lista encadeada.
void inserirOrdenado(ititle **lista, char key[6], char brTitle[100]);

// Remove um nó da lista encadeada com base na chave alfanumérica.
void removerPorChave(ititle **lista, char *key);

// Cria e retorna um novo nó de título com a chave alfanumérica e título informados.
ititle *criarNo2(char key[6], char brTitle[100]);

// Busca e retorna um nó na estrutura de dados com base na chave alfanumérica.
ititle *buscarPorChaveNoItitle(ititle *raiz,char *chave);

// Busca um título na estrutura de dados e armazena o caminho na pilha fornecida.
void buscarPorTitulo2Teste(ititle *raiz, struct Stack* stack, char *title);

/////////////////////// ARVORE B+///////////////////////////

//Função que le apenas um no da arvore
Node *lerNo(int rrn,FILE *arvore);

//Função que le e devolve a raiz da arvore
Node *lerESubirRaiz(FILE *arvore, FILE *headers);

//Função que acha o pai para o split de raizes
Node* findParent(Node* cursor, Node* child, FILE *arvore);

//Função que insere no nó interno
void insertInternal(Indice x, Node* cursor, Node* child);

//Função que insere na arvore B+
void insereArvore(Indice ind);

//Função para buscar o rrn apartir de uma chave
int buscarRrnPorChave(char *chave);