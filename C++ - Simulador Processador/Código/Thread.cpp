/* Thread.cpp */
#include "headers/Thread.h"
#include "headers/Componentes.h"
#include "headers/Instrucao.h"
#include <string>
#include <vector>
#include <algorithm>

// ─── Helpers de inicialização ─────────────────────────────────────────────────
void Thread::inicializarComponentes(std::vector<int> num_rs, std::vector<int> num_ufs){
    // Inicia o CDB
    for(int i{}; i < 32; i++){
        cdb.R.push_back(Registrador("R" + std::to_string(i)));
        cdb.F.push_back(Registrador("F" + std::to_string(i)));
    }
    // Inicia os Reservation Stations
    std::vector<int> aux;
    if(num_rs.size() >= 6) aux = num_rs;
    else aux = {5,5,5,4,3,2};
    for(int i{}; i < aux[0]; i++) rs.load.push_back(RS("load" + std::to_string(i)));
    for(int i{}; i < aux[1]; i++) rs.store.push_back(RS("store" + std::to_string(i)));
    for(int i{}; i < aux[2]; i++) rs.int_basico.push_back(RS("int_basico" + std::to_string(i)));
    for(int i{}; i < aux[3]; i++) rs.int_mult_div.push_back(RS("int_mult_div" + std::to_string(i)));
    for(int i{}; i < aux[4]; i++) rs.float_basico.push_back(RS("float_basico" + std::to_string(i)));
    for(int i{}; i < aux[5]; i++) rs.float_mult_div.push_back(RS("float_mult_div" + std::to_string(i)));
    // Inicia as Unidades Funcionais (num_ufs por tipo)
    if(num_ufs.size() >= 7) aux = num_ufs; // Sempre é passado, pelo menos o uf.commit
    else aux = {1,1,1,1,1,2, num_ufs.empty() ? 1 : num_ufs[0]};
    for(int i{}; i < aux[0]; i++) uf.acessar_memoria.push_back(UF{false, 0});
    for(int i{}; i < aux[1]; i++) uf.ula_int_basico.push_back(UF{false, 0});
    for(int i{}; i < aux[2]; i++) uf.ula_int_mult_div.push_back(UF{false, 0});
    for(int i{}; i < aux[3]; i++) uf.ula_float_basico.push_back(UF{false, 0});
    for(int i{}; i < aux[4]; i++) uf.ula_float_mult_div.push_back(UF{false, 0});
    uf.wr     = aux[5];
    uf.commit = aux[6];
    // Inicia o ROB (se for Tomasulo com especulação)
    if(tem_rob) capacidade_rob = 30;
}

// ─── Construtores ─────────────────────────────────────────────────────────────
Thread::Thread(std::vector<std::string> assembly,
               bool tem_rob,
               std::vector<int> num_rs,
               std::vector<int> num_ufs)
    : tem_rob(tem_rob)
{
    int i{};
    for (std::string& instr : assembly)
        tabela_de_instrucoes.push_back({Instrucao(i++, instr), 0, 0, {}, {}, 0, 0});
    inicializarComponentes(num_rs, num_ufs);
}

Thread::Thread(std::vector<int> instrucoes_troca,
               std::vector<std::string> assembly,
               bool tem_rob,
               std::vector<int> num_rs,
               std::vector<int> num_ufs)
               : tem_rob(tem_rob), instrucoes_troca(instrucoes_troca)
{
    int i{};
    for (std::string& instr : assembly)
        tabela_de_instrucoes.push_back({Instrucao(i++, instr), 0, 0, {}, {}, 0, 0});
    inicializarComponentes(num_rs, num_ufs);
}

// ─── Getters ──────────────────────────────────────────────────────────────────
int                      Thread::getPC()           const { return PC; }
int                      Thread::getNumStalls()    const { return num_stalls; }
EstadoThread             Thread::getEstadoThread() const { return estado; }
CDB                      Thread::getCDB()          const { return cdb; }
ReservationStations      Thread::getRS()           const { return rs; }
UnidadesFuncionais       Thread::getUF()           const { return uf; }
std::vector<LinhaTabela> Thread::getTabela()       const { return tabela_de_instrucoes; }

// ─── definirLatenciaEspecifica ────────────────────────────────────────────────
void Thread::definirLatenciaEspecifica(int posicao, int latencia_EX, int latencia_MEM) {
    tabela_de_instrucoes[posicao].instrucao.setLatenciaEX(latencia_EX);
    if (latencia_MEM > 0)
        tabela_de_instrucoes[posicao].instrucao.setLatenciaMEM(latencia_MEM);
}

