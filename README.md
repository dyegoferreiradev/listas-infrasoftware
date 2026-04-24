# 🧵 Programação Concorrente em C: Lista de Threads

Este repositório contém as resoluções de uma lista de problemas clássicos e práticos de **Sistemas Operacionais e Programação Concorrente**, desenvolvidos inteiramente em linguagem **C** utilizando a API POSIX Threads (`pthreads`). 

O foco deste projeto é a implementação de soluções seguras, eficientes e livres de *deadlocks* ou condições de corrida, aplicando conceitos avançados de sincronização como **Mutexes** e **Variáveis de Condição** em ambiente **Linux**.

## 🛠️ Tecnologias e Ferramentas
* **Linguagem:** C
* **Biblioteca:** `pthreads` (POSIX Threads)
* **Ambiente:** Linux
* **Conceitos Aplicados:** Concorrência, Sincronização, Exclusão Mútua (Mutex), Variáveis de Condição (Condition Variables), Prevenção de Starvation, Granularidade Fina de Locks.

## 📋 Sobre as Questões Resolvidas

Abaixo está o resumo dos 6 problemas de concorrência resolvidos neste repositório:

### 1. Corrida de Threads
Implementação de uma competição simples entre 10 threads, onde cada uma conta iterativamente até 1.000.000. O processo é finalizado assim que a primeira thread atinge o objetivo, declarando a vencedora.

### 2. Controle de Biblioteca Compartilhada
Simulação de uma sala de estudos com assentos limitados (N) disputada por dois cursos diferentes. 
* **Regra de Negócio:** Apenas alunos do mesmo curso podem estar na sala simultaneamente.
* **Sincronização:** Controle rigoroso com Mutexes e Variáveis de Condição para evitar mistura de alunos e garantir que nenhum curso monopolize a sala indefinidamente (prevenção contra *starvation*).

### 3. Validador Concorrente de Sudoku NxN
Um validador de grade de Sudoku simplificado que lê matrizes através de arquivos de texto (`.txt`). As threads verificam de forma paralela se cada linha e cada coluna contêm todos os números de 1 a N exatamente uma vez, emitindo o veredito da validade do jogo ao final.

### 4. Apuração Dinâmica de Votos Eletrônicos
Sistema de contagem de votos baseado em arquivos de texto (N arquivos, T threads, C candidatos).
* **Diferencial Técnico:** Implementação de alocação de carga de trabalho sob demanda (threads ociosas puxam arquivos pendentes dinamicamente) e uso de **granularidade fina de locks** (um *mutex* específico para cada candidato no array de votos), maximizando o paralelismo na escrita.

### 5. Simulação de Malha Ferroviária
Controle de tráfego de 10 trens circulando em loop contínuo por 5 interseções.
* **Regra de Negócio:** Cada interseção suporta no máximo 2 trens simultâneos.
* **Sincronização:** Trens aguardando são notificados via variáveis de condição apenas quando uma vaga na interseção específica é liberada, evitando bloqueios desnecessários (*busy waiting*) no sistema.

### 6. Problema Clássico dos Leitores e Escritores
Implementação robusta de um SGBD (Sistema Gerenciador de Banco de Dados) simulado em um Array.
* **Regras:** Múltiplas threads leitoras podem acessar os dados simultaneamente. Threads escritoras, no entanto, exigem acesso exclusivo (bloqueando tanto leitores quanto outros escritores).

## 🚀 Compilar e Executar

**OBS:** Como os códigos utilizam a biblioteca `pthreads`, é obrigatório adicionar a flag `-pthread` durante a compilação com o GCC no Linux.
