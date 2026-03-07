/*
- Desenvolvedores:
    Kayky Moreira Praxedes
    Carlos Ernesto Cardoso dos Reis

- Resumo do projeto:
    Sistema de gerenciamento de aeroportos com funcionalidades 
    para adicionar, alterar, consultar e remover: voos, passagens e 
    passageiros.

    O sistema utiliza listas encadeadas e alocação dinâmica para 
    armazenar os dados e oferece um menu interativo para o usuário.
    
    O sistema é interligado, realizando alterações automáticas em 
    elementos codependentes, como por exemplo, remover as passagens e
    e os passageiros associados a um voo ao remover o voo.
    
    Os dados são armazenados em memória durante a execução do programa,
    mas pode ser expandido para incluir persistência em arquivos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct voos{
    int identificador;
    int numero_aeronave;
    char empresa[20];
    char origem[50];
    char destino[50];
    char data_partida[15];
    char horario_partida[10];
    int quantidade_passageiros;
    int qvoos;
    struct voos *next;
}VOOS;

typedef VOOS *VOOSPTR;

typedef struct passagens{
    int numero;
    int identificador_passageiro;
    int identificador_voo;
    double valor;
    char data_venda[15];
    struct passagens *next;
}PASSAGENS;

typedef PASSAGENS *PASSAGENSPTR;

typedef struct passageiros{
    int identificador;
    char nome[30];
    char identidade[20];
    char endereco[50];
    char telefone[20];
    char sexo;
    int qpassageiros;
    struct passageiros *next;
}PASSAGEIROS;

typedef PASSAGEIROS *PASSAGEIROSPTR;

int menu();
int menuinterno();
int quantidade_total_voos = 0;
int quantidade_total_passageiros = 0;
void acao(VOOSPTR *, PASSAGENSPTR *, PASSAGEIROSPTR *, int, int);
void adicionar(VOOSPTR *, PASSAGENSPTR *, PASSAGEIROSPTR *, int);
void alterar(VOOSPTR *, PASSAGENSPTR *, PASSAGEIROSPTR *, int);
void consultar(const VOOSPTR *, const PASSAGENSPTR *, const PASSAGEIROSPTR *, int);
void remover(VOOSPTR *vptr, PASSAGENSPTR *psptr, PASSAGEIROSPTR *paptr, int menu);
void relatorio(VOOSPTR vptr, PASSAGEIROSPTR paptr, PASSAGENSPTR psptr);


int main(){
    VOOSPTR voo = NULL;
    PASSAGENSPTR passagem = NULL;
    PASSAGEIROSPTR passageiro = NULL;
    // Adiciona elementos de teste

    // Add voo 1
    VOOS *novoVoo = malloc(sizeof(VOOS));
    novoVoo->identificador = 1;
    novoVoo->numero_aeronave = 101;
    snprintf(novoVoo->empresa, sizeof(novoVoo->empresa), "EmpresaX");
    snprintf(novoVoo->data_partida, sizeof(novoVoo->data_partida), "01/10/2024");
    snprintf(novoVoo->horario_partida, sizeof(novoVoo->horario_partida), "10:00");
    snprintf(novoVoo->origem, sizeof(novoVoo->origem), "Sao Paulo");
    snprintf(novoVoo->destino, sizeof(novoVoo->destino), "Rio de Janeiro");
    novoVoo->quantidade_passageiros = 0;
    novoVoo->qvoos = 1;
    novoVoo->next = NULL;
    voo = novoVoo;
    // Add voo 2
    VOOS *novoVoo1 = malloc(sizeof(VOOS));
    novoVoo1->identificador = 2;
    novoVoo1->numero_aeronave = 911;
    snprintf(novoVoo1->empresa, sizeof(novoVoo1->empresa), "EmpresaY");
    snprintf(novoVoo1->data_partida, sizeof(novoVoo1->data_partida), "10/12/2024");
    snprintf(novoVoo1->horario_partida, sizeof(novoVoo1->horario_partida), "15:00");
    snprintf(novoVoo1->origem, sizeof(novoVoo1->origem), "Belo Horizonte");
    snprintf(novoVoo1->destino, sizeof(novoVoo1->destino), "Roraima");
    novoVoo1->quantidade_passageiros = 0;
    novoVoo1->qvoos = 1;
    novoVoo1->next = NULL;
    novoVoo->next = novoVoo1;
    //Add passageiro 1
    PASSAGEIROS *novoPassageiro1 = malloc(sizeof(PASSAGEIROS));
    novoPassageiro1->identificador = 1;
    snprintf(novoPassageiro1->nome, sizeof(novoPassageiro1->nome), "Joao Silva");
    snprintf(novoPassageiro1->identidade, sizeof(novoPassageiro1->identidade), "123456789");
    snprintf(novoPassageiro1->endereco, sizeof(novoPassageiro1->endereco), "Rua A, 123");
    snprintf(novoPassageiro1->telefone, sizeof(novoPassageiro1->telefone), "(11) 98765-4321");
    novoPassageiro1->sexo = 'M';
    novoPassageiro1->qpassageiros = 1;
    novoPassageiro1->next = NULL;
    passageiro = novoPassageiro1;
    quantidade_total_passageiros++;
    //Add passageiro 1
    PASSAGEIROS *novoPassageiro2 = malloc(sizeof(PASSAGEIROS));
    novoPassageiro2->identificador = 2;
    snprintf(novoPassageiro2->nome, sizeof(novoPassageiro2->nome), "Maria Oliveira");
    snprintf(novoPassageiro2->identidade, sizeof(novoPassageiro2->identidade), "987654321");
    snprintf(novoPassageiro2->endereco, sizeof(novoPassageiro2->endereco), "Rua B, 456");
    snprintf(novoPassageiro2->telefone, sizeof(novoPassageiro2->telefone), "(21) 91234-5678");
    novoPassageiro2->sexo = 'F';
    novoPassageiro2->qpassageiros = 2;
    novoPassageiro2->next = NULL;
    novoPassageiro1->next = novoPassageiro2;
    quantidade_total_passageiros++;
    //Add passageiro 3
    PASSAGEIROS *novoPassageiro3 = malloc(sizeof(PASSAGEIROS));
    novoPassageiro3->identificador = 3;
    snprintf(novoPassageiro3->nome, sizeof(novoPassageiro3->nome), "Kayky Praxedes");
    snprintf(novoPassageiro3->identidade, sizeof(novoPassageiro2->identidade), "11122233344");
    snprintf(novoPassageiro3->endereco, sizeof(novoPassageiro2->endereco), "Rua dos Bobos, 456");
    snprintf(novoPassageiro3->telefone, sizeof(novoPassageiro2->telefone), "(11) 91255-5678");
    novoPassageiro3->sexo = 'M';
    novoPassageiro3->qpassageiros = 3;
    novoPassageiro3->next = NULL;
    novoPassageiro2->next = novoPassageiro3;
    quantidade_total_passageiros++;
    //Add passagem 1
    PASSAGENS *novaPassagem1 = malloc(sizeof(PASSAGENS));
    novaPassagem1->numero = 1;
    novaPassagem1->identificador_passageiro = 1;
    novaPassagem1->identificador_voo = 1;
    novaPassagem1->valor = 500.00;
    snprintf(novaPassagem1->data_venda, sizeof(novaPassagem1->data_venda), "01/09/2024");
    novaPassagem1->next = NULL;
    passagem = novaPassagem1;
    //Add passagem 2
    PASSAGENS *novaPassagem2 = malloc(sizeof(PASSAGENS));
    novaPassagem2->numero = 2;
    novaPassagem2->identificador_passageiro = 2;
    novaPassagem2->identificador_voo = 1;
    novaPassagem2->valor = 550.00;
    snprintf(novaPassagem2->data_venda, sizeof(novaPassagem2->data_venda), "02/09/2024");
    novaPassagem2->next = NULL;
    novaPassagem1->next = novaPassagem2;
    //Add passagem 3
    PASSAGENS *novaPassagem3 = malloc(sizeof(PASSAGENS));
    novaPassagem3->numero = 3;
    novaPassagem3->identificador_passageiro = 3;
    novaPassagem3->identificador_voo = 2;
    novaPassagem3->valor = 230.00;
    snprintf(novaPassagem3->data_venda, sizeof(novaPassagem3->data_venda), "05/12/2024");
    novaPassagem3->next = NULL;
    novaPassagem2->next = novaPassagem3;

    int escolha;
    while((escolha = menu()) != 5){
        int escint;
        switch (escolha){
        case 1:
            puts("Voos");
            escint = menuinterno();
            acao(&voo, &passagem, &passageiro, escolha, escint);
            break;
        case 2:
            puts("Passagens");
            escint = menuinterno();
            acao(&voo, &passagem, &passageiro, escolha, escint);
            break;
        case 3:
            puts("Passageiros");
            escint = menuinterno();
            acao(&voo, &passagem, &passageiro, escolha, escint);
            break;
        case 4:
            relatorio(voo, passageiro, passagem);   
            break;
        
        default:
            puts("Opcao Invalida");
            break;
        }
    }
    return 0;
}

int menu(){
    int a;
    printf("1 - Voos\n"
    "2 - Passagens\n"
    "3 - Passageiros\n"
    "4 - Relatorios\n"
    "5 - Sair\n? ");
    scanf(" %d", &a);
    return a;
}

int menuinterno(){
    int a;
    printf("1 - Adicionar\n"
    "2 - Alterar\n"
    "3 - Excluir\n"
    "4 - Consultar\n"
    "5 - Sair\n? ");
    scanf(" %d", &a);
    return a;
}

void acao(VOOSPTR *vooptr, PASSAGENSPTR *passagemptr, PASSAGEIROSPTR *passageiroptr, int escolha1, int escolha2){
    switch (escolha2){
    case 1:
        adicionar(vooptr, passagemptr, passageiroptr, escolha1);
        break;
    case 2:
        alterar(vooptr, passagemptr, passageiroptr, escolha1);
        break;
    case 3:
        remover(vooptr, passagemptr, passageiroptr, escolha1);
        break;
    case 4:
        consultar(vooptr, passagemptr, passageiroptr, escolha1);
        break;
    case 5:
        break;          
    default:
        puts("Opcao Invalida");
        break;
    }
}

void adicionar(VOOSPTR *vptr, PASSAGENSPTR *psptr, PASSAGEIROSPTR *paptr, int menu){
    switch (menu){
    case 1: //adicionar voo
        if((*vptr != NULL)&&quantidade_total_voos >= 50){
            puts("Limite maximo de 50 voos ja foi excedido");
        }else{
            puts("Adicionando Voos:");
            VOOSPTR prevptr, atualptr, newptr;
            atualptr = *vptr;
            newptr = malloc(sizeof(VOOS));
            printf("Indentificador\n? ");
            scanf(" %d", &newptr->identificador);
            //Condição pra não ter identificador repetido
            atualptr = *vptr;
            while(atualptr != NULL){
                if(atualptr->identificador == newptr->identificador){
                    puts("Esse identificador ja foi cadastrado\n");
                    free(newptr);
                    return;
                }
                atualptr = atualptr->next;
            }
            printf("Numero da Aeronave\n? ");
            scanf(" %d", &newptr->numero_aeronave);
            printf("Empresa\n? ");
            scanf(" %[^\n]", newptr->empresa);
            printf("Data da partida \"00/00/0000\"\n? ");
            scanf(" %[^\n]", newptr->data_partida);
            printf("Horario da partida \"00:00\"\n? ");
            scanf(" %[^\n]", newptr->horario_partida);
            newptr->quantidade_passageiros = 0;
            printf("Origem\n? ");
            scanf(" %[^\n]", newptr->origem);
            printf("Destino\n? ");
            scanf(" %[^\n]", newptr->destino);
            newptr->next = NULL;
            prevptr = NULL;

        if(*vptr == NULL){
            *vptr = newptr;
            newptr->qvoos = 1;
        }else{
            VOOSPTR atualptr = *vptr;
            while(atualptr->next != NULL){
                atualptr = atualptr->next;
            }
            atualptr->next = newptr;
            (*vptr)->qvoos++;
        }
        quantidade_total_voos++;
        }
        break;
    case 2: //adicionar passagens
        if(*vptr == NULL ||*paptr == NULL){
            puts("Sem passageiros ou voos para passagens\n");
            return;
        }else{
            if((*psptr != NULL)){
                PASSAGENSPTR teste = *psptr;
                while(teste != NULL){
                    teste = teste->next;
                }
                if(teste->numero >= 100){
                    puts("Limite maximo de 100 passagens ja foi excedido");
                    return;
                }
            }
            puts("Adicionando Passagens:");
            PASSAGENSPTR prevptr, atualptr, newptr;
            newptr = malloc(sizeof(PASSAGENS));
            printf("Identificador Passageiro\n? ");
            scanf(" %d", &newptr->identificador_passageiro);
            PASSAGEIROSPTR testepa = *paptr;
            while(testepa != NULL && newptr->identificador_passageiro != testepa->identificador){
                testepa = testepa->next;
            }
            if(testepa == NULL){
                puts("Esse identificador nao existe\n");
                free(newptr);
                return;
            }
            printf("Identificador Voo\n? ");
            scanf(" %d", &newptr->identificador_voo);
            VOOSPTR testev = *vptr;
            while(testev != NULL && newptr->identificador_voo != testev->identificador){
                testev = testev->next;
            }
            if(testev == NULL){
                puts("Esse identificador nao existe\n");
                free(newptr);
                return;
            }
            printf("Valor\n? ");
            scanf(" %lf", &newptr->valor);
            printf("Data da Venda \"00/00/0000\"\n? ");
            scanf(" %[^\n]", newptr->data_venda);
            newptr->next = NULL;
            prevptr = NULL;
            atualptr = *psptr;
            if(atualptr == NULL){
                *psptr = newptr;
                (*psptr)->numero = 1;
            }else{
                while(atualptr != NULL){
                    prevptr = atualptr;
                    atualptr = atualptr->next;
                }
                prevptr->next = newptr;
                newptr->numero = prevptr->numero + 1;
            }
        }
        break;
    case 3: //adicionar passageiros
        if((*paptr != NULL)&&(*paptr)->qpassageiros >= 100){
            puts("Limite maximo de 100 passageiros ja foi excedido");
        }else{
            puts("Adicionando Passageiros:");
            PASSAGEIROSPTR prevptr, atualptr, newptr;
            atualptr = *paptr;
            newptr = malloc(sizeof(PASSAGEIROS));
            printf("Indentificador\n? ");
            scanf(" %d", &newptr->identificador);
            if(atualptr != NULL){
                while(atualptr != NULL && atualptr->identificador != newptr->identificador){
                    atualptr = atualptr->next;
                }
                if(atualptr->identificador == newptr->identificador){
                    puts("Esse identificador ja foi cadastrado\n");
                    free(newptr);
                    return;
                }
            }
            printf("Nome\n? ");
            scanf(" %[^\n]", newptr->nome);
            printf("Identidade\n? ");
            scanf(" %[^\n]", newptr->identidade);
            printf("Endereco\n? ");
            scanf(" %[^\n]", newptr->endereco);
            printf("Telefone \"(00)0 0000-0000\"\n? ");
            scanf(" %[^\n]", newptr->telefone);
            printf("Sexo\"H\", \"M\"\n? ");
            scanf(" %c", &newptr->sexo);
            newptr->next = NULL;
            prevptr = NULL;
            if(atualptr == NULL){
                *paptr = newptr;
                (*paptr)->qpassageiros = 1;
            }else{
                while(atualptr != NULL){
                    prevptr = atualptr;
                    atualptr = atualptr->next;
                }
                prevptr->next = newptr;
                (*paptr)->qpassageiros++;
            }
        }
        break;
    }
}

void alterar(VOOSPTR *vptr, PASSAGENSPTR *psptr, PASSAGEIROSPTR *paptr, int menu){
    switch (menu){
    case 1: // alterar voo
        if(*vptr == NULL){
            puts("Nao existem voos no sistema");
        } else {
            puts("Alterando Voos:");
            int id;
            printf("Digite o identificador do voo a ser alterado\n? ");
            scanf(" %d", &id);

            VOOSPTR atualptr = *vptr;
            while(atualptr != NULL && atualptr->identificador != id){
                atualptr = atualptr->next;
            }

            if(atualptr == NULL){
                puts("Voo nao encontrado");
            } else {
                printf("Numero da Aeronave (atual: %d)\n? ", atualptr->numero_aeronave);
                scanf(" %d", &atualptr->numero_aeronave);
                printf("Empresa (atual: %s)\n? ", atualptr->empresa);
                scanf(" %[^\n]", atualptr->empresa);
                printf("Data da partida (atual: %s)\n? ", atualptr->data_partida);
                scanf(" %[^\n]", atualptr->data_partida);
                printf("Horario da partida (atual: %s)\n? ", atualptr->horario_partida);
                scanf(" %[^\n]", atualptr->horario_partida);
                printf("Origem (atual: %s)\n? ", atualptr->origem);
                scanf(" %[^\n]", atualptr->origem);
                printf("Destino (atual: %s)\n? ", atualptr->destino);
                scanf(" %[^\n]", atualptr->destino);
                puts("Voo alterado com sucesso");
            }
        }
        break;
    case 2: // alterar passagens
        if(*psptr == NULL){
            puts("Nao existem passagens no sistema");
        } else {
            puts("Alterando Passagens:");
            int numero;
            printf("Digite o numero da passagem a ser alterada\n? ");
            scanf(" %d", &numero);

            PASSAGENSPTR atualptr = *psptr;
            while(atualptr != NULL && atualptr->numero != numero){
                atualptr = atualptr->next;
            }

            if(atualptr == NULL){
                puts("Passagem nao encontrada");
            } else {
                printf("Identificador do Passageiro (atual: %d)\n? ", atualptr->identificador_passageiro);
                scanf(" %d", &atualptr->identificador_passageiro);
                printf("Identificador do Voo (atual: %d)\n? ", atualptr->identificador_voo);
                scanf(" %d", &atualptr->identificador_voo);
                printf("Valor (atual: %.2f)\n? ", atualptr->valor);
                scanf(" %lf", &atualptr->valor);
                printf("Data da Venda (atual: %s)\n? ", atualptr->data_venda);
                scanf(" %[^\n]", atualptr->data_venda);
                puts("Passagem alterada com sucesso");
            }
        }
        break;
    case 3: // alterar passageiros
        if(*paptr == NULL){
            puts("Nao existem passageiros no sistema");
        } else {
            puts("Alterando Passageiros:");
            int id;
            printf("Digite o identificador do passageiro a ser alterado\n? ");
            scanf(" %d", &id);

            PASSAGEIROSPTR atualptr = *paptr;
            while(atualptr != NULL && atualptr->identificador != id){
                atualptr = atualptr->next;
            }

            if(atualptr == NULL){
                puts("Passageiro nao encontrado");
            } else {
                printf("Nome (atual: %s)\n? ", atualptr->nome);
                scanf(" %[^\n]", atualptr->nome);
                printf("Identidade (atual: %s)\n? ", atualptr->identidade);
                scanf(" %[^\n]", atualptr->identidade);
                printf("Endereco (atual: %s)\n? ", atualptr->endereco);
                scanf(" %[^\n]", atualptr->endereco);
                printf("Telefone (atual: %s)\n? ", atualptr->telefone);
                scanf(" %[^\n]", atualptr->telefone);
                printf("Sexo (atual: %c)\n? ", atualptr->sexo);
                scanf(" %c", &atualptr->sexo);
                puts("Passageiro alterado com sucesso");
            }
        }
        break;
    }
}

void consultar(const VOOSPTR *vptr, const PASSAGENSPTR *psptr, const PASSAGEIROSPTR *paptr, int menu){
    switch (menu){
     case 1: //Consultar voo
        if(*vptr == NULL){
            puts("Nao existem voos no sistema");
        }else{
            puts("Consultando Voos:");
            VOOSPTR atualptr = *vptr;
            int a = 1;
            while(atualptr != NULL){
                printf("Voo: #%d\n", a);
                printf("Indentificador:             %d\n", atualptr->identificador);
                printf("Numero da Aeronave:         %d\n", atualptr->numero_aeronave);
                printf("Empresa:                    %s\n", atualptr->empresa);
                printf("Data da partida:            %s\n", atualptr->data_partida);
                printf("Horario da partida:         %s\n", atualptr->horario_partida);
                printf("Origem:                     %s\n", atualptr->origem);
                printf("Destino:                    %s\n", atualptr->destino);
                // Contar passageiros para este voo
                int passageirosNesteVoo = 0;
                PASSAGENSPTR passagemAtual = *psptr;
                while(passagemAtual != NULL){
                    if(passagemAtual->identificador_voo == atualptr->identificador){
                        passageirosNesteVoo++;
                    }
                    passagemAtual = passagemAtual->next;
                }
                printf("Quantidade de Passageiros:  %d\n\n", passageirosNesteVoo);
                
                atualptr = atualptr->next;
                a++;
            }
        }
        break;
    case 2: //Consultar passagens
        if(*psptr == NULL){
            puts("Nao existem passagens no sistema");
        }else{
            puts("Consultando Passagens");
            PASSAGENSPTR atualptr = *psptr;
            while(atualptr != NULL){
                printf("Numero:                     #%d\n", atualptr->numero);
                printf("Identificador Passageiro:   %d\n", atualptr->identificador_passageiro);
                printf("Identificador Voo:          %d\n", atualptr->identificador_voo);
                printf("Valor:                      RS%.2lf\n", atualptr->valor);
                printf("Data da Venda:              %s\n\n", atualptr->data_venda);
                atualptr = atualptr->next;
            }
        }
        break;
    case 3: //Consultar passageiros
        if(*paptr == NULL){
            puts("Nao existem passageiros no sistema");
        }else{
            puts("Consultando Passageiros:");
            PASSAGEIROSPTR atualptr = *paptr;
            int a = 1;
            while(atualptr != NULL){
                printf("Passageiro: #%d\n", a);
                printf("Indentificador: %d\n", atualptr->identificador);
                printf("Nome:           %s\n", atualptr->nome);
                printf("Identidade:     %s\n", atualptr->identidade);
                printf("Endereco:       %s\n", atualptr->endereco);
                printf("Telefone:       %s\n", atualptr->telefone);
                printf("Sexo:           %c\n\n", atualptr->sexo);
                atualptr = atualptr->next;
                a++;
            }
        }
        break;
    }
}

void remover(VOOSPTR *vptr, PASSAGENSPTR *psptr, PASSAGEIROSPTR *paptr, int menu) {
    switch (menu) {
    case 1: { // remover voo
        if (*vptr == NULL) {
            puts("Nao existem voos no sistema");
            return;
        }
        int id;
        puts("Removendo Voo:");
        printf("Digite o identificador do voo a ser removido\n? ");
        scanf(" %d", &id);

        VOOSPTR atualptr = *vptr, prevptr = NULL;
        while (atualptr != NULL && atualptr->identificador != id) {
            prevptr = atualptr;
            atualptr = atualptr->next;
        }

        if (atualptr == NULL) {
            puts("Voo nao encontrado");
        } else {
            // Remover passagens e passageiros associados ao voo
            PASSAGENSPTR passagemAtual = *psptr, passagemPrev = NULL;
            while (passagemAtual != NULL) {
                if (passagemAtual->identificador_voo == id) {
                    // Remover passageiro associado à passagem
                    PASSAGEIROSPTR passageiroAtual = *paptr, passageiroPrev = NULL;
                    while (passageiroAtual != NULL) {
                        if (passageiroAtual->identificador == passagemAtual->identificador_passageiro) {
                            if (passageiroPrev == NULL) {
                                *paptr = passageiroAtual->next;
                            } else {
                                passageiroPrev->next = passageiroAtual->next;
                            }
                            free(passageiroAtual);
                            if (*paptr != NULL) {
                                (*paptr)->qpassageiros--;
                            }
                            break;
                        }
                        passageiroPrev = passageiroAtual;
                        passageiroAtual = passageiroAtual->next;
                    }

                    // Remover a passagem
                    PASSAGENSPTR passagemParaRemover = passagemAtual;
                    passagemAtual = passagemAtual->next;
                    
                    if (passagemPrev == NULL) {
                        *psptr = passagemAtual;
                    } else {
                        passagemPrev->next = passagemAtual;
                    }
                    
                    free(passagemParaRemover);
                } else {
                    passagemPrev = passagemAtual;
                    passagemAtual = passagemAtual->next;
                }
            }

            // Remover o voo
            if (prevptr == NULL) {
                *vptr = atualptr->next;
            } else {
                prevptr->next = atualptr->next;
            }
            free(atualptr);
            
            if (*vptr != NULL) {
                (*vptr)->qvoos--;
                quantidade_total_voos--;
            }
            
            puts("Voo, passagens e passageiros associados removidos com sucesso");
        }
        break;
    }
     case 2: { // remover passagem
        if (*psptr == NULL) {
            puts("Nao existem passagens no sistema");
            return;
        }
        int numero;
        puts("Removendo Passagem:");
        printf("Digite o numero da passagem a ser removida\n? ");
        scanf(" %d", &numero);

        PASSAGENSPTR atualptr = *psptr, prevptr = NULL;
        while (atualptr != NULL && atualptr->numero != numero) {
            prevptr = atualptr;
            atualptr = atualptr->next;
        }

        if (atualptr == NULL) {
            puts("Passagem nao encontrada");
        } else {
            if (prevptr == NULL) {
                *psptr = atualptr->next;
            } else {
                prevptr->next = atualptr->next;
            }
            free(atualptr);
            puts("Passagem removida com sucesso");
        }
        break;
    }
    
    case 3: { // remover passageiro
        if (*paptr == NULL) {
            puts("Nao existem passageiros no sistema");
            return;
        }
        int id;
        puts("Removendo Passageiro:");
        printf("Digite o identificador do passageiro a ser removido\n? ");
        scanf(" %d", &id);

        PASSAGEIROSPTR atualptr = *paptr, prevptr = NULL;
        while (atualptr != NULL && atualptr->identificador != id) {
            prevptr = atualptr;
            atualptr = atualptr->next;
        }

        if (atualptr == NULL) {
            puts("Passageiro nao encontrado");
        } else {
            // Remover passagens associadas ao passageiro
            PASSAGENSPTR passagemAtual = *psptr, passagemPrev = NULL;
            while (passagemAtual != NULL) {
                if (passagemAtual->identificador_passageiro == id) {
                    PASSAGENSPTR passagemParaRemover = passagemAtual;
                    passagemAtual = passagemAtual->next;
                    
                    if (passagemPrev == NULL) {
                        *psptr = passagemAtual;
                    } else {
                        passagemPrev->next = passagemAtual;
                    }
                    
                    free(passagemParaRemover);
                } else {
                    passagemPrev = passagemAtual;
                    passagemAtual = passagemAtual->next;
                }
            }

            // Remover o passageiro
            if (prevptr == NULL) {
                *paptr = atualptr->next;
            } else {
                prevptr->next = atualptr->next;
            }
            free(atualptr);
            
            if (*paptr != NULL) {
                (*paptr)->qpassageiros--;
            }
            
            puts("Passageiro e suas passagens removidos com sucesso");
        }
        break;
    }
    default:
        puts("Opcao invalida");
        break;
    }
}

void relatorio(VOOSPTR vptr, PASSAGEIROSPTR paptr, PASSAGENSPTR psptr) {
    printf("\n--- Relatorio ---\n");
    // Quantidade de passagens por voo
    printf("Quantidade de passagens por voo:\n");
    VOOSPTR vooAtual = vptr;
    while (vooAtual != NULL) {
        int passagensPorVoo = 0;
        PASSAGENSPTR passagemAtual = psptr;
        while (passagemAtual != NULL) {
            if (passagemAtual->identificador_voo == vooAtual->identificador) {
                passagensPorVoo++;
            }
            passagemAtual = passagemAtual->next;
        }
        printf("Voo %d: %d passagens\n", vooAtual->identificador, passagensPorVoo);
        vooAtual = vooAtual->next;
    }

    // Quantidade de voos por empresa
    printf("\nQuantidade de voos por empresa:\n");
    vooAtual = vptr;
    while (vooAtual != NULL) {
        int voosPorEmpresa = 0;
        VOOSPTR vooTemp = vptr;
        while (vooTemp != NULL) {
            if (strcmp(vooTemp->empresa, vooAtual->empresa) == 0) {
                voosPorEmpresa++;
            }
            vooTemp = vooTemp->next;
        }
        printf("Empresa %s: %d voo(s)\n", vooAtual->empresa, voosPorEmpresa);
        vooAtual = vooAtual->next;
    }

    // Lista de passageiros por voo
    printf("\nLista de passageiros por voo:\n");
    vooAtual = vptr;
    while (vooAtual != NULL) {
        printf("Voo %d:\n", vooAtual->identificador);
        PASSAGENSPTR passagemAtual = psptr;
        while (passagemAtual != NULL) {
            if (passagemAtual->identificador_voo == vooAtual->identificador) {
                PASSAGEIROSPTR passageiroAtual = paptr;
                while (passageiroAtual != NULL) {
                    if (passageiroAtual->identificador == passagemAtual->identificador_passageiro) {
                        printf("  - %s\n", passageiroAtual->nome);
                        break;
                    }
                    passageiroAtual = passageiroAtual->next;
                }
            }
            passagemAtual = passagemAtual->next;
        }
        vooAtual = vooAtual->next;
    }
    printf("-----------------\n");
}