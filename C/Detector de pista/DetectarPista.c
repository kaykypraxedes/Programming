/*
- Desenvolvedores:
    Kayky Moreira Praxedes
    Carlos Ernesto Cardoso dos Reis

- Resumo do projeto:
    O programa processa várias sequências de números (chaves), 
    classificando-os em tipos (1, 2, 3) e agrupando valores 
    consecutivos iguais para otimização. 
    
    Uma vez os valores agrupados, o programa busca pela sequência 
    (1, 3, 2, 3, 1) para determinar pista, verifica a varição do 
    ponto médio de cada linha para identificar curvas, além de 
    identificar padrões que indicam impedimentos na pista.

    O sistema utiliza listas encadeadas com alocação dinâmica para 
    realizar a a análise independente de cada linha sequência e ao 
    final retorna a interpretação dos dados para quem estiver.
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct lista{
    int Chave;
    int Tipo;
    int NumElementos;
    int *PontoMedio;
    struct lista *nextPtr;
} LISTA;
/*
Cria o tipo LISTAPTR para evitar de usar ponteiro duplo **
(mais confortável visualmente)
*/
typedef LISTA *LISTAPTR;

void novo(LISTAPTR *ptr, int Chave, int Tipo, int *PontoMedio){
    *ptr = (LISTAPTR)malloc(sizeof(LISTA));
    (*ptr)->Chave = Chave;
    (*ptr)->Tipo = Tipo;
    (*ptr)->NumElementos = 1;
    (*ptr)->PontoMedio = PontoMedio;
    (*ptr)->nextPtr = NULL;
}

void inserir(LISTAPTR *ptr, int Chave){
    LISTAPTR newPtr, currentPtr, previousPtr;
    currentPtr = *ptr;
    int auxRepetido = 0;
    // Verifica se a classificação desse número já ocorreu ou é um número novo
    while(currentPtr != NULL){
        if(currentPtr->Chave == Chave) auxRepetido = currentPtr->Tipo;
        previousPtr = currentPtr;
        currentPtr = currentPtr->nextPtr;
    }
    // Se o elemento for o último
    if(previousPtr != NULL && previousPtr->Chave == Chave) previousPtr->NumElementos++;
    // Criar nova ramificação (1) já existente (0) nova
    else{
        novo(&newPtr, Chave, auxRepetido ? auxRepetido : 1, previousPtr->PontoMedio);
        previousPtr->nextPtr = newPtr;
        if(!auxRepetido){
            int max = -1;
            LISTAPTR auxLista = *ptr;
            while(auxLista != NULL){
                // Se encontrar um valor maior que o último na ordenação, ele é aumentado
                if(newPtr->Chave < auxLista->Chave) auxLista->Tipo++;
                // Encontra o antecessor da ordem do último valor novo e assume como seu sucessor
                else if(newPtr->Chave > auxLista->Chave && auxLista->Chave > max){
                    newPtr->Tipo = auxLista->Tipo + 1;
                    max = auxLista->Chave;
                }
                auxLista = auxLista->nextPtr;               
            }
        }
    }
}

int testa(LISTAPTR lista){
    int PontoMedio = 0;
    LISTAPTR listaAux;
    while(lista != NULL){
        listaAux = lista;
        if(lista->Tipo == 1){
            int PontoMedioAux = PontoMedio + listaAux->NumElementos;
            listaAux = listaAux->nextPtr;
            if(listaAux != NULL && listaAux->Tipo == 3){
                PontoMedioAux += listaAux->NumElementos;
                listaAux = listaAux->nextPtr;
                if(listaAux != NULL && listaAux->Tipo == 2){
                    PontoMedioAux += (listaAux->NumElementos)/2;
                    listaAux = listaAux->nextPtr;
                    if(listaAux != NULL && listaAux->Tipo == 3){
                        listaAux = listaAux->nextPtr;
                        if(listaAux != NULL && listaAux->Tipo == 1){
                            return PontoMedioAux;
                        }
                    }
                }
            }
        }
        PontoMedio += lista->NumElementos;
        lista = lista->nextPtr;
    }
    return -1;
}

int TestaObstaculo(LISTAPTR lista){
    LISTAPTR listaAux;
    while(lista != NULL){
        listaAux = lista;
        if(lista->Tipo == 1){
            listaAux = listaAux->nextPtr;
            if(listaAux != NULL && listaAux->Tipo == 3){
                listaAux = listaAux->nextPtr;
                if(listaAux != NULL && listaAux->Tipo == 2){
                    listaAux = listaAux->nextPtr;
                    if(listaAux != NULL && listaAux->Tipo == 3){
                        listaAux = listaAux->nextPtr;
                        if(listaAux != NULL && listaAux->Tipo == 1){
                            return 10;
                        }
                    }
                    while (listaAux!=NULL){
                        if(listaAux != NULL && listaAux->Tipo == 3){
                            listaAux = listaAux->nextPtr;
                            if(listaAux != NULL && listaAux->Tipo == 1){
                                return 1;
                            }
                        }
                        listaAux = listaAux->nextPtr;                          
                    }
                }
            }
        }
        lista = lista->nextPtr;
    }
    return -1;
}

int contaObs(int vetor[], int tamanho){
    int contador = 0;
    for(int i = 0;i<tamanho; i++){
        if(vetor[i]==1){
            contador++;
        }
    }
    return contador;
}

void freePtr(LISTAPTR *lista){
    LISTAPTR prevPtr;
    while(*lista != NULL){
        prevPtr = *lista;
        *lista = (*lista)->nextPtr;
        free(prevPtr);
    }
}

int main(){
    LISTAPTR myList = NULL;
    int quantidade, L;
    scanf(" %d", &L);
    int PontosMedios[L], obstaculos[L];
    int PontoMedio, obstaculo, numObs;
    for(int i = 0; i < L; i++){
        scanf(" %d", &quantidade);
        for(int j = 0; j < quantidade; j++){
            int Chave;
            scanf(" %d", &Chave);
            if(j == 0) novo(&myList, Chave, 1, &PontoMedio);
            else inserir(&myList, Chave);
        }
        PontoMedio = testa(myList); 
        freePtr(&myList);
        PontosMedios[i] = PontoMedio;
        obstaculos[i] = obstaculo;
    }
    numObs = contaObs(obstaculos, L);
    // Calcular a média dos pontos médios
    int soma = 0, cont = 0, validf=0;
    for(int i = 0; i < L; i++){
        if(PontosMedios[i] > 0){
            soma += PontosMedios[i];
            cont++;
            validf = i;
        }
    }
    int media = 0;
    // Se a quantidade de pontos médios válidos for maior que 70% do total
    if (cont > 70 * L / 100){
        media = (int) (soma / cont);
        // media-14 < pontosMedios < media+14
        if(media + 14 > PontosMedios[validf] && media - 30 < PontosMedios[validf]){ 
            printf("Pista em linha reta e ");
        } else if(media + 15 > PontosMedios[validf] && PontosMedios[validf] < PontosMedios[0]){
            printf("Curva a direita e ");
        } else if(media-15 < PontosMedios[validf] && PontosMedios[validf] > PontosMedios[0]){
            printf("Curva a esquerda e ");
        }
        if(numObs > 0){
            printf("com impedimento.\n");
        } else{
            printf("sem impedimento.\n");
        }
    } else printf("Formato da pista nao estimado.\n");
    return 0;
}