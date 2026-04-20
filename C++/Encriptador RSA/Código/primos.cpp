/* primos.cpp */
#include "headers/primos.h"
// Elementos privados
static std::vector<bool> crivo{}; // Armazena as posições válidas do Crivo
static std::vector<unsigned long long int> primos{};
// Implementação do algorítimo do "Crivo de Eratóstenes"
static void funcaoCrivo(const unsigned long long int &limite){ // Posições dos primos em lista
    crivo = std::vector<bool>(limite, true); // Define o tamanho do crivo
    crivo[0] = crivo[1] = false; // 0 e 1 não são primos mas não são compostos
    // Marca como false todos os valores compostos dos primos calculados até limite
    for(unsigned long long int i{2}; i * i < limite; i+=2){
        if(i == 2){ // Caso inicial, para permitir o crivo ser incrementado de 2 em 2 (aumentando a eficiência)
            i++; // Novo começo vai ser em i == 3, incriementando apenas números impares
        }
        if(crivo[i]) // É um primo
            for(unsigned long long int j{i * i}; j < limite; crivo[j] = false, j += i);
    }
}
// Elementos públicos
std::vector<unsigned long long int> getPrimos(){
    return primos;
}
// Calcula os primos até sqrt(valor)
unsigned long long int calculaPrimos(const unsigned long long int &valor){
    auto inicio = std::chrono::high_resolution_clock::now(); // Inicio contagem
    unsigned long long int limite{1};
    if(valor > 0) limite = static_cast<unsigned long long int>(std::sqrt(valor) + 1);
    funcaoCrivo(limite);
    if(crivo.size() >= 2){
        primos.push_back(2); // Caso inicial (se for >= 2)
        for(unsigned long long int i{3}; i < crivo.size(); i++) // Demais casos
            if(crivo[i]) primos.push_back(i);
    }
    auto fim = std::chrono::high_resolution_clock::now(); // Fim contagem
    crivo.clear();
    // Impressão do tempo
    auto duracao = std::chrono::duration_cast<MICRO>(fim - inicio);
    return duracao.count();
} // Conjectura de Legendre: n^2 < primo < (n + 1)^2
unsigned long long int primoMin(const unsigned long long int &potencia){
    // min = 10^(n - 1),  max = sucessor em potência de 10^(n - 1)
    unsigned long long int min{static_cast<unsigned long long int>(std::pow(10, potencia - 1))},
    max{static_cast<unsigned long long int>(std::pow(std::floor(std::sqrt(min)) + 1, 2))};
    calculaPrimos(max);
    for(unsigned long long int i{min + 1}; i < max; i++){
        bool ehPrimo{true};
        for(unsigned long long int primo : primos) if((i % primo || i == primo) == 0){
            ehPrimo = false;
            break;
        }
        if(ehPrimo){
            primos.clear();
            return i;
        }
    }
    return -1;
}
void clearPrimos(){
    primos.clear();
}
