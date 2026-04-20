/* cripto.cpp */
#include "headers/cripto.h"
#include <string>
#include <iostream>
#include <vector>
// Debugging
static void printBlocos(std::vector<unsigned long long int> blocos){
    for(unsigned long long int bloco : blocos) std::cout << bloco << ' ';
    std::cout << std::endl;
}
// Elementos privados
// Uso comum:
// Multiplicação binária (mantém um valor máximo de bits, pois, para as operações modulares, não é necessário o valor completo)
static unsigned long long int modMul(unsigned long long int a, unsigned long long int b, const unsigned long long int mod) {
    /* Exemplo para ilustrar a lógica em 6 bits (resultado completo teria 7 bits):
       10 == 001010b => 10 * 10 = 001010b * 001010b = (001010b * 0b) + (010100b * 1b) + (101000b * 0b) + (010000b * 1b) = 010100b + 010000b = 100100b */
    unsigned long long int result{};
    a %= mod;
    while (b > 0) {
        if (b & 1) {
            if (result >= mod - a) result -= mod; // Previne o overflow do resultado (em relação à mod)
            result += a; // 2^0 * b[0] + 2^1 * b[1] + 2^2 * b[2] + ...
        }
        if (a >= mod - a) a -= (mod - a); // Previne o overflow de a (em relação à mod)
        else a += a; // a = 2 * a
        b >>= 1; // Shift para o próximo bit de b
    }
    return result;
} // Exponenciação binária (preserva os bits menores do que n a cada operação, tornando o processo mais eficiente e sem perdas)
static unsigned long long int modPow(const unsigned long long int base, unsigned long long int exp, const unsigned long long int mod) {
    /* Exemplo para ilustrar a lógica em 6 bits (resultado completo teria 17 bits):
       10 == 001010b => 10^5 = 001010b^101b = 001010b^1b * 001010b^0b * 001010b^100b = 001010b * (001010b * 001010b * 001010b * 001010b) = 001010b * 010000b = 100000b */
    unsigned long long int result{1};
    unsigned long long int b{base};
    while (exp > 0) {
        if (exp & 1)
            result = modMul(result, b, mod); // Multiplicação do resultado até agora pela potência da base
        b = modMul(b, b, mod); // Eleva a base atual ao quadrado (b -> b^2 -> b^4 -> b^16 -> ...)
        exp >>= 1; // Shift no expoente
    }
    return result;
} // Converte a mensagem em uma string de números
static std::string charToNum(const std::string &strInput, bool reverse){
    std::string strOutput{};
    if(!reverse) // String com letras para string com numero
        for(int i{}; strInput[i] != '\0'; i++)
            strOutput += std::to_string(0 + strInput[i]);
    else // String com numero para string com letras
        for(int i{}; strInput[i] != '\0'; i += 2){
            int aux{std::stoi(strInput.substr(i, 2))};
            if((aux < 65 || aux > 90) && aux != 32) aux = 122; // Falha na decodificação
            strOutput.push_back(static_cast<char>(aux));
        }
    strOutput.push_back('\0');
    return strOutput;
} // Codificação:
// Calcula o valor de d, tal que: (e * d) % phi == 1
static unsigned long long int modInverse(unsigned long long int e, unsigned long long int phi) {
    // Algoritmo de Euclides Estendido:
    // Invariante: r0 = t0*phi + ?*e e r1 = t1*phi + ?*e (não rastreamos o coeficiente de e)
    // Ao final, r0 == mdc(e, phi). Se r0 == 1, então t0*phi + ?*e == 1
    // Logo ?*e ≡ 1 (mod phi), ou seja, ? == e^-1 (mod phi)
    unsigned long long int r0{phi}, r1{e};
    unsigned long long int t0{0},  t1{1};
    while (r1 != 0) {
        unsigned long long int q{r0 / r1};
        // Atualiza o resto: r0, r1 = r1, r0 - q*r1 (passo do Euclides)
        unsigned long long int tmp{r1};
        r1 = r0 - q * r1;
        r0 = tmp;
        // Atualiza o coeficiente: t2 = t0 - q*t1
        // Como t pode ser negativo no algoritmo original, trabalhamos mod phi:
        // t0 - q*t1 (mod phi) == t0 + phi - (q*t1 % phi) (mod phi), sempre positivo
        tmp = t1;
        t1 = t0 + phi - modMul(q, t1, phi); // modMul evita overflow de q*t1
        if (t1 >= phi) t1 -= phi;           // Reduz se a soma ultrapassou phi
        t0 = tmp;
    }
    if (r0 != 1) return 0; // mdc(e, phi) != 1: inverso não existe
    return t0;             // t0 é o inverso modular de e em relação a phi
}
// Converte a string de números em blocos
static std::vector<unsigned long long int> numToBloco(const std::string &mensagem, const unsigned long long int &n){
    std::vector<unsigned long long int> blocos{};
    std::string aux{};
    for(int i{}; mensagem[i] != '\0'; i++){
        aux += mensagem[i];
        if(mensagem[i + 1] != '\0'){
            aux.push_back(mensagem[i + 1]);
            std::string nStr{std::to_string(n)};
            // Bloco < chave pública (método com prevenção de overflow de unsigned long long)
            if(aux.size() > nStr.size() || (aux.size() == nStr.size() && aux > nStr)){
                if(aux.back() == '0'){ // Garante que bloco[0] != 0
                    i--;
                    aux.pop_back();
                }
                aux.pop_back();
                blocos.push_back(std::stoull(aux));
                aux.clear();
            } else aux.pop_back(); // Apaga o elemento adicionado para teste
        } else { // Último elemento
            blocos.push_back(std::stoull(aux));
            aux.clear();
        }
    }
    return blocos;
}
static std::vector<unsigned long long int> preCodificacao(const std::string &mensagem, const unsigned long long int n){
    // Mensagem -> Numero -> Blocos
    return numToBloco(charToNum(mensagem, false), n);
}// Decodificação:
static std::vector<unsigned long long int> split(const std::string &mensagem) {
    std::vector<unsigned long long int> blocos{};
    std::string bloco{};
    // Separa a string pelos espaços
    for(int i{}; i <= mensagem.size(); i++){
        if((mensagem[i] == ' ' || mensagem[i] == '\0') && !bloco.empty()){
            // Caso o exista um espaço a mais no final, o bloco vazio causaria bugs
            blocos.push_back(std::stoull(bloco));
            bloco.clear();
        } else bloco.push_back(mensagem[i]);
    }
    return blocos;
}
// Converte os blocos numa string de números
static std::string blocoToNum(std::vector<unsigned long long int> blocos){
    std::string novoNumero{};
    for(unsigned long long int bloco : blocos) novoNumero += std::to_string(bloco);
    novoNumero.push_back('\0');
    blocos.clear();
    return novoNumero;
}
static std::string posDecodificacao(std::vector<unsigned long long int> blocos){
    // Blocos -> Números -> Mensagem
    return charToNum(blocoToNum(blocos), true);
}
// Elementos públicos
bool testaChaves(unsigned long long int &p, unsigned long long int &q, unsigned long long int &e){
    // Teste de overflow
    unsigned long long limiteULL = -1; // -1 vira 18446744073709551615 pelo complemento de dois
    if(p > limiteULL / q) return false; // p * q > limite do unsigned long long (transbordaria)
    // Teste de validade das chaves
    if((((p - 1) * (q - 1) % e) == 0) || (p == q)) return false;
    return true;
}
std::string getChaves(unsigned long long int &p, unsigned long long int &q, unsigned long long int &e){
    unsigned long long limiteULL = -1;
    if(p > limiteULL / q)
        return "Conjunto de chaves inválido! n >= 2^64 (limite do unsigned long long int)";
    if(((p - 1) * (q - 1) % e) == 0)
        return "Conjunto de chaves inválido! (p - 1) * (q - 1) % e) == 0";
    if(p == q) return "Conjunto de chaves inválido! p == q";
    // Calcula o valor do inverso modular de e para (p - 1) * (q - 1)
    unsigned long long int fi{(p - 1) * (q - 1)}, d{modInverse(e, fi)};
    std::string Output{"Chaves públicas (n, e): " +  std::to_string(p * q) + " " + std::to_string(e) + "\nChaves privadas (n, d): " + std::to_string(p * q) + " " + std::to_string(d)};
    return Output;
}
std::string codificacao(const std::string &mensagem, unsigned long long int &n, unsigned long long int &e){
    std::string mensagemEmBlocos{};
    // Cria os blocos
    std::vector<unsigned long long int> blocos{preCodificacao(mensagem, n)};
    // Codificação: C(b) = (b^e) % n
    for(unsigned long long int i{}; i != blocos.size(); i++){
        blocos[i] = modPow(blocos[i], e, n);
        mensagemEmBlocos += (std::to_string(blocos[i]) + ' ');
    }
    mensagemEmBlocos.back() = '\0';
    return mensagemEmBlocos;
}
std::string decodificacao(const std::string &mensagemEmBlocos, unsigned long long int &n, unsigned long long int &d){
    // Transforma a string recebida em blocos
    std::vector<unsigned long long int> blocos{split(mensagemEmBlocos)};
    // Imprime blocos decodificados (D(a) = (a^d) % n == (b^(e * d)) % n)
    for(unsigned long long int i{}; i != blocos.size(); i++) blocos[i] = modPow(blocos[i], d, n);
    return posDecodificacao(blocos);
}
