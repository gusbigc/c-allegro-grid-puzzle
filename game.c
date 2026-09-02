//Meu trabalho 3 de laboratoria de programacao
//ao decorrer do codigo irei comentando ele
//comando pra executar o jogo: gcc game.c janela.c animacao.c -o l1-t3-gustavo -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_image


//bibliotecas (janela.h e animacao.h foram disponibilizadas pelo prof)
#include "janela.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "animacao.h"
#include <math.h>
//defines
#define LINHAS 5 //numero de linhas do tab
#define COLUNAS 6 //numero de colunas do tab
#define MAX_CORES 8 //max de cores que esse jogo ira suportar
#define TAM_NOME 50 //tamanho do nome do player
#define TEMPO_MAXIMO_FASE 200.0 //tempo de casa fase do jogo
#define FASES 5 //numero de fases
#define BONUS_FASE(n) ((n) * 50) //macro pra calcular o bonus de transicao de fase


//definicao de cores
const cor_t vermelho = {1, 0, 0, 1};
const cor_t verde    = {0, 1, 0, 1};
const cor_t azul     = {0, 0, 1, 1};
const cor_t amarelo  = {1, 1, 0, 1};
const cor_t roxo     = {1, 0, 1, 1};
const cor_t ciano    = {0, 1, 1, 1};
const cor_t laranja  = {1, 0.5, 0, 1};
const cor_t rosa     = {1, 0.6, 0.8, 1};
const cor_t preto    = {0, 0, 0, 1};
const cor_t branco   = {1, 1, 1, 1};
const cor_t cinza    = {0.5, 0.5, 0.5, 1};




//structs
typedef struct { //essa struct representa uma fase do jogo
    int numero; //indice pro numero da fase
    int cores; //qantas cores aparecem na fase atual
    int removidas_necessarias; //meta de remocao 
} fase;

typedef struct{ //registro de um jogador
    char nome[TAM_NOME];
    int pontuacao;
    int fase;
} Recorde;

typedef struct{ //estado completo de uma partida em andamento
    int tabuleiro[LINHAS][COLUNAS]; //matriz de linhas por colunas 
    fase fase_atual; 
    int fase_numero; //contador de fases percorridas
    int pontos; //pontuacao
    int peca_remov_fase; //total de pecas removidas na fase
    Recorde *recordes; //vetor dinamico dos recordes
    int qtd_recordes; //quantidade de elementos no vetor acima
    int linha_selecionada; //indice da linha q ta destacada no momento
    int coluna_selecionada; //mesma coisa so que com a coluna
    bool seleciona_linha; //true vai pro modo linha e false pro modo da coluna
    int espera_acao; //contagem de quadros pra pausar apos umaacao
    double inicio_fase; //timestamp de inicio da fase atual
    bool aviso_game_over; //true se estoura sem o jogador ter cumprido seu proposito
    bool saiu; //true se o player apertou esc
    animacao_t animacao_selecao; //animacao de contorno na linha/coluna q ta selecionada
} estadodojogo;
//prototipos
void processa_tecla(char tecla, estadodojogo *e);
int  ler_recordes(Recorde **vetor);
void initab(estadodojogo *e);
int  processa_limpeza(estadodojogo *e);
void gravidade(estadodojogo *e);
bool preencher_topo(estadodojogo *e);
void textos_pos_jogo(estadodojogo *e, char *nome, int largura, int altura);
void salva_recordes(estadodojogo *e, char *nome);
void add_recorde(Recorde *r);
void cabecalho_recordes(void);
bool jogar_de_novo(int largura, int altura);
cor_t cores(int cor);
int comparar_recordes(const void *a, const void *b);
fase criar_fase(int numero);
int cor_random(int max_cor);


//funcoes

//funcao pra inicializar a fase
fase criar_fase(int numero){ //dado um numero de fase ira definir o n max de cores (4 na fase 1, 5 na 2, etc..) e meta de remocoes q tem se
    fase f;                  //ser metade do tab
    f.numero = numero;
    if(f.numero > FASES) //evita que o numero passe do total de fase distintas
    f.numero = FASES;

    f.cores = 3 + numero;
    if(f.cores > MAX_CORES)
    f.cores = MAX_CORES;

    f.removidas_necessarias = (LINHAS * COLUNAS) / 2;
    return f;

}

