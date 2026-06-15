/* headers/Thread.h */
#ifndef THREAD_H // Include guard
#define THREAD_H
#include <string>
#include <vector>
#include "Instrucao.h"
#include "Componentes.h"
#include "ReservationStations.h"

// enum:
enum class EstadoThread {
    LIBERADA,
    ESPERA,
    BLOQUEADA
};

// structs:
struct LinhaTabela{
    Instrucao                instrucao;
    int                      posicao_PC{-1};
    int                      ciclo_issue{-1};
    std::vector<int>         ciclo_EX;  // Operação multiciclos (recebe ciclo de início e o ciclo de encerramento)
    std::vector<int>         ciclo_MEM; // Operação multiciclos
    int                      ciclo_WR{-1};  // Exclusivo do Tomasulo com especulação (ROB)
    int                      ciclo_commit{-1};
};

// classe:
class Thread {
public:
    // Construtores
    Thread(std::vector<std::string>, bool, std::vector<int> = {}, std::vector<int> = {}); // Thread com granulação fina ou SMT
    Thread(std::vector<int>, std::vector<std::string>, bool, std::vector<int> = {}, std::vector<int> = {}); // Thread com granulação grossa
    // Getters
    int                      getPC() const;
    int                      getNumStalls() const;
    EstadoThread             getEstadoThread() const;
    CDB                      getCDB() const;
    ReservationStations      getRS() const;
    UnidadesFuncionais       getUF() const;
    std::vector<LinhaTabela> getTabela() const;
    // Métodos públicos
    bool                     executarExMem(int);
    void                     executarWr(int);
    bool                     executarIssue(int);
    void                     definirLatenciaEspecifica(int, int, int=0);
private:
    // Atributos
    bool                     tem_rob{false};
    bool                     tem_previsor{false};
    int                      PC{};
    int                      capacidade_rob{1};
    int                      num_instrucoes_finalizadas{};
    int                      num_instrucoes_commitadas{};
    int                      num_stalls{};
    int                      ponteiro_commit{}; // ROB que age na tabela de instruções
    int                      pc_branch_nao_resolvido{-1}; // >= 0 enquanto branch sem ROB não começou EX
    EstadoThread             estado{EstadoThread::LIBERADA};
    CDB                      cdb;
    ReservationStations      rs;
    UnidadesFuncionais       uf;
    std::vector<int>         buffer_WB;
    std::vector<int>         instrucoes_troca;
    std::vector<Instrucao>   rob;
    std::vector<LinhaTabela> tabela_de_instrucoes;
    // Métodos privados
    void                     inicializarComponentes(std::vector<int>, std::vector<int>);
    void                     executarCommit(int);
    void                     consumirBufferWB(int);
    void                     executaExMemTodos(int);
    void                     executaWrTodos(int);
    void                     liberarRScommit(std::vector<RS>&, Registrador, int);
};

#endif