// ─── executarExMem ────────────────────────────────────────────────────────────
bool Thread::executarExMem(int ciclo) {
    if((num_instrucoes_commitadas == tabela_de_instrucoes.size()) || // Tem ROB
       (!tem_rob && (num_instrucoes_finalizadas == tabela_de_instrucoes.size()))) // Não tem ROB
       return true;
    // Commit foi movido para executarCommitPublico(), chamado após executarWr()
    // no Processador, para que ciclo_WR já esteja preenchido.
    if (estado == EstadoThread::ESPERA)
        estado = EstadoThread::LIBERADA;
    if(num_instrucoes_finalizadas != tabela_de_instrucoes.size()) executaExMemTodos(ciclo);
    return false;
}

void Thread::executarCommitPublico(int ciclo) {
    if(tem_rob) executarCommit(ciclo);
}

// ─── executarWr ───────────────────────────────────────────────────────────────
void Thread::executarWr(int ciclo) {
    // Flush dos pendentes do ciclo anterior → buffer principal → consome (WR)
    for (int pc : buffer_WB_pendente)
        buffer_WB.push_back(pc);
    buffer_WB_pendente.clear();
    consumirBufferWB(ciclo);
    // Detecta fins de fase neste ciclo → vai para buffer_WB_pendente
    executaWrTodos(ciclo);
    // Verifica branch no buffer pendente para travar a thread sem ROB
    if (!tem_rob) {
        for (int i : buffer_WB_pendente) {
            if (tabela_de_instrucoes[i].instrucao.getTipoInstrucao() == TipoInstrucao::BRANCH)
                estado = EstadoThread::ESPERA;
        }
    }
}

// ─── executarIssue ────────────────────────────────────────────────────────────
// FIX: chaves {} no corpo do for para que o return false seja do if externo,
//      não execute sempre após a primeira iteração
bool Thread::executarIssue(int ciclo) {
    if (estado == EstadoThread::BLOQUEADA) return false;
    if (PC >= static_cast<int>(tabela_de_instrucoes.size())) return false;
    if (rob.size() >= capacidade_rob) return false;

    Instrucao& instrucao = tabela_de_instrucoes[PC].instrucao;
    TipoInstrucao tipo   = instrucao.getTipoInstrucao();
    if (tipo == TipoInstrucao::NAO_EXISTE) { PC++; return false; }

    std::vector<RS>* grupo = nullptr;
    switch (tipo) {
        case TipoInstrucao::LOAD:         grupo = &rs.load;          break;
        case TipoInstrucao::STORE:        grupo = &rs.store;         break;
        case TipoInstrucao::FLOAT_BASICO: grupo = &rs.float_basico;  break;
        case TipoInstrucao::INT_MUL:
        case TipoInstrucao::INT_DIV:      grupo = &rs.int_mult_div;  break;
        case TipoInstrucao::FLOAT_MUL:
        case TipoInstrucao::FLOAT_DIV:    grupo = &rs.float_mult_div;break;
        default:                          grupo = &rs.int_basico;    break;
    }

    for (RS& r : *grupo) {
        if (r.addIssue(instrucao, cdb, ciclo)) {
            tabela_de_instrucoes[PC].ciclo_issue = ciclo;
            tabela_de_instrucoes[PC].posicao_PC  = PC;
            if(tem_rob) rob.push_back(instrucao); // Coloca a instrução no ROB
            PC++;
            // Sem ROB: marca o PC do branch para bloquear EX das instruções posteriores
            if (tipo == TipoInstrucao::BRANCH && !tem_rob)
                pc_branch_nao_resolvido = PC - 1; // PC já foi incrementado
            return true;
        }
    }
    return false;
}