//funcao para gerar um numero aleatorio entre 1 e o max de cor
int cor_random(int max_cor){
    return (rand() % max_cor) + 1;
}

//funcao pra inicializar o tab com as cores aleatorias geradas na funcao anterior
void initab(estadodojogo *estado){
    int max_cor = estado->fase_atual.cores;
    for(int i = 0; i < LINHAS; i++){ //percorre cada celula da matriz
        for(int j = 0; j < COLUNAS; j++){
            estado->tabuleiro[i][j] = cor_random(max_cor); //gera uma cor entre 1 e estado->tab....
        }
    }
}


//funcao pra girar a linha

void gira_linha(estadodojogo *estado, int linha, char direcao){//linha eh o numero da linha que vai girar(0 a 4) e direcao 'e' pra esq e 'd' pra dir
    if(linha < 0 || linha >= LINHAS) return; //aqui ta verificando pq pra girar a linha, nao pode ser menos que 0 a linha q vc quer girar pq ela nao existe e nao pode ser maior ou igual a LINHAS(5) pois o limite eh a linha 4 

    if(direcao == 'd'){
        int ultimapos = estado->tabuleiro[linha][COLUNAS - 1];//salvar o valor da ultima posicao que depois vai ir pra frente
        for(int j = COLUNAS - 1; j > 0; j--){//esse for vem de tras pra frente e vai alterando os n. [4] vai pro lugar do [5] e etc
            estado->tabuleiro[linha][j] = estado->tabuleiro[linha][j - 1];
        }
        estado->tabuleiro[linha][0] = ultimapos;//coloca o valor salvo no comeco
    }else if(direcao == 'e'){
        int primeirapos = estado->tabuleiro[linha][0];
        for(int j = 0; j < COLUNAS - 1; j++){
            estado->tabuleiro[linha][j] = estado->tabuleiro[linha][j + 1]; //numero 5 vai pro lugar do 4
        }
        estado->tabuleiro[linha][COLUNAS - 1] = primeirapos;
    }

}

//funcao de limpar a linha
void limpar_linha(estadodojogo *estado, int linha){
    if(linha < 0 || linha >= LINHAS) return;

    for(int j = 0; j < COLUNAS; j++){
        estado->tabuleiro[linha][j] = 0;
    }
}
//funcao de girar a coluna
void girar_coluna(int coluna, char direcao, estadodojogo *estado){
    if(coluna < 0 || coluna >= COLUNAS) return;

    // Verifica se a coluna tem pelo menos uma peça
    bool ocupada = false;
    for(int i = 0; i < LINHAS; i++){
        if(estado->tabuleiro[i][coluna] != 0){
            ocupada = true;
            break;
        }
    }
    if(!ocupada) return; // Se a coluna estiver vazia, não gira

    if(direcao == 'b'){ // girar pra baixo
        int ultima = estado->tabuleiro[LINHAS - 1][coluna];
        for(int i = LINHAS - 1; i > 0; i--){
            estado->tabuleiro[i][coluna] = estado->tabuleiro[i - 1][coluna];
        }
        estado->tabuleiro[0][coluna] = ultima;
    } else if(direcao == 'c'){ // girar pra cima
        int primeira = estado->tabuleiro[0][coluna];
        for(int i = 0; i < LINHAS - 1; i++){
            estado->tabuleiro[i][coluna] = estado->tabuleiro[i + 1][coluna];
        }
        estado->tabuleiro[LINHAS - 1][coluna] = primeira;
    }
}

