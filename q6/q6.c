#include <stdio.h>              // Biblioteca padrão para entradas e saídas
#include <pthread.h>            // Biblioteca para uso de threads

// Constantes
#define tamArray 10             // Tamanho do array banco de dados (BUFFER)
#define nLeitoras 5             // Número de threads leitoras (xN)
#define mEscritoras 4           // Número de threads escritoras (xM)

int bancoDados[tamArray];       // Array que simula o banco de dados compartilhado

// Contadores e Flags para o controle
int leitorasAtivas = 0;           // Quantidade de leitoras que estão lendo
int escritorasAtivas = 0;         // Flag = 1 se tiver uma escritora na região crítica, Flag = 0 caso contrário

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;        // Mutex para proteger o acesso às variáveis de controle
pthread_mutex_t acessoArray = PTHREAD_MUTEX_INITIALIZER;  // Mutex específico para proteger a região crítica em utilização (o array), apenas usada por escritoras
pthread_cond_t condLeitor = PTHREAD_COND_INITIALIZER;    // Threads leitoras esperam aqui se houver escritora ativa
pthread_cond_t condEscritor = PTHREAD_COND_INITIALIZER;  // Threads escritoras esperam aqui se houver escritora ou leitoras ativos

int valorEscrito = 1; // Valor que será escrito de forma sequencial no array pelas escritoras

// Função para simular tempo de espera
void esperaSimples(){
    volatile int x;
    for (int i = 0; i < 10000000; i++){
        x = i * i; // Ocupa tempo com cálculo inútil
    }
}

// Função de execução das threads leitoras
void *leitor(void *arg){
    int id = *(int *)arg; // ID da leitora

    while (1){
        pthread_mutex_lock(&mutex); // Entrada na região de controle
        while (escritorasAtivas){
            pthread_cond_wait(&condLeitor, &mutex); // Espera enquanto uma escritora estiver ativa
        }
        leitorasAtivas++; // Marca que mais uma leitora está ativa
        pthread_mutex_unlock(&mutex);

        printf("[LEITOR %d] lendo: ", id); // Região crítica: leitura do array que pode ser feita por várias leitoras ao mesmo tempo
        for (int i = 0; i < tamArray; i++){
            printf("%d ", bancoDados[i]);
        }
        printf("\n");

        esperaSimples(); // Simula tempo de leitura
        pthread_mutex_lock(&mutex); // Saída da região crítica de leitura
        leitorasAtivas--; // Leitora termina
        if (leitorasAtivas == 0){
            pthread_cond_signal(&condEscritor); // Se era a última leitora, acorda uma escritora
        }
        pthread_mutex_unlock(&mutex);

        esperaSimples(); // Simula tempo fora da região crítica
    }
    return NULL;
}

// Função de execução das threads escritoras
void *escritor(void *arg){
    int id = *(int *)arg; // ID da escritora

    while (1){
        pthread_mutex_lock(&mutex);  // Entrada na região de controle
        while (escritorasAtivas || leitorasAtivas > 0){
            pthread_cond_wait(&condEscritor, &mutex); // Espera enquanto houver escritoras ou leitoras ativas
        }
        escritorasAtivas = 1; // Marca que agora há uma escritora ativa
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&acessoArray);        // Região crítica: acesso exclusivo ao array
        int pos = valorEscrito % tamArray;       // Define posição de escrita
        bancoDados[pos] = valorEscrito++;        // Escreve valor e incrementa para próxima escrita
        printf("[ESCRITOR %d] escreveu %d na posição %d\n", id, bancoDados[pos], pos);
        esperaSimples();                         // Simula tempo de escrita
        pthread_mutex_unlock(&acessoArray);

        pthread_mutex_lock(&mutex); // Saída da escritora
        escritorasAtivas = 0;       // Libera a flag
        pthread_cond_broadcast(&condLeitor); // Acorda todas as leitoras que estavam esperando
        pthread_cond_signal(&condEscritor);  // Acorda uma escritora no caso de existir mais de uma esperando
        pthread_mutex_unlock(&mutex);

        esperaSimples(); // Simula tempo fora da região crítica
    }
    return NULL;
}

// MAIN
int main(){
    pthread_t leitoras[nLeitoras], escritoras[mEscritoras]; // Vetores para as threads
    int idsLeitoras[nLeitoras], idsEscritoras[mEscritoras]; // ID's

    for (int i = 0; i < tamArray; i++){ // Inicializa o array com zeros
        bancoDados[i] = 0;
    }
    
    for (int i = 0; i < nLeitoras; i++){ // Cria as threads leitoras
        idsLeitoras[i] = i + 1; // Atribui os ID's
        pthread_create(&leitoras[i], NULL, leitor, &idsLeitoras[i]);
    }
    
    for (int i = 0; i < mEscritoras; i++){ // Cria as threads escritoras
        idsEscritoras[i] = i + 1;
        pthread_create(&escritoras[i], NULL, escritor, &idsEscritoras[i]);
    }
    
    for (int i = 0; i < nLeitoras; i++){ // Aguarda as threads (laço infinito, nunca termina naturalmente)
        pthread_join(leitoras[i], NULL);
    }

    for (int i = 0; i < mEscritoras; i++){
        pthread_join(escritoras[i], NULL);
    }
    return 0;
}