// ─── executarCommit ───────────────────────────────────────────────────────────
void Thread::executarCommit(int ciclo){
    int escritas{};
    while (!rob.empty() && escritas < uf.commit){
        LinhaTabela& linha{tabela_de_instrucoes[ponteiro_commit]};
        TipoInstrucao tipo = linha.instrucao.getTipoInstrucao();
        bool store_com_rob = (tipo == TipoInstrucao::STORE && tem_rob);
        bool pronto = false;

        if (store_com_rob) {
            if (linha.ciclo_MEM.empty()) {
                // Indica o ciclo que a instrução de STORE foi mandada para o COMMIT (tempo de MEM teórico)
                linha.ciclo_MEM.push_back(ciclo);
            }
            if (linha.ciclo_MEM.size() == 1) { // Verifica se já passou a latência de MEM para executar o commit
                int fim_mem = linha.ciclo_MEM[0] + linha.instrucao.getLatenciaMEM() - 1;
                if (ciclo >= fim_mem) {
                    linha.ciclo_MEM.pop_back(); // regulariza o MEM (--)
                    pronto = true;
                }
            }
        } else if (tipo == TipoInstrucao::BRANCH) {
            // Branch: pronto quando EX terminou
            pronto = (linha.ciclo_EX.size() == 2);
        } else {
            // Instrução comum: pronta após WR, e o commit deve ser em ciclo posterior ao WR
            pronto = (linha.ciclo_WR != 0 && linha.ciclo_WR < ciclo);
        }

        if (pronto) {
            linha.ciclo_commit = ciclo;
            num_instrucoes_commitadas++;
            escritas++;
            ponteiro_commit++;
            rob.erase(rob.begin());
            // Sem previsor: branch resolve no commit, instruções posteriores
            // só podem fazer commit no ciclo seguinte
            if (tipo == TipoInstrucao::BRANCH && !(tem_previsor && tem_rob)) break;
        }
        else break;
    }
}

// ─── executaExMemTodos ────────────────────────────────────────────────────────
// atualizarDependencias retorna true quando a instrução COMEÇA uma fase (EX ou MEM).
// Para LOAD/STORE, o início de MEM já é registrado em executaWrTodos junto com o
// fechamento de EX — portanto não o duplicamos aqui.
void Thread::executaExMemTodos(int ciclo) {
    // Coleta todas as RS ocupadas (de todos os grupos) que ainda não iniciaram
    // uma fase neste ciclo, e processa em ordem de PC. Assim, em caso de disputa
    // por uma UF compartilhada (ex.: ula_int_basico, usada tanto por INT_BASICO
    // quanto pelo cálculo de endereço de LOAD/STORE), a instrução com menor PC
    // (mais antiga) tem prioridade.
    std::vector<RS*> candidatas;
    auto coletar = [&](std::vector<RS>& grupo) {
        for (RS& r : grupo) {
            if (!r.getBusy()) continue;
            int inst_pc = r.getInstrucaoAtual().getPC();
            // Se há branch pendente, instruções despachadas APÓS ele não podem iniciar EX
            if (pc_branch_nao_resolvido >= 0 && inst_pc > pc_branch_nao_resolvido) continue;
            // STORE com ROB: após o fim do EX, a RS já está em fase MEM apenas
            // esperando ser drenada do buffer_WB (commit fará a escrita real).
            // Não deve disputar UF de memória nem re-entrar em atualizarDependencias.
            TipoInstrucao tipo = r.getInstrucaoAtual().getTipoInstrucao();
            if (tipo == TipoInstrucao::STORE && tem_rob && r.getFaseInstrucao() == FaseInstrucao::MEM)
                continue;
            candidatas.push_back(&r);
        }
    };
    coletar(rs.load);
    coletar(rs.store);
    coletar(rs.int_basico);
    coletar(rs.int_mult_div);
    coletar(rs.float_basico);
    coletar(rs.float_mult_div);

    std::sort(candidatas.begin(), candidatas.end(), [](RS* a, RS* b) {
        return a->getInstrucaoAtual().getPC() < b->getInstrucaoAtual().getPC();
    });

    for (RS* rp : candidatas) {
        RS& r = *rp;
        if (r.atualizarDependencias(cdb, uf, ciclo)) {
            int pc = r.getInstrucaoAtual().getPC();
            if (r.getFaseInstrucao() == FaseInstrucao::EX)
                tabela_de_instrucoes[pc].ciclo_EX.push_back(ciclo);
            else if (r.getFaseInstrucao() == FaseInstrucao::MEM)
                // Início real da fase MEM (LOAD, ou STORE sem ROB) — só ocorre
                // quando as dependências (Qj/Qk) já estão resolvidas.
                tabela_de_instrucoes[pc].ciclo_MEM.push_back(ciclo);
            // Se o próprio branch acabou de começar EX, não há mais bloqueio
            if (pc == pc_branch_nao_resolvido && r.getFaseInstrucao() == FaseInstrucao::EX)
                pc_branch_nao_resolvido = -1;
        }
    }
}