//funcao pra verificar e limpar uma linha caso ela esteja com todos os valores da mesma cor
/*int verificar_linha(estadodojogo *estado){
   int removidas = 0;
    for(int i = 0; i < LINHAS; i++){
        int cor = estado->tabuleiro[i][0]; //cor da primeira coisa da linha pra comparar com as outras e tals

        if(cor == 0) continue; //se for 0 a kinha nao sera testada e vai pra proxima linha

        bool igual = true;//assume que tudo é =, se for visto um numeor diferente ela vira 0(falso)

        for(int j = 1; j < COLUNAS; j++){//começa da coluna 1 pois a coluna 0 ja foi usada no int cor
            if(estado->tabuleiro[i][j] != cor){
                igual = false; //fica falso e para
                break;
            }
        }
        if(igual){
            //remove COLUNAS pecas
            for(int j = 0; j < COLUNAS; j++){
                estado->tabuleiro[i][j] = 0;
            }
            removidas += COLUNAS;
           
        }
    }
     return removidas; //retorna quantas colunas foram removidas
    
}*/
//parecida com a funcao de verificar a linha porem com as colunas agora
//perccorre cada coluna, limpa se todas as celulas tiverem a mesma cor(!=0)
int verificar_coluna(estadodojogo *estado){
    int removidas = 0;
    for(int j = 0; j < COLUNAS; j++){
        int cor = estado->tabuleiro[0][j];
        if(cor == 0) continue; //ignora se a coluna comeca vazia

        bool igual = true;
        for(int i = 1; i < LINHAS; i++){
            if(estado->tabuleiro[i][j] != cor){
                igual = false;
                break;
            }
        }
        if(igual){//limpa a coluna
            for(int i = 0; i < LINHAS; i++){
                estado->tabuleiro[i][j] = 0;
            }
            removidas += LINHAS;
        }
    }
    return removidas;
}

//chama a funcao de verificar a linha e coluna, soma o total de pecas removidas, adiciona pontos e acumulao o total
int processa_limpeza(estadodojogo *estado){
    int rc = verificar_coluna(estado);
    if (rc > 0) {
        estado->peca_remov_fase += rc;
        int ganho = rc * estado->fase_atual.numero;
        estado->pontos += ganho;
    }
    return rc;
}


//funcao pra girar a coluna onde a peça mais baixa (LINHAS -1 ) nao vazia de uma coluna e é colocada no topo da coluna vizinha(e ou d)
//as condicoes nas intrucoes sao: a coluna de origem tem que ter pelo menos 1 peca e a vizinha pelo menos um espaco vazio
void girar_coluna_horizontal(int coluna, char direcao, estadodojogo *estado) {
    int destino;    
    if (direcao == 'e') {
        // esquerda: coluna −1 
        if (coluna == 0)
            destino = COLUNAS - 1;
        else
            destino = coluna - 1;
    } else {
        // direita: coluna +1 
        if (coluna == COLUNAS - 1)
            destino = 0;
        else
            destino = coluna + 1;
    }

    // 1) origem: precisa ter ao menos uma peça
    bool tem_peca = false;
    for (int i = 0; i < LINHAS; i++) {
        if (estado->tabuleiro[i][coluna] != 0) {
            tem_peca = true;
            break;
        }
    }
    if (!tem_peca) return;

    // 2) destino: precisa ter pelo menos um espaço vazio
    bool tem_vazio = false;
    for (int i = 0; i < LINHAS; i++) {
        if (estado->tabuleiro[i][destino] == 0) {
            tem_vazio = true;
            break;
        }
    }
    if (!tem_vazio) return;

    // 3) remove a peça da linha mais baixa ocupada
    int li = LINHAS - 1;
    while (li >= 0 && estado->tabuleiro[li][coluna] == 0)
        li--;
    int p = estado->tabuleiro[li][coluna];
    estado->tabuleiro[li][coluna] = 0;

    // 4) insere no topo (primeiro vazio) da coluna destino
    for (int i = 0; i < LINHAS; i++) {
        if (estado->tabuleiro[i][destino] == 0) {
            estado->tabuleiro[i][destino] = p;
            break;
        }
    }
}

//funcao gravidade, responsavel por fazer as pecas cairem.
//percorre uma coluna de baixo p cima. caso ache um bloco vazio, vai procurar acima e se tiver uma peça troca de lugar e tals...
void gravidade(estadodojogo *estado){
    for(int coluna = 0; coluna < COLUNAS; coluna++){ //for pra percorrer todas as colunas
        for(int linha = LINHAS - 1; linha >= 0; linha--){ //agr percorre de baixo pra cima as linhas 
            if(estado->tabuleiro[linha][coluna] == 0){ //verifica se a casa atual esta vazia
                int linha_aux = linha - 1; //cria uma funcao auxiliar que vai procurar acima da linha atual(que no momento é a q tem 0)alguma peça que nao seja 0
                while(linha_aux >= 0 && estado->tabuleiro[linha_aux][coluna] == 0){ //o whle vai subir a coluna procurando a proxima peça diferente de 0 e vai subir ate encontrar ela ou cheagar fora do tabulerio

                    linha_aux--;
                }
                if(linha_aux >= 0){
                    estado->tabuleiro[linha][coluna] = estado->tabuleiro[linha_aux][coluna];
                    estado->tabuleiro[linha_aux][coluna] = 0;
                }
            }
        } 
    }
}

