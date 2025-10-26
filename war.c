// ============================================================================
//               PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.
#define MAX_TERRITORIOS 10
#define MAX_MISSOES 3
#define TAM_NOME 50
#define COR_JOGADOR "AZUL"
#define COR_INIMIGO_ALVO "PRETO" // Inimigo específico para a missão de destruição

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.
typedef struct {
    char nome[TAM_NOME];
    char corDono[TAM_NOME];
    int tropas;
} Territorio;

// --- Protótipos das Funções ---
// Declarações antecipadas de todas as funções que serão usadas no programa, organizadas por categoria.
// Funções de setup e gerenciamento de memória:
Territorio* alocarMapa(int numTerritorios);
void inicializarTerritorios(Territorio* mapa, int numTerritorios);
void liberarMemoria(Territorio* mapa);
char* obterNomeCor(int idCor);

// Funções de interface com o usuário:
void exibirMenuPrincipal();
void exibirMapa(const Territorio* mapa, int numTerritorios); // const correctness
void exibirMissao(int idMissao);
void pausarTela();

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio* mapa, int numTerritorios, const char* corJogador);
void simularAtaque(Territorio* atacante, Territorio* defensor, const char* corJogador);
int sortearMissao();
int rolarDado();
int verificarVitoria(const Territorio* mapa, int numTerritorios, int idMissao, const char* corJogador); // const correctness

// Função utilitária:
void limparBufferEntrada();

// --- Função Principal (main) ---
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.
int main() {
    // Variáveis de Jogo
    Territorio* mapa = NULL;
    int escolha = -1;
    int idMissaoJogador = -1;
    int jogoVencido = 0;

    // 1. Configuração Inicial (Setup):
    // - Define o locale para português.
    setlocale(LC_ALL, "Portuguese");
    // - Inicializa a semente para geração de números aleatórios com base no tempo atual.
    srand(time(NULL));

    // - Aloca a memória para o mapa do mundo e verifica se a alocação foi bem-sucedida.
    mapa = alocarMapa(MAX_TERRITORIOS);
    if (mapa == NULL) {
        fprintf(stderr, "Falha ao alocar memória. Encerrando.\n");
        return 1;
    }

    // - Preenche os territórios com seus dados iniciais (tropas, donos, etc.).
    inicializarTerritorios(mapa, MAX_TERRITORIOS);

    // - Define a cor do jogador e sorteia sua missão secreta.
    idMissaoJogador = sortearMissao();

    printf("\n============================================\n");
    printf("        WAR ESTRUTURADO - INÍCIO DO JOGO\n");
    printf("============================================\n");
    printf("Sua cor de exército é: %s\n", COR_JOGADOR);
    exibirMissao(idMissaoJogador);

    // 2. Laço Principal do Jogo (Game Loop):
    // - Roda em um loop 'do-while' que continua até o jogador sair (opção 0) ou vencer.
    do {
        printf("\n--- RODADA ATUAL ---\n");
        // - A cada iteração, exibe o mapa, a missão e o menu de ações.
        exibirMapa(mapa, MAX_TERRITORIOS);
        exibirMenuPrincipal();

        printf("Escolha sua ação (0-2): ");
        // - Lê a escolha do jogador e usa um 'switch' para chamar a função apropriada:
        if (scanf("%d", &escolha) != 1) {
            limparBufferEntrada();
            escolha = -1;
        }
        limparBufferEntrada();

        switch (escolha) {
            case 1:
                // - Opção 1: Inicia a fase de ataque.
                faseDeAtaque(mapa, MAX_TERRITORIOS, COR_JOGADOR);
                break;
            case 2:
                // - Opção 2: Verifica se a condição de vitória foi alcançada e informa o jogador.
                jogoVencido = verificarVitoria(mapa, MAX_TERRITORIOS, idMissaoJogador, COR_JOGADOR);
                if (jogoVencido) {
                    printf("\n*** PARABÉNS! MISSÃO CUMPRIDA! VOCÊ VENCEU! ***\n");
                } else {
                    printf("\n--- Missão ainda não concluída. Continue lutando. ---\n");
                }
                break;
            case 0:
                // - Opção 0: Encerra o jogo.
                printf("\nEncerrando o jogo...\n");
                break;
            default:
                printf("\nOpção inválida. Tente novamente.\n");
                break;
        }

        if (jogoVencido && escolha != 0) {
            escolha = 0; // Sai do loop após vitória
        } else if (escolha != 0) {
            // - Pausa a execução para que o jogador possa ler os resultados antes da próxima rodada.
            pausarTela();
        }

    } while (escolha != 0);

    // 3. Limpeza:
    // - Ao final do jogo, libera a memória alocada para o mapa para evitar vazamentos de memória.
    liberarMemoria(mapa);
    printf("Memória liberada. Fim do Jogo.\n");

    return 0;
}

