/* headers/Componentes.h */
#ifndef COMPONENTES_H // Include guard
#define COMPONENTES_H
#include <string>
#include <vector>
#include <deque>
#include <cctype>


// Classe
class Registrador {
    public:
        // Construtores
        Registrador();
        Registrador(std::string);
        // Getters e setters
        char                     getTipo() const;
        int                      getId() const;
        bool                     getBusy() const;
        void                     trocaBusy();
        std::vector<int>         getTempoAlocacao() const;
        std::vector<std::string> getRSalocadas() const;
        std::string              getRSatual() const;  // retorna o produtor mais recente (back da fila)
        bool                     temProdutorPendente() const; // true se fila não vazia
        // Métodos públicos
        void                     alocarRS(std::string&, int);
        void                     desalocarRS(int);
    private:
        // Atributos
        char                     tipo{'Z'};
        int                      id;
        bool                     busy{false};
        // WAW fix: fila de produtores pendentes.
        // front = produtor mais antigo (que termina primeiro),
        // back  = produtor mais recente (que novas dependências devem aguardar).
        std::deque<std::string>  fila_produtores;
        std::vector<int>         tempo_alocacao;
        std::vector<std::string> RS_alocadas;
        // Métodos privados
        char                     identificaTipo(std::string&);
        int                      identificaId(std::string&);
};

// structs:
struct CDB {
    std::vector<Registrador> R;
    std::vector<Registrador> F;
};
struct UF {
    bool                     busy;
    int                      id;
    std::string              RS_atual;
    std::vector<int>         tempo_alocacao; // De 2 em 2 (tempo de uso do início e do final da alocação)
    std::vector<std::string> RS_alocadas;    // De 1 em 1
};
struct UnidadesFuncionais {
    std::vector<UF>          acessar_memoria;
    std::vector<UF>          ula_int_basico;
    std::vector<UF>          ula_int_mult_div;
    std::vector<UF>          ula_float_basico;
    std::vector<UF>          ula_float_mult_div;
    int                      wr{1};
};
#endif
