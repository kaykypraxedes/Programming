// ======================================================
// Para rodar: ./executavel < ../teste.txt
// ======================================================

#include "headers/Processador.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

constexpr int W_PC       = 6;
constexpr int W_INST     = 30;
constexpr int W_ISSUE    = 10;
constexpr int W_EX       = 16;
constexpr int W_MEM      = 16;
constexpr int W_WR       = 10;
constexpr int W_COMMIT   = 10;

// ======================================================
// Helpers
// ======================================================

std::string ciclo_str(int c) {
    return (c == 0 ? "--" : std::to_string(c));
}

std::string vec_str(const std::vector<int>& v) {
    if (v.empty()) return "--";

    std::ostringstream oss;

    for (int i{}; i < (int)v.size(); i++) {
        if (i) oss << '-';

        oss << std::setw(2)
            << std::setfill('0')
            << v[i];
    }

    return oss.str();
}

int largura_total(bool rob) {
    int total =
        W_PC +
        W_INST +
        W_ISSUE +
        W_EX +
        W_MEM +
        W_WR;

    if (rob)
        total += W_COMMIT;

    return total;
}

// ======================================================
// Leitura das configuracoes via stdin
// ======================================================

struct Config {
    int tipo         = 0;
    int num_threads  = 1;
    int granulacao   = 0;
    int despacho     = 2;
    std::vector<std::string> prog;
};

Config lerConfig() {
    Config cfg;
    std::string linha;

    while (std::getline(std::cin, linha)) {

        // Ignora comentarios e linhas vazias
        if (linha.empty() || linha[0] == '#')
            continue;

        std::istringstream iss(linha);
        std::string chave;
        iss >> chave;

        if (chave == "tipo")             { iss >> cfg.tipo;        }
        else if (chave == "num_threads") { iss >> cfg.num_threads; }
        else if (chave == "granulacao")  { iss >> cfg.granulacao;  }
        else if (chave == "despacho")    { iss >> cfg.despacho;      }
        else if (chave == "programa")    {
            // Le instrucoes ate encontrar END_PROG
            while (std::getline(std::cin, linha)) {
                if (linha == "END_PROG") break;
                if (!linha.empty() && linha[0] != '#')
                    cfg.prog.push_back(linha);
            }
        }
        // Ignora a secao CODIGO_FONTE e tudo abaixo
        else if (chave == "CODIGO_FONTE") { break; }
    }

    return cfg;
}

// ======================================================
// Impressao da tabela principal
// ======================================================

