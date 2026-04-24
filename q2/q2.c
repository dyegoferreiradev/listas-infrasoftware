#include <stdio.h>               // Biblioteca padrão para entradas e saídas
#include <stdlib.h>              // Biblioteca para uso de alocação dinâmica
#include <pthread.h>             // Biblioteca para uso de threads

// Constantes
#define capacidadeSala 3         // Número máximo de alunos simultâneos na sala (xN)
#define totalAlunosA 9           // Total de alunos do curso A (xN)
#define totalAlunosB 9           // Total de alunos do curso B (xN)

// Variáveis Globais
int ocupando = 0;                // Quantos alunos ocupam a sala agora
char cursoAtual = ' ';           // Qual curso está usando a sala no momento (A ou B)
int esperaA = 0;                 // Quantos alunos do curso A estão esperando
int esperaB = 0;                 // Quantos alunos do curso B estão esperando

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;          // Mutex para proteger acesso às variáveis compartilhadas
pthread_cond_t cond_A = PTHREAD_COND_INITIALIZER;           // Condição para alunos do curso A
pthread_cond_t cond_B = PTHREAD_COND_INITIALIZER;           // Condição para alunos do curso B

// Função para simular o tempo de leitura
void esperaSimples(){
    volatile int x;
    for (int i = 0; i < 10000000; i++){
        x = i * i;  // Ocupa tempo com cálculo inútil
    }
}

// Função para simular o uso da sala de leitura
void usarSala(char curso, int id){
    printf("Aluno %d do curso %c está lendo na sala.\n", id, curso);
    esperaSimples();  // Simula o tempo de leitura
}

// Função de execução para cada thread (aluno)
void *aluno(void *arg) {
    char curso = ((char *)arg)[0];         // Extrai o curso ('A' ou 'B') da string
    int id = atoi(&((char *)arg)[1]);      // Extrai o número do aluno da string
    pthread_mutex_lock(&mutex);            // Entra na região crítica

    if (curso == 'A') esperaA++; // Marca que este aluno está esperando
    else esperaB++;

    // Enquanto não for possível entrar na sala, a thread espera
    while ( ocupando == capacidadeSala || (cursoAtual != ' ' && cursoAtual != curso) ){ // Se a sala estiver cheia OU Se outro curso estiver na sala
        if (curso == 'A') pthread_cond_wait(&cond_A, &mutex); // Espera na variável de condição do seu curso
        else pthread_cond_wait(&cond_B, &mutex);
    }

    if (curso == 'A') esperaA--;    // Ao sair do laço, o aluno pode entrar
    else esperaB--;

    ocupando++;                     // Ocupa um assento na sala
    cursoAtual = curso;             // Define o curso atualmente presente na sala

    pthread_mutex_unlock(&mutex);   // Sai da região crítica
    usarSala(curso, id);            // Simula leitura na sala
    pthread_mutex_lock(&mutex);     // Volta para a região crítica para sair da sala
    ocupando--;                     // Libera um assento

    if (ocupando == 0){ // Se a sala ficar vazia, decide qual curso será liberado a seguir
        cursoAtual = ' ';  // Marca que a sala está vazia

        if (esperaA > esperaB && esperaA > 0){ // Se A tem mais alunos esperando, libera A. Senão, libera B. Se só A está esperando, libera A.
            pthread_cond_broadcast(&cond_A);   // Acorda todos os alunos do curso A
        } else if (esperaB > 0) {
            pthread_cond_broadcast(&cond_B);   // Acorda todos os alunos do curso B
        } else if (esperaA > 0) {
            pthread_cond_broadcast(&cond_A);   // Caso só A esteja esperando
        }
    }
    pthread_mutex_unlock(&mutex);  // Sai da região crítica
    return NULL;                   // Finaliza a thread do aluno
}

// MAIN
int main(){
    pthread_t threads[totalAlunosA + totalAlunosB];   // Vetor de threads para os alunos
    char *args[totalAlunosA + totalAlunosB];          // Vetor de argumentos para as threads

    for (int i = 0; i < totalAlunosA; i++){                 // Cria threads para os alunos do curso A
        args[i] = malloc(10);                               // Aloca memória para string do argumento
        sprintf(args[i], "A%d", i + 1);                     // Formata o argumento com curso + número
        pthread_create(&threads[i], NULL, aluno, args[i]);  // Cria a thread do aluno do curso A
    }
    
    for (int i = 0; i < totalAlunosB; i++){                 // Cria threads para os alunos do curso B (equivalente ao curso A)
        args[i + totalAlunosA] = malloc(10);                
        sprintf(args[i + totalAlunosA], "B%d", i + 1);             
        pthread_create(&threads[i + totalAlunosA], NULL, aluno, args[i + totalAlunosA]);
    }

    for (int i = 0; i < totalAlunosA + totalAlunosB; i++){  // Aguarda todas as threads terminarem e libera memória
        pthread_join(threads[i], NULL);                     
        free(args[i]);                                      
    }
    return 0;  
}