// ─── executaWrTodos ───────────────────────────────────────────────────────────
// atualizaContagem retorna true quando uma fase TERMINA.
// Após a chamada, r.getFaseInstrucao() já reflete a NOVA fase (MEM ou WB).
// Quando uma instrução entra em WB, emite broadcast imediato para que dependentes
// resolvam Qj/Qk antes do próximo executaExMemTodos.
//
// STORE com ROB: não passa por MEM antes do commit — após EX o endereço está
// calculado e a instrução fica pronta para commit (que escreve na memória).
// A fase MEM do STORE sem ROB continua funcionando normalmente.
void Thread::executaWrTodos(int ciclo) {
    // Coleta todos os eventos de fim de fase neste ciclo SEM ainda fechar a
    // tabela nem inserir no buffer_WB, para depois ordenar por PC e garantir
    // que instruções mais antigas façam WR primeiro (independente do grupo de RS).
    struct Evento {
        int           pc;
        FaseInstrucao fase_antes;
        FaseInstrucao fase_depois;
        TipoInstrucao tipo;
    };
    std::vector<Evento> eventos;

    auto coletar = [&](std::vector<RS>& grupo) {
        for (RS& r : grupo) {
            if (!r.getBusy()) continue;
            FaseInstrucao fase_antes = r.getFaseInstrucao();
            if (r.atualizaContagem(uf, ciclo)) {
                eventos.push_back({
                    r.getInstrucaoAtual().getPC(),
                    fase_antes,
                    r.getFaseInstrucao(),
                    r.getInstrucaoAtual().getTipoInstrucao()
                });
            }
        }
    };
    coletar(rs.load);
    coletar(rs.store);
    coletar(rs.int_basico);
    coletar(rs.int_mult_div);
    coletar(rs.float_basico);
    coletar(rs.float_mult_div);

    // Ordena por PC: instrução mais antiga tem prioridade no WR
    std::sort(eventos.begin(), eventos.end(), [](const Evento& a, const Evento& b) {
        return a.pc < b.pc;
    });

    for (const Evento& e : eventos) {
        int pc             = e.pc;
        bool tem_mem       = (e.tipo == TipoInstrucao::LOAD || e.tipo == TipoInstrucao::STORE);
        bool store_com_rob = (e.tipo == TipoInstrucao::STORE && tem_rob);

        if (e.fase_antes == FaseInstrucao::EX && e.fase_depois == FaseInstrucao::MEM) {
            // EX terminou → fecha ciclo_EX
            tabela_de_instrucoes[pc].ciclo_EX.push_back(ciclo);

            if (store_com_rob) {
                buffer_WB_pendente.push_back(pc);
            }
            // LOAD ou STORE sem ROB: o início real de MEM é registrado em
            // executaExMemTodos, no ciclo em que atualizarDependencias
            // efetivamente alocar a fase MEM (após Qj/Qk resolvidos).
        } else if (e.fase_depois == FaseInstrucao::WB) {
            // Fase final concluída
            if (tem_mem && !store_com_rob)
                tabela_de_instrucoes[pc].ciclo_MEM.push_back(ciclo);
            else if (!tem_mem)
                tabela_de_instrucoes[pc].ciclo_EX.push_back(ciclo);
            buffer_WB_pendente.push_back(pc);
        }
    }
}