//funcao pra preencher o topo do tabuleiro. quando toda primeira linha (i=0) estiver vqazia , vai sortear uma coluna e colocar uma peca la
//retorna true se gerou
bool preencher_topo(estadodojogo *estado){
    bool toda_vazia = true;
    for(int j = 0; j < COLUNAS; j++){ //verifica se todas as colunas de i = 0 estao vazias
        if(estado->tabuleiro[0][j] != 0 ){
            toda_vazia = false; //se retornou falso eh pq nao estao todas vazias
            break;
        }
    }
    if(!toda_vazia) return false;

    int j = rand() % COLUNAS; //escolhe coluna aleatoria e gera a cor
    estado->tabuleiro[0][j] = cor_random(estado->fase_atual.cores); 
    return true;
}



//funcao de processamento de entrada, traduz a tecla em acoes no jogo
//em cada acao valida reinicia animacao_selecao para dar o feedback visual no jogo
void processa_tecla(char tecla, estadodojogo *estado){
    
    switch(tecla){
        case '\t': //o tab vai alterar entre coluna e linha
        estado->seleciona_linha = !estado->seleciona_linha;
        anim_inicia(&estado->animacao_selecao, 0.3f, entrada_saida);
        break;

        case 'w': //move pra cima caso esteja selecionando a linha
        if(estado->seleciona_linha && estado->linha_selecionada > 0)
        estado ->linha_selecionada--;
        anim_inicia(&estado->animacao_selecao, 0.3f, entrada_saida);
        break;

        case 's':// p baixo a linha
        if(estado->seleciona_linha && estado->linha_selecionada < LINHAS - 1)
        estado->linha_selecionada++;
        anim_inicia(&estado->animacao_selecao, 0.3f, entrada_saida);
        break;

        case 'a': //esquerda se estiver a coluna selecionada
        if(!estado->seleciona_linha && estado->coluna_selecionada > 0)
        estado->coluna_selecionada--;
        anim_inicia(&estado->animacao_selecao, 0.3f, entrada_saida);
        break;

        case 'd': //direita se estiver com a coluna selec
        if(!estado->seleciona_linha && estado->coluna_selecionada < COLUNAS - 1)
        estado->coluna_selecionada++;
        anim_inicia(&estado->animacao_selecao, 0.3f, entrada_saida);
        break;

        case 'e': //gira pra esquerda a linha ou pra cima a coluna
        if(estado->seleciona_linha){
            gira_linha(estado, estado->linha_selecionada, 'd'); //esquerda
            
        }else{
            girar_coluna_horizontal(estado->coluna_selecionada, 'd', estado); //cima
        }
        estado->espera_acao = 30;
        break;

        case 'q'://gira pra direita ou pra baixo a coluna
        if(estado->seleciona_linha){
            gira_linha(estado, estado->linha_selecionada, 'e');
        }else{
            girar_coluna_horizontal(estado->coluna_selecionada, 'e', estado);
        }
         
         estado->espera_acao = 30;
         break;

         case 'b': //coluna ir pra baixo
         if(!estado->seleciona_linha){
            girar_coluna(estado->coluna_selecionada, 'b', estado);
            estado->espera_acao = 30;
         }break;

         case 'c': //coluna ir pra cima
         if(!estado->seleciona_linha){
            girar_coluna(estado->coluna_selecionada, 'c', estado);
            
            estado->espera_acao = 30;
         }break;

        case ' ': //limpa a linha selecionada
        limpar_linha( estado, estado->linha_selecionada);
        estado->pontos -= 2; //penalidade de limpar a linha
        if(estado->pontos < 0) estado->pontos = 0;
        
        estado->espera_acao = 30;
        break;
        
        case T_ESC: //sai do game
        estado->saiu = true;
        break;

        default:
        break;
    }
}
//funcao pra interface grafica do jogo
//converte os numeros em cores
cor_t cores(int cor) {
    switch (cor) {
        case 1: return vermelho;
        case 2: return azul;
        case 3: return verde;
        case 4: return amarelo;
        case 5: return roxo;
        case 6: return ciano;
        case 7: return laranja;
        case 8: return rosa;
        default: return preto;
    }
}   

