/* cifraDeCesar.c */
/*
Desenvolvedor:
    Kayky Moreira Praxedes
*/

#include <stdio.h>

void cifragem(char vet[], int i){
    for(int j = 0; vet[j] != '\n'; j++)
        // Letra maiúscula
        vet[j] += (vet[j] >= 'A' && vet[j] <= 'Z' ? (vet[j] + i <= 'Z' ? i : i - 26) : 
        // Letra minúscula
        (vet[j] >= 'a' && vet[j] <= 'z' ? (vet[j] + i <= 'z' ? i : i - 26) : 
        // Não é um dígito
        0));
}

int main(){
    char vet[100] = "";
    puts("Insira a mensagem:");
    fgets(vet, 100, stdin);
    puts("Insira o valor da cifra:");
    puts("0        - Brute-force.");
    puts("[1 - 26] - Cífra específica.");
    int cifra = 0, qualSwitch = 0;
    scanf(" %d", &cifra);
    // cifra == 0 (primeiro caso), cifra in [1, 26] (segundo caso), else (inválido)
    qualSwitch = (cifra == 0 ? 0 : (cifra >= 1 && cifra <= 26 ? 1 : -1));
    switch(qualSwitch){
        case 0:
            for(int i = 1; i <= 26; i++){
                cifragem(vet, 1);
                printf("Cifra = %02d: %s", i, vet);
            }
            break;
        case 1:
            cifragem(vet, cifra);
            printf("Cifra = %02d: %s", cifra, vet);
            break;
        default:
            puts("Valor inválido!");
    }
    return 0;
}
