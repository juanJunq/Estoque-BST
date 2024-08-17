#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char nome[50], dp[50];
    int ID;
    double price;
} produto_t;

typedef struct node
{
    produto_t product;  //Cada no da arovre é uma struct do tipo produto_t
    struct node *left;
    struct node *right;
} node_t;

int handleFILE(FILE **input, int argc, char **argv); //Cuida das operacoes de abertura e fechamento do arquivo

node_t *importFILE(FILE *input); //Importa os itens do arquivo para a arvore

node_t *createNode(produto_t product);

node_t *insertNode(node_t *tree, produto_t product); //Percore a arvore e procura um lugar para criar um no

int searchID(node_t *tree, int ID);

int searchDP(node_t *tree, char *DP, int *tag); //Procura o departamento desejado na arvore

int budget(node_t *tree, double max_price); // Dado uma entrada referente ao preco maximo, imprime todos os precos <= preco maximo

void InOrder(node_t *tree, produto_t *prices, int *n); //Travessia InOrder pela arvore

int compareQuickSort(const void *a, const void *b); //Retorna -1(a menor que b), 1,(a maior que b) e 0(a=b) para a funcao qsort(Quick Sort)

int countNodes(node_t *tree); //Conta quantos nos tem a arvore

void freeTree(node_t *tree); //Libera memoria alocada para os nos 

int menu(node_t *tree);

int main(int argc, char **argv)
{
    FILE *input_file = NULL;
    if (handleFILE(&input_file, argc, argv) < 0) //Verifica se a abertura do arquivo foi bem sucedida
        return EXIT_FAILURE;
    node_t *tree = importFILE(input_file); //Cria a arvore inicial
    menu(tree);
    fclose(input_file);
    freeTree(tree);
    return 0;
}

int handleFILE(FILE **input, int argc, char **argv)
{
    //Abre arquivo no modo de leiura
    *input = fopen(argv[1], "r");
    if (*input == NULL)
    {
        perror("Error");
        return 1;
    }
    return 0;
}

node_t *importFILE(FILE *input)
{
    //Move ponteiro do arquivo pro inicio do arquivo
    fseek(input, 0, SEEK_SET);
    char *buffer = (char *) malloc(sizeof(char)*128);
    //Inicializa um ponteiro da arvore
    node_t *tree = NULL; 
    produto_t product;
    while (fgets(buffer, 128, input) != NULL)
    {
        sscanf(buffer, "%d %s %s %lf", &product.ID, product.nome, product.dp, &product.price);
        product.nome[strcspn(product.nome, "\n")] = '\0'; //Remove o caracter de \n 
        product.dp[strcspn(product.dp, "\n")] = '\0'; //Remove o caracter de \n
        tree = insertNode(tree, product); //Insercao dos nos na arvore
    }
    free(buffer);
    return tree;
}

node_t *createNode(produto_t product)
{
    //Aloca memoria suficiente para o no e retorna ele mesmo
    node_t *Node = (node_t *)malloc(sizeof(node_t));
    Node->product = product;
    Node->left = NULL;
    Node->right = NULL;
    return Node;
}

node_t *insertNode(node_t *tree, produto_t product)
{
    if (tree == NULL)
        return createNode(product);
    if (product.ID < tree->product.ID)
        tree->left = insertNode(tree->left, product);
    else if (product.ID > tree->product.ID)
        tree->right = insertNode(tree->right, product);
    return tree;
}

int searchID(node_t *tree, int ID)
{
    if (tree == NULL) return 1;
    if (ID == tree->product.ID)
    {
        printf("(%s) %s - R$ %.2lf\n", tree->product.dp, tree->product.nome, tree->product.price);
        return 0;
    }
    //Vai imprimindo os IDs ate achar o ID desejado
    printf("(%s) %s - R$ %.2lf\n", tree->product.dp, tree->product.nome, tree->product.price);
    if (ID < tree->product.ID) return searchID(tree->left, ID);
    else return searchID(tree->right, ID);
    return 0;
}

int searchDP(node_t *tree, char *DP, int *tag)
{
    if (tree == NULL) return 0;
    //Procura o DP desejado usando logica de travessia InOrder: visitamos sempre a raiz depois de visitar o filho à esquerda e antes de visitar o filho à direita
    searchDP(tree->left, DP, tag);
    if (!strcmp(DP, tree->product.dp))
    {
        printf("(%s) %s - R$ %.2lf\n", tree->product.dp, tree->product.nome, tree->product.price);
        //Caso ache o DP, indicamos por *tag = 1
        *tag = 1;
    }
    searchDP(tree->right, DP, tag);
    return 0;
}