// --- Implementação das Funções ---

// alocarMapa():
// Aloca dinamicamente a memória para o vetor de territórios usando calloc.
// Retorna um ponteiro para a memória alocada ou NULL em caso de falha.
Territorio* alocarMapa(int numTerritorios) {
    Territorio* mapa = (Territorio*)calloc(numTerritorios, sizeof(Territorio));
    return mapa;
}

// inicializarTerritorios():
// Preenche os dados iniciais de cada território no mapa (nome, cor do exército, número de tropas).
// Esta função modifica o mapa passado por referência (ponteiro).
void inicializarTerritorios(Territorio* mapa, int numTerritorios) {
    if (mapa == NULL) return;

    for (int i = 0; i < numTerritorios; i++) {
        sprintf(mapa[i].nome, "Territorio %d", i + 1);

        if (i < 4) {
            strcpy(mapa[i].corDono, COR_JOGADOR);
            mapa[i].tropas = 3 + (rand() % 2);
        } else if (i == 4) {
            strcpy(mapa[i].corDono, COR_INIMIGO_ALVO);
            mapa[i].tropas = 4 + (rand() % 3);
        } else {
            strcpy(mapa[i].corDono, obterNomeCor(rand() % 2)); // Outras cores inimigas
            mapa[i].tropas = 2 + (rand() % 2);
        }
    }
}

// liberarMemoria():
// Libera a memória previamente alocada para o mapa usando free.
void liberarMemoria(Territorio* mapa) {
    if (mapa != NULL) {
        free(mapa);
    }
}

// exibirMenuPrincipal():
// Imprime na tela o menu de ações disponíveis para o jogador.
void exibirMenuPrincipal() {
    printf("\n--- MENU DE AÇÕES ---\n");
    printf("1. Atacar\n");
    printf("2. Verificar Missão\n");
    printf("0. Sair do Jogo\n");
    printf("---------------------\n");
}

// exibirMapa():
// Mostra o estado atual de todos os territórios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a função apenas leia os dados do mapa, sem modificá-los.
void exibirMapa(const Territorio* mapa, int numTerritorios) {
    printf("\n================ MAPA DO MUNDO ================\n");
    printf("| ID | Nome              | Dono     | Tropas |\n");
    printf("|----|-------------------|----------|--------|\n");
    for (int i = 0; i < numTerritorios; i++) {
        printf("| %-2d | %-17s | %-8s | %-6d |\n",
               i + 1, mapa[i].nome, mapa[i].corDono, mapa[i].tropas);
    }
    printf("===============================================\n");
}

// exibirMissao():
// Exibe a descrição da missão atual do jogador com base no ID da missão sorteada.
void exibirMissao(int idMissao) {
    printf("\n--- MISSÃO SECRETA ---\n");
    switch (idMissao) {
        case 1:
            printf("ID %d: Conquistar 6 territórios no total.\n", idMissao);
            break;
        case 2:
            printf("ID %d: Destruir completamente o Exército %s.\n", idMissao, COR_INIMIGO_ALVO);
            break;
        case 3:
            printf("ID %d: Conquistar 8 territórios no total.\n", idMissao);
            break;
        default:
            printf("Missão Desconhecida.\n");
            break;
    }
    printf("----------------------\n");
}

