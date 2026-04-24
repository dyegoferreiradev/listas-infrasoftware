#include <stdio.h>        // Biblioteca padrão para entradas e saídas
#include <stdlib.h>       // Biblioteca para uso de alocação dinâmica
#include <pthread.h>      // Biblioteca para uso de threads

// Variáveis globais
int N;                   // Tamanho da matriz (NxN) *grade = matriz*
int **matriz;            // Ponteiro para a matriz NxN com alocação dinâmica                  
int *linhasValidas;      // Vetor que indica se cada linha é válida (1 = válida, 0 = inválida)
int *colunasValidas;     // Vetor que indica se cada coluna é válida (1 = válida, 0 = inválida)

// Função que valida uma linha da matriz
void *validaLinha(void *arg){
    int linha = *(int *)arg;           // Armazena o índice da linha a ser verificada
    int presente[N + 1];               // Vetor para verificar se os números de 1 a N já apareceram

    for (int i = 0; i <= N; i++){      // Inicializa todos os verificados como 0 (não apareceu)
        presente[i] = 0;
    }
    for (int i = 0; i < N; i++){
        int val = matriz[linha][i];                    // Pega o valor da célula na linha atual
        if (val < 1 || val > N || presente[val]) {     // Se o número for inválido ou já apareceu, a linha é inválida           
            linhasValidas[linha] = 0;
            return NULL;
        }
        presente[val] = 1;             // Verifica o número como presente
    }

    linhasValidas[linha] = 1;         // Após chegar aqui, a linha é válida
    return NULL;
}

// Função que valida uma coluna da matriz (equivalente à de linha)
void *validaColuna(void *arg){
    int coluna = *(int *)arg;
    int presente[N + 1];

    for (int i = 0; i <= N; i++)
        presente[i] = 0;

    for (int i = 0; i < N; i++) {
        int val = matriz[i][coluna];  
        if (val < 1 || val > N || presente[val]) {
            colunasValidas[coluna] = 0;
            return NULL;
        }
        presente[val] = 1;
    }

    colunasValidas[coluna] = 1;
    return NULL;
}
// MAIN
int main(){
    FILE *fp = fopen("entradaQ3.txt", "r");   // Abre o arquivo de entrada que contêm a matriz
    if (!fp){                                 // Verifica qualquer erro na abertura do arquivo
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    fscanf(fp, "%d", &N);                     // Faz a leitura do tamanho da matriz de tamanho (N)

    matriz = malloc(N * sizeof(int *));       // Faz a alocação dinâmica da matriz (NxN)
    for (int i = 0; i < N; i++){
        matriz[i] = malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) {
            fscanf(fp, "%d", &matriz[i][j]);  // Faz a leitura dos elementos da matriz
        }
    }
    fclose(fp);                               // Fecha o arquivo depois da leitura

    linhasValidas = calloc(N, sizeof(int));   // Aloca os vetores que irão indicar se cada linha/coluna é válida
    colunasValidas = calloc(N, sizeof(int));

    pthread_t threads_linhas[N], threads_colunas[N]; // Vetores que guardam as threads
    int *indices = malloc(N * sizeof(int));          // Vetor de índices a serem passados para as threads

    for (int i = 0; i < N; i++){
        indices[i] = i; // Define o índice atual
        pthread_create(&threads_linhas[i], NULL, validaLinha, &indices[i]); // Cria uma thread para verificar a linha i
        pthread_create(&threads_colunas[i], NULL, validaColuna, &indices[i]); // Cria uma thread para verificar a coluna i
    }
    
    for (int i = 0; i < N; i++){ // Aguarda todas as threads terminarem (linha e coluna)
        pthread_join(threads_linhas[i], NULL);
        pthread_join(threads_colunas[i], NULL);
    }

    int valido = 1; // É Assumido que o jogo é válido até que se prove o contrário

    for (int i = 0; i < N; i++){ // Verifica os resultados das threads
        if (linhasValidas[i] == 0 || colunasValidas[i] == 0){
            valido = 0; // Se qualquer linha ou coluna for inválida, marca o jogo como inválido
            break;
        }
    }

    if (valido){
        printf("O jogo é válido!\n");
    }
    else{
        printf("O jogo é inválido!\n");
    }
    
    for (int i = 0; i < N; i++){ // Libera toda a memória alocada utilizada
        free(matriz[i]);
    }
    free(matriz);
    free(linhasValidas);
    free(colunasValidas);
    free(indices);

    return 0;
}