// Mover para executarWr(), chamado após executaWrTodos():
void Thread::consumirBufferWB(int ciclo) {
    // Garante que instruções mais antigas (menor PC) façam WR primeiro,
    // independente da ordem em que entraram no buffer.
    std::sort(buffer_WB.begin(), buffer_WB.end());

    int escritas{};
    while (!buffer_WB.empty() && escritas < uf.wr) {
        int pc = buffer_WB.front();
        TipoInstrucao auxTipo{tabela_de_instrucoes[pc].instrucao.getTipoInstrucao()};
        bool store_com_rob = (auxTipo == TipoInstrucao::STORE && tem_rob);

        // STORE com ROB: entra no buffer apenas para liberar a RS; não tem WR nem
        // escrita no CDB (sem registrador destino). O commit registrará o MEM.
        if (store_com_rob) {
            // Libera a RS do store agora; não registra ciclo_WR nem consome vaga de WR
            // (store com ROB não usa o CDB — a escrita na memória ocorre no commit)
            auto liberarPorPC = [&](std::vector<RS>& grupo) {
                for (RS& r : grupo)
                    if (r.getBusy() && r.getInstrucaoAtual().getPC() == pc)
                        r.liberar(ciclo);
            };
            liberarPorPC(rs.store);
            buffer_WB.erase(buffer_WB.begin());
            num_instrucoes_finalizadas++;
            // escritas não incrementado: store com ROB não ocupa barramento WR
            continue;
        }

        if(auxTipo != TipoInstrucao::STORE && auxTipo != TipoInstrucao::BRANCH) tabela_de_instrucoes[pc].ciclo_WR = ciclo;
        Registrador dest = tabela_de_instrucoes[pc].instrucao.getRegDestino();
        // Broadcast: resolve Qj/Qk das RS dependentes APÓS o WR ser confirmado.
        // Também desaloca o CDB usando o hash (rs_id, ciclo_inicio) para evitar
        // ambiguidade em caso de WAW com reutilização do mesmo RS.
        {
            auto broadcast = [&](std::vector<RS>& grupo) {
                for (RS& r : grupo)
                    if (r.getBusy() && r.getFaseInstrucao() == FaseInstrucao::WB
                        && r.getInstrucaoAtual().getPC() == pc
                        && dest.getTipo() != 'Z' && dest.getId() >= 0) {
                        std::string rs_id = r.getId();
                        // Desaloca o CDB com hash exato: (rs_id, ciclo_inicio)
                        int ciclo_inicio = -1;
                        if (dest.getTipo() == 'F') {
                            ciclo_inicio = cdb.F[dest.getId()].getCicloInicioRS(rs_id);
                            cdb.F[dest.getId()].desalocarRS(rs_id, ciclo_inicio, ciclo);
                        } else if (dest.getTipo() == 'R') {
                            ciclo_inicio = cdb.R[dest.getId()].getCicloInicioRS(rs_id);
                            cdb.R[dest.getId()].desalocarRS(rs_id, ciclo_inicio, ciclo);
                        }
                        auto bcst = [&](std::vector<RS>& grp) {
                            for (RS& dep : grp)
                                if (dep.getBusy()) dep.resolverDependencia(rs_id, dest);
                        };
                        bcst(rs.load);    bcst(rs.store);
                        bcst(rs.int_basico);    bcst(rs.int_mult_div);
                        bcst(rs.float_basico);  bcst(rs.float_mult_div);
                    }
            };
            broadcast(rs.load);          broadcast(rs.store);
            broadcast(rs.int_basico);    broadcast(rs.int_mult_div);
            broadcast(rs.float_basico);  broadcast(rs.float_mult_div);
        }
        buffer_WB.erase(buffer_WB.begin());
        num_instrucoes_finalizadas++;
        // BRANCH não usa o CDB (sem registrador destino), então não consome vaga de WR
        if(auxTipo != TipoInstrucao::BRANCH) escritas++;
        TipoInstrucao t{tabela_de_instrucoes[pc].instrucao.getTipoInstrucao()};
        // FIX: para instruções sem destino (STORE, BRANCH), libera pelo PC
        auto liberarPorPC = [&](std::vector<RS>& grupo) {
            for (RS& r : grupo) {
                if (r.getBusy() && r.getFaseInstrucao() == FaseInstrucao::WB
                    && r.getInstrucaoAtual().getPC() == pc)
                    r.liberar(ciclo);
            }
        };
        if(t == TipoInstrucao::LOAD)
            liberarRScommit(rs.load, dest, ciclo);
        else if(t == TipoInstrucao::STORE)
            liberarPorPC(rs.store);
        else if(t == TipoInstrucao::BRANCH)
            liberarPorPC(rs.int_basico);
        else if(t == TipoInstrucao::FLOAT_BASICO)
            liberarRScommit(rs.float_basico, dest, ciclo);
        else if(t == TipoInstrucao::INT_MUL || t == TipoInstrucao::INT_DIV)
            liberarRScommit(rs.int_mult_div, dest, ciclo);
        else if(t == TipoInstrucao::FLOAT_MUL || t == TipoInstrucao::FLOAT_DIV)
            liberarRScommit(rs.float_mult_div, dest, ciclo);
        else liberarRScommit(rs.int_basico, dest, ciclo);
    }
}
void Thread::liberarRScommit(std::vector<RS>& rs, Registrador reg_destino, int ciclo){
    for(RS& r : rs){
        if(!r.getBusy()) continue;
        Registrador aux{r.getInstrucaoAtual().getRegDestino()};
        if(r.getFaseInstrucao() == FaseInstrucao::WB && // WB = pronta para liberar
            aux.getTipo() == reg_destino.getTipo() &&
            aux.getId() == reg_destino.getId())
            r.liberar(ciclo);
    }
}