// faseDeAtaque():
// Gerencia a interface para a ação de ataque, solicitando ao jogador os territórios de origem e destino.
// Chama a função simularAtaque() para executar a lógica da batalha.
void faseDeAtaque(Territorio* mapa, int numTerritorios, const char* corJogador) {
    int idAtacante, idDefensor;
    
    printf("\n*** FASE DE ATAQUE ***\n");
    printf("Selecione os territórios pelo ID (1 a %d).\n", numTerritorios);

    printf("ID do Território Atacante (Seu): ");
    if (scanf("%d", &idAtacante) != 1) return;
    limparBufferEntrada();

    if (idAtacante < 1 || idAtacante > numTerritorios) {
        printf("ID de atacante inválido.\n");
        return;
    }
    Territorio* atacante = &mapa[idAtacante - 1]; 

    if (strcmp(atacante->corDono, corJogador) != 0 || atacante->tropas < 2) {
        printf("Ataque não permitido (Não é seu ou tem menos de 2 tropas).\n");
        return;
    }

    printf("ID do Território Defensor (Inimigo): ");
    if (scanf("%d", &idDefensor) != 1) return;
    limparBufferEntrada();

    if (idDefensor < 1 || idDefensor > numTerritorios || idDefensor == idAtacante) {
        printf("ID de defensor inválido ou igual ao atacante.\n");
        return;
    }
    Territorio* defensor = &mapa[idDefensor - 1];

    if (strcmp(defensor->corDono, corJogador) == 0) {
        printf("Você não pode atacar seu próprio território!\n");
        return;
    }

    printf("\n--- BATALHA INICIADA: %s (%d tropas) vs %s (%d tropas) ---\n",
           atacante->nome, atacante->tropas, defensor->nome, defensor->tropas);
           
    simularAtaque(atacante, defensor, corJogador);
}

// simularAtaque():
// Executa a lógica de uma batalha entre dois territórios.
// Realiza validações, rola os dados, compara os resultados e atualiza o número de tropas.
// Se um território for conquistado, atualiza seu dono e move uma tropa.
void simularAtaque(Territorio* atacante, Territorio* defensor, const char* corJogador) {
    // Regra: Rola 1 dado simples para cada lado
    int resultadoAtacante = rolarDado();
    int resultadoDefensor = rolarDado();

    printf("Dados Rolados: Atacante (%d) vs Defensor (%d)\n", resultadoAtacante, resultadoDefensor);

    if (resultadoAtacante > resultadoDefensor) {
        defensor->tropas--;
        printf("Atacante venceu o confronto. Defensor perdeu 1 tropa.\n");
    } else {
        atacante->tropas--;
        printf("Defensor venceu o confronto. Atacante perdeu 1 tropa.\n");
    }

    // Verifica conquista do território
    if (defensor->tropas <= 0) {
        printf("\n*** CONQUISTA! O TERRITÓRIO %s FOI CONQUISTADO! ***\n", defensor->nome);
        // Atualiza seu dono
        strcpy(defensor->corDono, corJogador);
        // Move uma tropa
        atacante->tropas--; 
        defensor->tropas = 1; // 1 tropa fica no território recém-conquistado
    }
}

// sortearMissao():
// Sorteia e retorna um ID de missão aleatório para o jogador.
int sortearMissao() {
    return 1 + (rand() % MAX_MISSOES);
}

// rolarDado():
int rolarDado() {
    return 1 + (rand() % 6);
}

// verificarVitoria():
// Verifica se o jogador cumpriu os requisitos de sua missão atual.
// Implementa a lógica para cada tipo de missão (destruir um exército ou conquistar um número de territórios).
// Retorna 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.
int verificarVitoria(const Territorio* mapa, int numTerritorios, int idMissao, const char* corJogador) {
    int territoriosJogador = 0;
    int inimigoAlvoVivo = 0; 

    for (int i = 0; i < numTerritorios; i++) {
        if (strcmp(mapa[i].corDono, corJogador) == 0) {
            territoriosJogador++;
        }
        // Verifica se o exército alvo (PRETO) ainda existe
        if (strcmp(mapa[i].corDono, COR_INIMIGO_ALVO) == 0) {
            inimigoAlvoVivo = 1; 
        }
    }

    printf("Status atual: %d territórios conquistados.\n", territoriosJogador);

    switch (idMissao) {
        case 1:
            // Conquistar 6 territórios
            return (territoriosJogador >= 6);
        case 2:
            // Destruir completamente o Exército PRETO
            return (inimigoAlvoVivo == 0);
        case 3:
            // Conquistar 8 territórios
            return (territoriosJogador >= 8);
        default:
            return 0;
    }
}

// limparBufferEntrada():
// Função utilitária para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// pausarTela():
void pausarTela() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

// obterNomeCor():
char* obterNomeCor(int idCor) {
    if (idCor == 0) return "AMARELO";
    if (idCor == 1) return "VERDE";
    return COR_INIMIGO_ALVO; 
}