//funcao tutorial pra ajudar o jogador a entender como se joga o jogo 
//exibe uma tela preta com texto em branco bem simples com os comandos do jogo
void mostraTutorial(int largura, int altura){
    j_seleciona_fonte(NULL, 25);

    //loop esperando pelo enter
    bool esperando = true;
    while(esperando){
        //limpa a tela
        retangulo_t fundo = {{0, 0}, {largura, altura}};
        j_retangulo(fundo, 0, preto, preto);

        
        ponto_t pos = {50,50};
        j_texto(pos, branco, "---TUTORIAL---");

        pos.y += 70;
        j_texto(pos, branco, "Objetivo: formar linhas ou colunas da mesma cor");
        pos.y += 50;
        j_texto(pos, branco, "Tab: troca entre linhas e colunas");
        pos.y += 50;
        j_texto(pos, branco, "W/S: so funcionam quando modo linha ta ativado e movem a linha pra cima e pra baixo");
        pos.y += 50;
        j_texto(pos, branco, "A/D: so funcionam quando modo coluna esta ativado e ele move a coluna pra esquerda ou direita");
        pos.y += 50;
        j_texto(pos, branco, "Q/E giram a coluna/linha");
        pos.y += 50;
        j_texto(pos,branco, "o jogo nao funcionara se o CAPSLOCK estiver ativado");
        pos.y += 70;
        j_texto(pos, branco, "Pressione ENTER pra começar");

        j_mostra();
        j_cochila(0.016);

        if(j_tem_tecla() && (j_tecla() == '\r' || j_tecla() == '\n'))
        esperando = false;
    }
}
bool jogar_de_novo(int largura, int altura){ //exibe se o player qer jogar de novo e retorna true ou false;
    while(1){
    retangulo_t f = {{0, 0}, {largura, altura}};
        j_retangulo(f, 0, preto, preto);

        ponto_t q ={largura/2 - 380, altura/2};
        j_texto(q, branco, "jogar de novo: (s/n): ");

        j_mostra();
        j_cochila(0.016);

        if(j_tem_tecla()){
            tecla_t k = j_tecla();
            if(k == 's' || k == 'S'){
                return true;
            }
            if(k == 'n' || k == 'N'){
                return false;
            }
        }
    }
}

//funcao pros textos que aparecem apos o jogo terminar
//loop de entrada de nome apos game over /saida
void textos_pos_jogo(estadodojogo *estado,char *nome, int largura, int altura){
    int pos = 0;
    bool digitando = true;
    nome[0] = '\0';

     while(digitando){
        retangulo_t fundo = {{0, 0}, {largura, altura}};
        j_retangulo(fundo, 0, preto, preto);

        if(estado->aviso_game_over){ //mensagem de fim de jogo
            ponto_t gc = {largura/2 - 200, altura/2 - 120};
            j_texto(gc, branco, "Game over");
        }
        else if(estado->saiu){// ou saiu
            ponto_t m = {largura /2 - 220, altura/2 - 120};
            j_texto(m, branco, "saiu do jogo");
        }

        //pede pra digitar o nome do jogador
        ponto_t p1 = {largura/2 - 340, altura / 2 - 20};
        j_texto(p1, branco, "Digite seu nome: ");
        ponto_t p2 = {largura/2 - 340, altura / 2 + 30};
        j_texto(p2, branco, nome);

        j_mostra();
        j_cochila(0.016);

        //deteccao de teclas
        if (j_tem_tecla()) {
        tecla_t c = j_tecla();
        if (c == T_ENTER) {
          
            digitando = false;
        }
        else if (c == T_BACKSPACE || c == T_DEL || c == T_BS) {
        
            if (pos > 0) {
                pos--;
                nome[pos] = '\0';
            }
        }
        else if (c >= 32 && c < 127 && pos < TAM_NOME - 1) {
          
            nome[pos++] = (char)c;
            nome[pos] = '\0';
        }
        
    }
    }

    
}
//abre o recordes.txt pra leitura e caso o arquivo nao exista retorna 0.
//ele le cada linha com nome, pontuacao, fase e realoca o vetor pra armazenar tudo
//precisei da ajuda de colegas pra fazer isso, pois faltei a aula de alocacao dinamica e fiquei perdido.
int ler_recordes(Recorde **vetor){
    FILE *arq = fopen("recordes.txt", "r");
    if(!arq) return 0;

    char linha[128];
    fgets(linha, sizeof(linha), arq);

    int qtd = 0;
    Recorde temp;
    *vetor = NULL;

    while(fscanf(arq, "%s %d %d", temp.nome, &temp.pontuacao, &temp.fase) == 3){
        *vetor = realloc(*vetor, (qtd + 1) * sizeof(Recorde));
        (*vetor)[qtd++] = temp;
    }
    fclose(arq);
    return qtd;
}

