/* main.cpp */
/*
Desenvolvedor:
    Kayky Moreira Praxedes
*/
#include <iostream>
#include "headers/fatora.h"
#include "headers/primos.h"
#include "headers/cripto.h"
void imprimeFatoracao(){
    std::cout << "\n====================== Fatoração ======================\n\n";
    unsigned long long int valor{};
    std::cout << "Insira o valor a ser fatorado: ";
    std::cin >> valor;
    FATOR tempoEprimos{fatora(valor)};
    std::cout << stringFatores(valor) << "\n\n" << tempoEprimos.num2 << " primo(s) calculado(s) em " << ajustaEscala(tempoEprimos.num1) << "\n\n";
}
void imprimeMinPrimo(){
    std::cout << "\n================== Cálculo do primo ===================\n\n";
    int valor{};
    std::cout << "Insira o número de dígitos do primo: ";
    std::cin >> valor;
    if(valor > 0) std::cout << "O menor primo com " << valor << " dígito(s) é " << primoMin(valor) << "\n\n";
    else std::cout << "Valor inválido! Não existem números com 0 dígitos!\n\n";
}
void criptografia(){
    std::cout << "\n=================== Criptografia RSA ==================\n\n";
    short int opcao{};
    while(true){
        unsigned long long int p{}, q{}, e{}, n{}, d{};
        std::string mensagem{};
        std::cout << "1) Verificar um conjunto de chaves\n2) Codificar uma mensagem\n3) Decodificar uma mensagem\n0) Encerrar\nOpção: ";
        std::cin >> opcao;
        switch (opcao){
        case 1:
            std::cout << "\nInsira as chaves (p q e): ";
            std::cin >> p >> q >> e;
            std::cout << getChaves(p, q, e) << "\n\n";
            break;
        case 2:
            std::cout << "\nInsira as chaves para codificação (n e): ";
            std::cin >> n >> e;
            std::cin.ignore(); // Limpa o buffer
            std::cout << "Insira a mensagem: ";
            std::getline(std::cin, mensagem);
            std::cout << "Mensagem codificada: " << codificacao(mensagem, n, e) << "\n\n";
            break;
        case 3:
            std::cout << "\nInsira as chaves para decodificação (n d): ";
            std::cin >> n >> d;
            std::cin.ignore(); // Limpa o buffer
            std::cout << "Insira a mensagem: ";
            std::getline(std::cin, mensagem);
            std::cout << "Mensagem decodificada: " << decodificacao(mensagem, n, d) << "\n\n";
            break;
        default:
            std::cout << std::endl;
            return;
        }
    }
}
int main(){
    // Menu
    short int opcao{};
    while(true){
        std::cout << "\n~~~~~~~~~~~~~~~~~~~ Menu principal ~~~~~~~~~~~~~~~~~~~~\n\n";
        std::cout << "1) Fatorar um número\n2) Calcular o menor primo com n dígitos\n3) Codificar e decodificar uma mensagem usando RSA\n0) Encerrar\nOpção: ";
        std::cin >> opcao;
        switch (opcao){
        case 1:
            imprimeFatoracao();
            break;
        case 2:
            imprimeMinPrimo();
            break;
        case 3:
            criptografia();
            break;
        case 0:
            std::cout << "\nAplicação encerrada!" << std::endl;
            return 0;
        default:
            std::cout << "\nOpção inválida!\n\n";
        }
    }
}
