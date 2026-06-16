/* headers/ReservationStations.h */
#ifndef RESERVATION_STATIONS_H // Include guard
#define RESERVATION_STATIONS_H
#include <string>
#include <vector>
#include <utility>
#include "Instrucao.h"
#include "Componentes.h"

// classe:
class RS {
    public:
        // Construtor
        RS(std::string);
        // Getters
        bool                     getBusy() const;
        int                      getContagemRegressiva() const;
        int                      getPosicaoUF() const;
        Instrucao                getInstrucaoAtual() const;
        FaseInstrucao            getFaseInstrucao() const;
        std::string              getId() const;
        std::string              getQj() const;
        std::string              getQk() const;
        std::vector<int>         getTempos() const;
        std::vector<std::string> getInstrucoes() const;
        // Métodos públicos
        bool                     addIssue(Instrucao&, CDB&, int);
        bool                     atualizarDependencias(CDB&, UnidadesFuncionais&, int);
        bool                     atualizaContagem(UnidadesFuncionais&, int);
        void                     liberar(int);
        void                     resolverDependencia(const std::string& rs_id, const Registrador& valor);
    private:
        // Atributos
        bool                          busy{false};
        int                           contagem_regressiva_alocacao{-1};
        int                           posicao_UF{-1};
        Instrucao                     instrucao_atual;
        FaseInstrucao                 fase;
        std::string                   id;                  // Nome do RS ("load1", "addInt3", etc.)
        // Qj/Qk: {rs_id, ciclo_inicio} — par vazio {"", -1} significa operando disponível
        std::pair<std::string, int>   Qj{"", -1};
        std::pair<std::string, int>   Qk{"", -1};
        Registrador                   Vj;
        Registrador                   Vk;
        bool                          destino_pendente_no_cdb{false};
        std::vector<int>         tempos_alocacao;     // De 2 em 2, início da alocação e fim da alocação
        std::vector<std::string> instrucoes_alocacao; // De 1 em 1, instruções alocadas na RS
        // Métodos privados
        int                      procuraUFlivre(UnidadesFuncionais&, int, FaseInstrucao) const;
        void                     liberarUF(UnidadesFuncionais&, int, FaseInstrucao);
};

// struct:
struct ReservationStations {
    std::vector<RS>              load;
    std::vector<RS>              store;
    std::vector<RS>              int_basico;
    std::vector<RS>              int_mult_div;
    std::vector<RS>              float_basico;
    std::vector<RS>              float_mult_div;
};

#endif
