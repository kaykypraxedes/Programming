/* Processador.cpp */
#include "headers/Processador.h"
#include "headers/Thread.h"
#include <string>
#include <vector>

// ─── Construtor ───────────────────────────────────────────────────────────────
Processador::Processador(int                      num_threads,
                         int                      largura_de_despacho,
                         bool                     tem_previsor,
                         TipoProcessador          tipo,
                         ModeloMultithreading     modelo,
                         std::vector<std::string> Assembly,
                         std::vector<int>         num_rs,
                         std::vector<int>         num_ufs,
                         std::vector<int>         instrucoes_troca)
    : largura_de_despacho(largura_de_despacho),
    tem_previsor         (tem_previsor),
      tipo               (tipo),
      modelo_MT          (modelo)
{
    bool tem_rob{tipo == TipoProcessador::TOMASULO_COM_ROB};
    if (instrucoes_troca.empty()) iniciarThreads(Assembly, tem_rob, num_threads, num_rs, num_ufs);
    else                          iniciarThreads(Assembly, tem_rob, num_threads, num_rs, num_ufs, instrucoes_troca);
}

TipoProcessador Processador::getTipo() const{
    return tipo;
}

Thread Processador::getThread(int i) const{
    return threads[i];
}

// ─── executarCiclo ────────────────────────────────────────────────────────────
bool Processador::executarCiclo() {
    if(executarExMemWr()) return true;
    executarIssue();
    ciclo_atual++;
    return false;
}

std::vector<LinhaTabela> Processador::getTabelaThread(int i) const {
    return threads[i].getTabela();
}

// ─── executarExMemWr ─────────────────────────────────────────────────────────
// ExMem antes de Wr: a contagem é decrementada depois de registrar o início.
bool Processador::executarExMemWr() {
    bool executou_todos{true};
    for (Thread& t : threads) {
        executou_todos &= t.executarExMem(ciclo_atual);
        t.executarWr(ciclo_atual);
    }
    return executou_todos;
}

// ─── executarIssue ────────────────────────────────────────────────────────────
void Processador::executarIssue() {
    int despachadas   = 0;
    int voltas        = 0;
    int total_threads = static_cast<int>(threads.size());

    while (despachadas < largura_de_despacho) {
        if (voltas >= total_threads) break; // percorreu todas sem conseguir despachar

        bool ok = threads[ponteiro_da_thread].executarIssue(ciclo_atual);
        if (ok) {
            despachadas++;
            if(threads[ponteiro_da_thread].getTabela()[threads[ponteiro_da_thread].getPC() - 1].instrucao.getTipoInstrucao() ==
                TipoInstrucao::BRANCH && modelo_MT != ModeloMultithreading::SMT
                && !tem_previsor) // Instrução é um branch
                break;
            // GRANULACAO_FINA e SMT: sempre rotaciona após issue bem-sucedido
            // (garante alternância entre threads a cada despacho)
            if (modelo_MT == ModeloMultithreading::SMT) avancarPonteiroRoundRobin();
        } else {
            voltas++;
            if (modelo_MT == ModeloMultithreading::SMT || despachadas == 0)
                avancarPonteiroRoundRobin();
            else
                break;
        }
    }
}

// ─── avancarPonteiroRoundRobin ────────────────────────────────────────────────
// FIX: separado em duas operações para evitar undefined behavior
void Processador::avancarPonteiroRoundRobin() {
    ponteiro_da_thread = (ponteiro_da_thread + 1) % static_cast<int>(threads.size());
}

// ─── iniciarThreads ───────────────────────────────────────────────────────────
void Processador::iniciarThreads(std::vector<std::string> Assembly, bool tem_rob, int num_threads, std::vector<int> num_rs, std::vector<int> num_ufs, std::vector<int> instrucoes_troca)
{
    for (int i = 0; i < num_threads; i++) {
        if (modelo_MT == ModeloMultithreading::GRANULACAO_GROSSA)
            threads.push_back(Thread(instrucoes_troca, Assembly, tem_rob, num_rs, num_ufs));
        else
            threads.push_back(Thread(Assembly, tem_rob, num_rs, num_ufs));
    }
}
