#include <stdio.h>                  // Biblioteca padrão para entradas e saídas
#include <stdlib.h>                 // Biblioteca com funções auxiliares para uso de arquivos
#include <pthread.h>                // Biblioteca para uso de threads
#include <string.h>                 // Biblioteca que fornece funções para manipulação de strings

//Constantes
#define maxCandidatos 100   // Limites para fins de cálculo
#define maxArquivos 100     

//Variáveis Globais
int N; // Número de arquivos
int T; // Número de threads
int C; // Número de candidatos

int votos[maxCandidatos];                 // Vetor de contagem de votos 
pthread_mutex_t mutexes[maxCandidatos];   // Um mutex por posição de voto

char arquivos[maxArquivos][20];           // Nomes dos arquivos no formato ("1.in", "2.in", ...)
int proxArquivo = 0;                      // Índice do próximo arquivo a ser processado
pthread_mutex_t mutex_arquivo = PTHREAD_MUTEX_INITIALIZER; // Mutex para controlar acesso a proxArquivo

// Função para pegar e contabilizar votos
void *contador(void *arg){
    int idx;

    while (1){
        pthread_mutex_lock(&mutex_arquivo); // Pega o próximo arquivo disponível com exclusão mútua
        if (proxArquivo >= N){
            pthread_mutex_unlock(&mutex_arquivo);
            break; // Todos os arquivos foram lidos
        }
        idx = proxArquivo++;
        pthread_mutex_unlock(&mutex_arquivo);

        FILE *fp = fopen(arquivos[idx], "r"); // Abre o arquivo
        if (!fp){
            fprintf(stderr, "Erro ao abrir arquivo %s\n", arquivos[idx]);
            continue;
        }

        int voto;
        while (fscanf(fp, "%d", &voto) == 1){
            if (voto < 0 || voto > C) continue;
            pthread_mutex_lock(&mutexes[voto]);   // Trava somente a posição que vai ser modificada
            votos[voto]++;
            pthread_mutex_unlock(&mutexes[voto]); // Destrava essa posição
        }
        fclose(fp);
    }
    return NULL;
}

// MAIN
int main(){
    printf("Digite o número de arquivos (N): "); // Entradas de N, T, C
    scanf("%d", &N);
    printf("Digite o número de threads (T <= N): ");
    scanf("%d", &T);
    printf("Digite o número de candidatos (C): ");
    scanf("%d", &C);

    for (int i = 0; i < N; i++){                // Inicializa os nomes dos arquivos (1.in até N.in)
        sprintf(arquivos[i], "%d.in", i + 1);
    }

    for (int i = 0; i <= C; i++){               // Inicializa mutexes dos candidatos
        votos[i] = 0;
        pthread_mutex_init(&mutexes[i], NULL);
    }

    pthread_t threads[T];                       // Cria threads
    for (int i = 0; i < T; i++){
        pthread_create(&threads[i], NULL, contador, NULL);
    }
    
    for (int i = 0; i < T; i++){                // Aguarda todas as threads terminarem
        pthread_join(threads[i], NULL);
    }

    int total = 0;                              // Calcula total de votos
    for (int i = 0; i <= C; i++){
        total += votos[i];
    }

    printf("\nResultado da eleição:\n");        // Exibe os resultados
    for (int i = 0; i <= C; i++){
        float perc = total > 0 ? (votos[i] * 100.0f / total) : 0;
        if (i == 0)
            printf("Branco: %d votos (%.2f%%)\n", votos[i], perc);
        else
            printf("Candidato %d: %d votos (%.2f%%)\n", i, votos[i], perc);
    }
    
    int vencedor = 1;                           // Determina o vencedor (ignora votos em branco)
    for (int i = 2; i <= C; i++) {
        if (votos[i] > votos[vencedor]) {
            vencedor = i;
        }
    }

    printf("\nVencedor: Candidato %d com %d votos.\n", vencedor, votos[vencedor]);

    for (int i = 0; i <= C; i++){              // Libera os mutexes
        pthread_mutex_destroy(&mutexes[i]);
    }
    pthread_mutex_destroy(&mutex_arquivo);
    return 0;
}
