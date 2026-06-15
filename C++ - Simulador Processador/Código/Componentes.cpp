/* Componentes.cpp */
#include "headers/Componentes.h"

// Construtores
Registrador::Registrador(){}
Registrador::Registrador(std::string registrador_em_string){
    tipo = identificaTipo(registrador_em_string);
    id   = identificaId(registrador_em_string);
}

// Getters
char                     Registrador::getTipo()          const { return tipo;}
int                      Registrador::getId()            const { return id;}
bool                     Registrador::getBusy()          const { return busy;}
// getRSatual: retorna o produtor mais recente (back da fila).
// É este que novas instruções devem aguardar em caso de WAW.
std::string              Registrador::getRSatual()       const {
    return fila_produtores.empty() ? "" : fila_produtores.back();
}
bool                     Registrador::temProdutorPendente() const { return !fila_produtores.empty(); }
std::vector<int>         Registrador::getTempoAlocacao() const { return tempo_alocacao;}
std::vector<std::string> Registrador::getRSalocadas()    const { return RS_alocadas;}

// Mutadores
void Registrador::trocaBusy() { busy = !busy; }

// alocarRS: empurra o novo produtor no back da fila.
// Múltiplos produtores pendentes (WAW) ficam enfileirados.
void Registrador::alocarRS(std::string& rs, int inicio) {
    busy     = true;
    fila_produtores.push_back(rs); // novo produtor entra no back
    RS_alocadas.push_back(rs);
    tempo_alocacao.push_back(inicio);
}

// desalocarRS: remove apenas o produtor mais antigo (front).
// Se ainda há outros produtores na fila, busy permanece true e
// getRSatual() continua retornando o próximo produtor pendente.
void Registrador::desalocarRS(int fim) {
    if (!fila_produtores.empty())
        fila_produtores.pop_front(); // o produtor mais antigo terminou
    busy = !fila_produtores.empty();
    tempo_alocacao.push_back(fim);
}

// Privados
char Registrador::identificaTipo(std::string& s) {
    if (s.empty()) return 'Z';
    char c = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
    if (c == 'F') return 'F';
    if (c == 'R') return 'R';
    return 'Z';
}

int Registrador::identificaId(std::string& s) {
    if (s.size() < 2 || !std::isdigit(static_cast<unsigned char>(s[1]))) return -1;
    return std::stoi(s.substr(1));
}
