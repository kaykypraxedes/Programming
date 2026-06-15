/* headers/Instrucao.h */
#ifndef INSTRUCAO_H // Include guard
#define INSTRUCAO_H
#include <cctype>
#include <string>
#include <vector>
#include "Componentes.h"

//enums:
enum class TipoInstrucao {
    NAO_EXISTE,
    LOAD,
    STORE,
    BRANCH,
    INT_BASICO,
    INT_MUL,
    INT_DIV,
    FLOAT_BASICO,
    FLOAT_MUL,
    FLOAT_DIV
};
enum class FaseInstrucao {
    ISSUE,
    EX,
    MEM,
    WB,
    COMMIT
};

// classe:
class Instrucao {
    public:
        // Elementos static
        static std::vector<int>  latencias_basicas_EX;
        static std::vector<int>  latencias_basicas_MEM;
        // Construtores
        Instrucao() : PC(-1), latencia_EX(0), latencia_MEM(0), tipo(TipoInstrucao::NAO_EXISTE) {}
        Instrucao(int, std::string);
        // Getters e setters
        int                      getPC() const;
        int                      getLatenciaEX() const;
        void                     setLatenciaEX(int);
        int                      getLatenciaMEM() const;
        void                     setLatenciaMEM(int);
        TipoInstrucao            getTipoInstrucao() const;
        Registrador              getRegDestino() const;
        Registrador              getJ() const;
        Registrador              getK() const;
        std::string              getInstrucaoString() const;
    private:
        // Atributos (instrucao_em_string DEVE vir antes de PC para que o
        // initializer list a inicialize antes de splitInstrucao() ser chamado)
        std::string              instrucao_em_string;
        int                      PC{};
        int                      latencia_EX;
        int                      latencia_MEM{};
        TipoInstrucao            tipo;
        Registrador              reg_destino;
        Registrador              reg_J;
        Registrador              reg_K;
        // Métodos privados
        void                     stringToInstrucao();
        std::vector<std::string> splitInstrucao();
        void                     identificaTipo(std::string&);
        void                     defineLatencias();
        void                     defineAtributos(std::vector<std::string>&);
};
#endif
