#include <stdio.h>                 // Biblioteca padrão para entradas e saídas
#include <pthread.h>               // Biblioteca para uso de threads
#include <unistd.h>                // Biblioteca para uso do usleep (função para setar os 500ms)

// Constantes
#define numTrens 10               // Número total de trens (threads)
#define numIntersec 5             // Número de interseções
#define tempo 500000              // 500ms em microssegundos

int contadorIntersec[numIntersec];                     // Quantos trens estão na interseção
pthread_mutex_t mutexIntersec[numIntersec];            // Mutex de cada interseção
pthread_cond_t condIntersec[numIntersec];              // Condição de espera de cada interseção

// Função de execução de cada trem (thread)
void *trem(void *arg){
    int id = *(int *)arg;

    while (1){
        for (int i = 0; i < numIntersec; i++){      // Acessar a interseção i
            pthread_mutex_lock(&mutexIntersec[i]);

            while (contadorIntersec[i] >= 2){       // Espera até que haja menos de 2 trens na interseção
                pthread_cond_wait(&condIntersec[i], &mutexIntersec[i]);
            }

            contadorIntersec[i]++; // Entra na interseção
            printf("Trem %d entrou na interseção %d (ocupada por %d)\n", id, i + 1, contadorIntersec[i]);
            pthread_mutex_unlock(&mutexIntersec[i]);

            usleep(tempo); // Simula a passagem por 500ms

            pthread_mutex_lock(&mutexIntersec[i]); // Sai da interseção
            contadorIntersec[i]--;
            printf("Trem %d saiu da interseção %d (agora com %d)\n", id, i + 1, contadorIntersec[i]);

            pthread_cond_signal(&condIntersec[i]); // Notifica um trem esperando na mesma interseção
            pthread_mutex_unlock(&mutexIntersec[i]);
        }
        // Depois de passar pela 5, volta para 1 (por estar dentro do for)
    }
    return NULL;
}

// MAIN
int main(){
    pthread_t trens[numTrens];    // Vetor de threads
    int ids[numTrens];            // ID's dos trens

    for (int i = 0; i < numIntersec; i++){ // Inicializa os recursos de sincronização
        contadorIntersec[i] = 0;
        pthread_mutex_init(&mutexIntersec[i], NULL);
        pthread_cond_init(&condIntersec[i], NULL);
    }

    for (int i = 0; i < numTrens; i++){ // Cria os trens (threads)
        ids[i] = i + 1;
        pthread_create(&trens[i], NULL, trem, &ids[i]);
    }

    for (int i = 0; i < numTrens; i++){ // Aguarda as threads (rodam infinitamente)
        pthread_join(trens[i], NULL);
    }
    return 0;
}
