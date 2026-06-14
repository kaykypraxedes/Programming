/* tempo.cpp */
#include "headers/tempo.h"
// Ajusta a escala para um formato "n minuto(s) e nn segundo(s)" ou "nnn "str"segundo(s)"
std::string ajustaEscala(unsigned long long int duracao){
    int i{};
    TEMPO tempo{};
    for(; i < 2 && duracao > 1000; i++, duracao /= 1000); // us - s
    tempo.resto = duracao;
    if(i == 2 && duracao >= 60){
        tempo.minutos = duracao / 60; // min
        tempo.resto = duracao % 60; // Ajuste dos segundos
    }
    switch(i){
    case 0:
        tempo.escala = "microssegundo(s)";
        break;
    case 1:
        tempo.escala = "milissegundo(s)";
        break;
    default:
        tempo.escala = "segundo(s)";
        break;
    }
    return (((tempo.minutos != 0) ? (std::to_string(tempo.minutos) + " minuto(s) e ") : "") + std::to_string(tempo.resto) + " " + tempo.escala);
}