void insertProduct(node_t *tree)
{
    produto_t product;
    char *buffer = NULL;
    buffer = (char *) malloc(sizeof(char)*128);
    fgets(buffer, 128, stdin);
    buffer = (char *) realloc(buffer, sizeof(char) * (strlen(buffer)+1));
    sscanf(buffer, "%d %s %s %lf", &product.ID, product.nome, product.dp, &product.price);
    insertNode(tree, product);
    free(buffer);
}

void InOrder(node_t *tree, produto_t *prices, int *n)
{
    //Travessia InOrder coletando as structs referentes a cada no e passando para um vetor
    if(tree == NULL) return;
    InOrder(tree->left, prices, n);
    prices[*n] = tree->product;
    (*n)++;
    InOrder(tree->right, prices, n);
}

int countNodes(node_t *tree) {
    if (tree == NULL) return 0;
    //+1 para cada no contado
    return 1 + countNodes(tree->left) + countNodes(tree->right);
}

int compareQuickSort(const void *a, const void *b) {
    // Compare indica(retorna) para qsort a relacao entre os elementos a e b
    // -1	a e menor que b
    //  0	a e igual a b
    //  1	a e maior que b
    produto_t *prodA = (produto_t *)a;
    produto_t *prodB = (produto_t *)b;
    if(prodA->price < prodB->price) return -1;
    if(prodA->price > prodB->price) return 1;
    return 0;
}

int budget(node_t *tree, double max_price)
{
    int size = countNodes(tree), tag = 0;
    produto_t *prices = NULL;
    // Vetor "de precos" tem o mesmo tamanho da quantidade de nos na arvore(1 no = 1 struct)
    prices = (produto_t *) malloc(sizeof(produto_t)*size); 
    int n = 0;
    InOrder(tree, prices, &n);
    // qsort vai fazer a ordenacao dos elementos do vetor por um quick sort
    // 1. Tem-se um pivo
    // 2. Lista rearranjada para que todos elementos antes do pivo sejam menores que o pivo e todos elementos posteriores sejam maiores(particao)
    // 3. Recursivamente, ordena a sub lista dos elementos menores e a sub lista dos elementos maiores
    qsort(prices, n, sizeof(produto_t), compareQuickSort);
    for(int i = 0; i < size; i++)
    {
        if (prices[i].price <= max_price)
        {
            printf("(%s) %s - R$ %.2lf\n", prices[i].dp, prices[i].nome, prices[i].price);
            // Se tem produtos na faixa de preco especificada, marcamos tag = 1
            tag = 1;
        }
    }
    // Se nao achamos nenhum produto, tag nao e marcado(0)
    if(!tag) printf("Sem resultados para o filtro!\n");
    free(prices);
    return 0;
}


int menu(node_t *tree)
{
    puts("1 - Procurar por ID");
    puts("2 - Procurar por Departamento");
    puts("3 - Inserir Produto");
    puts("4 - Filtrar por Preco");
    puts("5 - Sair");
    int op, ID, tag;
    char *DP;
    double max_price;
    do
    {
        tag = 0;
        scanf("%d", &op);
        getchar();
        switch (op)
        {
        case 1:
            scanf("%d", &ID);
            if (searchID(tree, ID) == 1)
                printf("Produto nao encontrado!\n");
            break;
        case 2:
            DP = (char *)malloc(50 * sizeof(char));
            fgets(DP, 50, stdin);
            DP[strcspn(DP, "\n")] = '\0';
            DP = (char *) realloc(DP, sizeof(char)*(strlen(DP)+1));
            searchDP(tree, DP, &tag);
            if (!tag)
                printf("Departamento vazio!\n");
            free(DP);
            break;
        case 3:
            insertProduct(tree);
            break;
        case 4:
            scanf("%lf", &max_price);
            budget(tree, max_price);
            break;
        }
    } while (op != 5);
}

void freeTree(node_t *tree)
{
    if (tree != NULL)
    {
        //Deslocamento a esquerda e a direita para ir liberando todos os nos ate liberarmos a raiz
        freeTree(tree->left);
        freeTree(tree->right);
        free(tree);
    }
}
