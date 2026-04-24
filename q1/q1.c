#include <stdio.h>     // Biblioteca padrão para entradas e saídas
#include <pthread.h>   // Biblioteca para uso de threads

// Constantes
#define tamanhoMax 1000000
#define nThreads 10

// Variáveis globais
long contador = 0;      // contador compartilhado
int vencedor = 0;       // 1 quando já houver um vencedor
int idVencedora = -1;   // ID da thread vencedora

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Função de execução de cada n thread
void *competicao(void *threadid){
    int id = *(int *)threadid;
    while (1){
        pthread_mutex_lock(&mutex);
        if (contador >= tamanhoMax || vencedor){
            pthread_mutex_unlock(&mutex);
            break; // Encerra se já passou do tamanhoMax ou já se já tiver um vencedor
        }
        contador++;

        if (contador == tamanhoMax && !vencedor){
            vencedor = 1;            // Marca que tem um vencedor
            idVencedora = id;        // Armazena o ID da thread vencedora
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}
// MAIN
int main(){
    pthread_t threads[nThreads];
    int ids[nThreads];
    
    for (int i = 0; i < nThreads; i++){ // Criação das threads
        ids[i] = i + 1;                 // ID da thread
        pthread_create(&threads[i], NULL, competicao, &ids[i]);
    }
    
    for (int i = 0; i < nThreads; i++){ // Espera todas as threads terminarem
        pthread_join(threads[i], NULL);
    }
    
    printf("Valor final do contador: %ld\n", contador); // Mostra resultado final
    if (idVencedora != -1){
        printf("A thread vencedora foi a thread %d!\n", idVencedora);
    } else {
        printf("Nenhuma thread venceu.\n");
    }
    return 0;
}