//funcao auxiliar para qsort: recebe dois ponteiros genericos e converte pra Recorde* e compara a pontuacao em ordem decrescente
int comparar_recordes(const void *a, const void *b){
    Recorde *r1 = (Recorde *)a;
    Recorde *r2 = (Recorde *)b;
    return r2->pontuacao - r1->pontuacao; // ordem decrescente
}


// abre recordes.txt em modo append e adiciona só um recorde
void add_recorde(Recorde *r) {
    FILE *arq = fopen("recordes.txt", "a");      // modo "a" = append
    if (!arq) { perror("fopen"); return; }
    // escreve nome, pontos e fase em uma linha nova:
    fprintf(arq, "%-10s %-8d %-5d\n",
            r->nome,
            r->pontuacao,
            r->fase);
    fclose(arq);
}
//funcao que salva os recordes
void salva_recordes(estadodojogo *estado, char *nome) {
    Recorde novo;
    strncpy(novo.nome, nome, TAM_NOME);
    novo.pontuacao = estado->pontos;
    novo.fase      = estado->fase_numero;

    //acrescenta no arquivo
    add_recorde(&novo);
}
// Se recordes.txt não existir, cria e escreve um cabeçalho
void cabecalho_recordes() {
    FILE *f = fopen("recordes.txt", "r");
    if (f) {
        fclose(f);
        return;   // ja existe dai não faz nada
    }
    f = fopen("recordes.txt", "w");
    if (!f) {
        perror("Não foi possível criar recordes.txt");
        return;
    }
    // escreve o cabeccalho
    fprintf(f, "%-10s %-8s %-5s\n", "Nome", "Pontos", "Fase");
    fclose(f);
}