void imprimirTabela(
    const std::vector<LinhaTabela>& tabela,
    bool rob
) {
    std::cout << "=====================\n";
    std::cout << "TABELA DE RESULTADOS\n";
    std::cout << "=====================\n\n";

    std::cout << std::left
              << std::setw(W_PC)     << "PC"
              << std::setw(W_INST)   << "Instrucao"
              << std::setw(W_ISSUE)  << "Issue"
              << std::setw(W_EX)     << "EX"
              << std::setw(W_MEM)    << "MEM"
              << std::setw(W_WR)     << "WR";

    if (rob)
        std::cout << std::setw(W_COMMIT) << "Commit";

    std::cout << '\n';

    std::cout << std::string(largura_total(rob), '-') << '\n';

    for (const auto& l : tabela) {
        std::cout << std::left
                  << std::setw(W_PC)
                  << l.instrucao.getPC()

                  << std::setw(W_INST)
                  << l.instrucao.getInstrucaoString()

                  << std::setw(W_ISSUE)
                  << ciclo_str(l.ciclo_issue)

                  << std::setw(W_EX)
                  << vec_str(l.ciclo_EX)

                  << std::setw(W_MEM)
                  << vec_str(l.ciclo_MEM)

                  << std::setw(W_WR)
                  << ciclo_str(l.ciclo_WR);

        if (rob) {
            std::cout << std::setw(W_COMMIT)
                      << ciclo_str(l.ciclo_commit);
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}

// ======================================================
// Impressao generica de estruturas temporais
// ======================================================

template<typename T>
void imprimirEstrutura(
    const std::string& titulo,
    const std::vector<T>& estrutura
) {
    std::cout << titulo << ":\n";

    for (int j{}; j < (int)estrutura.size(); j++) {

        auto tempos = estrutura[j].getTempos();
        auto insts  = estrutura[j].getInstrucoes();

        if (tempos.empty())
            continue;

        std::cout << std::left
                  << std::setw(20)
                  << (titulo + std::to_string(j));

        for (int i{1}; i < (int)tempos.size(); i += 2) {

            std::cout
                << insts[(i - 1) / 2]
                << " ("
                << tempos[i - 1]
                << '-'
                << tempos[i]
                << ") ";

            if (i + 1 < (int)tempos.size())
                std::cout << "| ";
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}

// ======================================================
// Impressao generica de unidades funcionais
// ======================================================

template<typename T>
void imprimirUF(
    const std::string& titulo,
    const std::vector<T>& estrutura
) {
    std::cout << titulo << ":\n";

    for (int j{}; j < (int)estrutura.size(); j++) {

        auto tempos = estrutura[j].tempo_alocacao;
        auto rs     = estrutura[j].RS_alocadas;

        if (tempos.empty())
            continue;

        std::cout << std::left
                  << std::setw(24)
                  << (titulo + std::to_string(j));

        for (int i{1}; i < (int)tempos.size(); i += 2) {

            std::cout
                << rs[(i - 1) / 2]
                << " ("
                << tempos[i - 1]
                << '-'
                << tempos[i]
                << ") ";

            if (i + 1 < (int)tempos.size())
                std::cout << "| ";
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}

// ======================================================
// MAIN
// ======================================================

int main() {

    Config cfg = lerConfig();

    ModeloMultithreading modelo;
    switch (cfg.granulacao) {
        case 1:  modelo = ModeloMultithreading::GRANULACAO_GROSSA; break;
        case 2:  modelo = ModeloMultithreading::SMT;               break;
        default: modelo = ModeloMultithreading::GRANULACAO_FINA;   break;
    }

    Processador p(
        cfg.num_threads,
        cfg.despacho,
        false,
        (cfg.tipo == 1
            ? TipoProcessador::TOMASULO_COM_ROB
            : TipoProcessador::TOMASULO_SEM_ROB),
        modelo,
        cfg.prog
    );

    std::cout << "\nSimulando...\n";

    int resultado{};

    for (int c{}; c < 10000 && !resultado; c++) { // Máximo de ciclos = 10000
        resultado = p.executarCiclo();
    }

    std::cout
        << (resultado
            ? "Concluido!\n"
            : "Limite de 10000 ciclos atingido.\n");

    std::cout << '\n';

    // ==================================================
    // TABELA PRINCIPAL
    // ==================================================

    imprimirTabela(
        p.getTabelaThread(0),
        p.getTipo() == TipoProcessador::TOMASULO_COM_ROB
    );

    // ==================================================
    // RESERVATION STATIONS
    // ==================================================

    std::cout << "=====================\n";
    std::cout << "RESERVATION STATIONS\n";
    std::cout << "=====================\n\n";

    auto rs = p.getThread(0).getRS();

    imprimirEstrutura("load", rs.load);
    imprimirEstrutura("store", rs.store);
    imprimirEstrutura("int_basico", rs.int_basico);
    imprimirEstrutura("int_mult_div", rs.int_mult_div);
    imprimirEstrutura("float_basico", rs.float_basico);
    imprimirEstrutura("float_mult_div", rs.float_mult_div);

    // ==================================================
    // CDB
    // ==================================================

    std::cout << "=====================\n";
    std::cout << "CDB\n";
    std::cout << "=====================\n\n";

    auto cdb = p.getThread(0).getCDB();

    std::cout << "F:\n";

    for (int j{}; j < 32; j++) {

        auto tempos = cdb.F[j].getTempoAlocacao();
        auto rsaloc = cdb.F[j].getRSalocadas();

        if (tempos.empty())
            continue;

        std::cout << std::setw(8)
                  << ("F" + std::to_string(j));

        for (int i{1}; i < (int)tempos.size(); i += 2) {

            std::cout
                << rsaloc[(i - 1) / 2]
                << " ("
                << tempos[i - 1]
                << '-'
                << tempos[i]
                << ") ";

            if (i + 1 < (int)tempos.size())
                std::cout << "| ";
        }

        std::cout << '\n';
    }

    std::cout << "\nR:\n";

    for (int j{}; j < 32; j++) {

        auto tempos = cdb.R[j].getTempoAlocacao();
        auto rsaloc = cdb.R[j].getRSalocadas();

        if (tempos.empty())
            continue;

        std::cout << std::setw(8)
                  << ("R" + std::to_string(j));

        for (int i{1}; i < (int)tempos.size(); i += 2) {

            std::cout
                << rsaloc[(i - 1) / 2]
                << " ("
                << tempos[i - 1]
                << '-'
                << tempos[i]
                << ") ";

            if (i + 1 < (int)tempos.size())
                std::cout << "| ";
        }

        std::cout << '\n';
    }

    std::cout << '\n';

    // ==================================================
    // UNIDADES FUNCIONAIS
    // ==================================================

    std::cout << "=====================\n";
    std::cout << "UNIDADES FUNCIONAIS\n";
    std::cout << "=====================\n\n";

    auto uf = p.getThread(0).getUF();

    imprimirUF("acessar_memoria", uf.acessar_memoria);
    imprimirUF("ula_int_basico", uf.ula_int_basico);
    imprimirUF("ula_int_mult_div", uf.ula_int_mult_div);
    imprimirUF("ula_float_basico", uf.ula_float_basico);
    imprimirUF("ula_float_mult_div", uf.ula_float_mult_div);

    return 0;
}
