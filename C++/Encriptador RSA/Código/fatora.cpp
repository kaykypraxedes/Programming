/* fatora.cpp */
#include "headers/fatora.h"
// Elementos privados
static std::vector<FATOR> fatores{};
static std::string stringFatores(const unsigned long long int &valor){
    std::string numeroFatorado{std::to_string(valor) + " = "};
    for(FATOR fator : fatores){
        std::string pot{fator.num2 != 1 ? ("^" + std::to_string(fator.num2)) : ""};
        numeroFatorado += (std::to_string(fator.num1) + pot);
        numeroFatorado += (fator.num1 != fatores.back().num1 ? " * " : "");
    }
    if(fatores.size() == 1 && fatores.back().num2 == 1 && (fatores.back().num1 != 1 && fatores.back().num1 != 0)) numeroFatorado += " (primo)";
    numeroFatorado.push_back('\0');
    fatores.clear();
    return numeroFatorado;
}
// Elementos públicos
FATOR fatora(const unsigned long long int &valor){
    if(valor == 0){
        fatores.push_back({0,1});
        std::cout << stringFatores(valor) << "\n\n";
        return FATOR{0,0};
    }
    unsigned long long int copyValor{valor};
    // Output receberá tanto o tempo da fatoração quanto o número de primos calculados
    FATOR Output{calculaPrimos(valor), static_cast<unsigned long long int>(getPrimos().size())};
    for(unsigned long long int primo : getPrimos()){ // Fatora o valor
        if(primo > copyValor) break; // Fim
        while(copyValor % primo == 0){
            if(fatores.empty() || fatores.back().num1 != primo) fatores.push_back({primo, 0});
            copyValor /= primo;
            fatores.back().num2++;
        }
    } // primo || numero composto com um fator maior do que sqrt(n)
    if(fatores.empty() || copyValor != 1) fatores.push_back({copyValor, 1});
    std::cout << stringFatores(valor) << "\n\n";
    clearPrimos();
    return Output;
}
