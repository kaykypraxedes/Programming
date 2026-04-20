/* fatora.h */
#include <vector>
#include <string>
#include <iostream>
#include "tempo.h"
#include "primos.h"
#ifndef FATORA_H // Include guard
#define FATORA_H
// Struct com o 2 valores long long int
struct FATOR{ // Vai ser usado como {fator, multiplicidade} e {tempo, nº primos}
    unsigned long long int num1{};
    unsigned long long int num2{};
};
// Modifica um vetor de FATOR e retorna o tempo demorado para as operações
FATOR fatora(const unsigned long long int &);
#endif