//função principal
int main(){

    //interface grafica da janela onde ira ocorrer o jogo
    int largura = 1920;
    int altura = 1080;
    tamanho_t tamanho = {largura, altura};
    j_inicializa(tamanho, "Jogo de tabuleiro");
    mostraTutorial(largura, altura);

    bool jogar_dnv = false;
    cabecalho_recordes();
    srand(time(NULL));
    do{ //definicao da fase inicial do jogo e zera os contadores
    estadodojogo estado;
    estado.recordes = NULL;
    estado.qtd_recordes = ler_recordes(&estado.recordes); //le os recordes do disco estado.qtd_recordes e guarda quantos
    estado.fase_numero = 1;
    estado.fase_atual = criar_fase(estado.fase_numero);
    estado.pontos = 0;
    estado.peca_remov_fase = 0;
    estado.linha_selecionada = 0;
    estado.coluna_selecionada = 0;
    estado.seleciona_linha = true; //modo linha ativado
    estado.inicio_fase = j_relogio();
    estado.aviso_game_over = false; //aviso de game over quando acaba o tempo
    estado.saiu = false; //saida do jogador do jogo
    anim_inicia(&estado.animacao_selecao, 0.3f, entrada_saida); //animacao
    initab(&estado); //preenche o tab com cores random
    estado.espera_acao = 0; //sem o delay inicial apos as acoes
    j_seleciona_fonte(NULL, 40);//fonte do sans la

    //loop interno com um quadro por iteracao ate a partida acabar
    bool rodando = true;
    while(rodando){

        if(estado.saiu){
            rodando = false;
            break;
        }

        double t_atual = j_relogio(); //calcula o tempo que passou desde o inicio da fase
        double tempo_passado = t_atual - estado.inicio_fase;

        if(tempo_passado >= TEMPO_MAXIMO_FASE){ //se passou o tempo e a meta n foi atingida o jogo acaba
           if(estado.peca_remov_fase >= estado.fase_atual.removidas_necessarias){ //se cumprir a meta de pecas removidas ganha bonus
            int bonus = BONUS_FASE(estado.fase_numero);
            estado.pontos += bonus;
            printf("Bonus de fase: +%d pontos\n", bonus);

            estado.fase_numero++;//avanca o numero da fase e reconstroi os parametros
            estado.fase_atual = criar_fase(estado.fase_numero);
            //zera os contadores e reincia o cronometro
            estado.peca_remov_fase = 0;
            estado.inicio_fase = t_atual;
            initab(&estado);

            estado.espera_acao = 0;
           }else{ //nao cumpriu a meta da game over e sai do laco
            estado.aviso_game_over = true;
            rodando = false;
            break;
           }
        }
        //aqui eh basicamente interface grafica e reconhecimento de teclas
        //a biblioteca do professor auxiliou demais nessa ocasiao
        retangulo_t fundo = {{0, 0}, {largura, altura}};
        j_retangulo(fundo, 0, preto, preto);

        if(j_tem_tecla()){
            tecla_t tecla = j_tecla();
            if(tecla == 27){
                estado.saiu = true;
                rodando = false;
            }else{
                processa_tecla(tecla, &estado);
                
            }
        }
        //acoes automaticas(gravidade e surgimeto de peas novas)
        if(estado.espera_acao > 0){ //em cada frame se ainda estiver esperando decrementa, assim levando um tempo ate aparecer peças novas
            estado.espera_acao--; //se ainda estiver com delay pos jogada ele decrementa
        }else{//se nao ele limpa as combinacoes, soma os pontos, aplica a gravidade e se toda a linha i = 0 estiver vazia coloca uma peca
            processa_limpeza(&estado);
            gravidade(&estado);
            if(preencher_topo(&estado))
                estado.espera_acao = 30;//pausa pro jogador ver a peca chegando

        }
        //desenho do tab
        int largura_celula = 130;
        int altura_celula = 110;
        int espaco = 2;

        int largura_tab = COLUNAS * largura_celula + (COLUNAS - 1) * espaco;
        int altura_tab  = LINHAS   * altura_celula  + (LINHAS   - 1) * espaco;


        int margem_x = (largura - largura_tab) / 2;
        int margem_y = (altura - 100 - altura_tab) / 2 + 100;

        for(int i = 0; i < LINHAS; i++){
            for(int j = 0; j < COLUNAS; j++){
                //decide cor conforme valor no tab
                cor_t cor = cores(estado.tabuleiro[i][j]);

                //calcula a posicao na tela
                int x = margem_x + j * (largura_celula + espaco);
                int y = margem_y + i * (altura_celula + espaco);

                //fundo
                retangulo_t r = {{x, y}, {largura_celula, altura_celula}};
                j_retangulo(r, 0, preto, cor);

                //se uma linha ou coluna estiver selecionada desenha a borda branca
                bool estaSelecionada = (estado.seleciona_linha && i == estado.linha_selecionada) || (!estado.seleciona_linha && j == estado.coluna_selecionada);
                if(estaSelecionada){
                    float a = anim_valor(&estado.animacao_selecao);
                    int espessura = 1 + (int)(4 * a);
                    j_retangulo(r, espessura, branco, cor);
                }else{
                    j_retangulo(r, 1, preto, cor);
                }
            }
        }

        //parte do texto durante o jogo
        char texto[100];
        ponto_t texto_pos;

        //mostra a pontuacao
        sprintf(texto, "pontos: %d", estado.pontos);
        texto_pos.x = 60;
        texto_pos.y = 40;
        j_texto(texto_pos, branco, texto);

        //mostra a fase atual
        sprintf(texto, "fase: %d", estado.fase_atual.numero);
        texto_pos.y += 40;
        j_texto(texto_pos, branco, texto);
        
        //mostra o tempo restate em segundos
        int restante = (int)ceil(TEMPO_MAXIMO_FASE - tempo_passado);
        if(restante < 0) restante = 0;

        char tempo_txt[32];
        sprintf(tempo_txt, "Tempo: %ds", restante);
        ponto_t p = { largura - 250, 40};
        j_texto(p, branco, tempo_txt);

        j_mostra();
        j_cochila(0.016);
    }
       //entrada de nome pro recorde
    char nome[TAM_NOME] = {0};
    
    textos_pos_jogo(&estado, nome, largura, altura);
    salva_recordes(&estado, nome);
    jogar_dnv = jogar_de_novo(largura, altura);
}while(jogar_dnv); 
    j_finaliza();
    return 0;
}

