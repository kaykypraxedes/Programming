/* ReservationStations.cpp */
#include "headers/ReservationStations.h"
#include "headers/Componentes.h"
#include "headers/Instrucao.h"
#include <string>

// Construtor:
RS::RS(std::string id) : id(id){}
// ─── Getters ──────────────────────────────────────────────────────────────────
bool                     RS::getBusy()               const { return busy; }
int                      RS::getContagemRegressiva() const { return contagem_regressiva_alocacao; }
int                      RS::getPosicaoUF()          const { return posicao_UF; }
Instrucao                RS::getInstrucaoAtual()     const { return instrucao_atual; }
FaseInstrucao            RS::getFaseInstrucao()      const { return fase; }
std::string              RS::getId()                 const { return id; }
std::string              RS::getQj()                 const { return Qj; }
std::string              RS::getQk()                 const { return Qk; }
std::vector<int>         RS::getTempos()             const { return tempos_alocacao; }
std::vector<std::string> RS::getInstrucoes()         const { return instrucoes_alocacao; }

// ─── addIssue ─────────────────────────────────────────────────────────────────
// Lê Qj/Qk do CDB ANTES de marcar o destino para não se auto-bloquear.
// Se J ou K coincide com o destino (WAR), o operando lido é o valor ATUAL
// do registrador (disponível), e o destino é reservado normalmente no CDB.
// Não há motivo para adiar a reserva do destino — dependentes emitidos depois
// precisam ver essa reserva para se bloquear corretamente.
bool RS::addIssue(Instrucao& instrucao, CDB& cdb, int ciclo) {
    if (busy) return false;
    busy            = true;
    instrucao_atual = instrucao;
    fase            = FaseInstrucao::ISSUE;
    contagem_regressiva_alocacao = -1;
    posicao_UF                   = -1;
    Qj = Qk = "";
    Vj = Vk = Registrador{};
    destino_pendente_no_cdb = false;
    instrucoes_alocacao.push_back(instrucao.getInstrucaoString());
    tempos_alocacao.push_back(ciclo);

    Registrador dest = instrucao.getRegDestino();
    bool dest_valido = (dest.getTipo() != 'Z' && dest.getId() >= 0);

    // Lê dependências ANTES de marcar o destino para não se auto-bloquear (WAR).
    // Se J ou K é o mesmo registrador que o destino, o valor atual está disponível
    // (esta instrução vai sobrescrevê-lo, mas lê o valor antigo).
    Registrador regJ = instrucao.getJ();
    Registrador regK = instrucao.getK();

    auto mesmo_reg = [](const Registrador& a, const Registrador& b) {
        return a.getTipo() != 'Z' && a.getTipo() == b.getTipo() && a.getId() == b.getId();
    };

    if (regJ.getTipo() != 'Z' && regJ.getId() >= 0) {
        if (dest_valido && mesmo_reg(regJ, dest)) {
            Vj = regJ; // WAR: lê valor atual, que está disponível
        } else {
            std::string tag = (regJ.getTipo() == 'F')
                ? cdb.F[regJ.getId()].getRSatual()
                : cdb.R[regJ.getId()].getRSatual();
            if (tag.empty()) Vj = regJ;
            else             Qj = tag;
        }
    }
    if (regK.getTipo() != 'Z' && regK.getId() >= 0) {
        if (dest_valido && mesmo_reg(regK, dest)) {
            Vk = regK; // WAR: lê valor atual, que está disponível
        } else {
            std::string tag = (regK.getTipo() == 'F')
                ? cdb.F[regK.getId()].getRSatual()
                : cdb.R[regK.getId()].getRSatual();
            if (tag.empty()) Vk = regK;
            else             Qk = tag;
        }
    }

    // Marca o destino no CDB agora, sempre.
    // Dependentes emitidos depois precisam ver esta reserva para se bloquear.
    if (dest_valido) {
        if      (dest.getTipo() == 'F') cdb.F[dest.getId()].alocarRS(id, ciclo);
        else if (dest.getTipo() == 'R') cdb.R[dest.getId()].alocarRS(id, ciclo);
    }
    return true;
}

// ─── atualizarDependencias ────────────────────────────────────────────────────
// Resolve Qj/Qk consultando o CDB. Quando prontos, aloca UF e inicia contagem.
// Retorna true apenas no ciclo em que a instrução COMEÇA a executar.
bool RS::atualizarDependencias(CDB& cdb, UnidadesFuncionais& uf, int ciclo) {
    if (!busy || contagem_regressiva_alocacao != -1) return false;

    Registrador regJ = instrucao_atual.getJ();
    Registrador regK = instrucao_atual.getK();

    // Resolver Vj / Qj (só se ainda não resolvido)
    if (Vj.getTipo() == 'Z' && !Qj.empty()) {
        // Qj estava pendente — verifica se já foi produzido
        std::string tag = (regJ.getTipo() == 'F')
            ? cdb.F[regJ.getId()].getRSatual()
            : cdb.R[regJ.getId()].getRSatual();
        if (tag.empty()) { Vj = regJ; Qj = ""; }
    }
    // Resolver Vk / Qk (só se ainda não resolvido)
    if (Vk.getTipo() == 'Z' && !Qk.empty()) {
        std::string tag = (regK.getTipo() == 'F')
            ? cdb.F[regK.getId()].getRSatual()
            : cdb.R[regK.getId()].getRSatual();
        if (tag.empty()) { Vk = regK; Qk = ""; }
    }

    TipoInstrucao tipo = instrucao_atual.getTipoInstrucao();
    bool load_store = (tipo == TipoInstrucao::LOAD || tipo == TipoInstrucao::STORE);

    if (load_store) {
        if (fase == FaseInstrucao::ISSUE && Qk.empty()) {
            posicao_UF = procuraUFlivre(uf, ciclo, FaseInstrucao::EX);
            if (posicao_UF == -1) return false;
            contagem_regressiva_alocacao = instrucao_atual.getLatenciaEX();
            fase = FaseInstrucao::EX;
            return true;
        }
        // Após atualizaContagem, fase já foi avançada para MEM (EX terminou)
        // Aqui alocamos a UF de memória e iniciamos a contagem de MEM
        if (fase == FaseInstrucao::MEM && contagem_regressiva_alocacao == -1
            && ((tipo == TipoInstrucao::STORE && Qj.empty()) || tipo == TipoInstrucao::LOAD)) {
            posicao_UF = procuraUFlivre(uf, ciclo, FaseInstrucao::MEM);
            if (posicao_UF == -1) return false;
            contagem_regressiva_alocacao = instrucao_atual.getLatenciaMEM();
            // fase já é MEM — não precisa mudar
            return true;
        }
        return false;
    }

    // Instrução comum: precisa de Qj e Qk resolvidos
    if (Qj.empty() && Qk.empty()) {
        posicao_UF = procuraUFlivre(uf, ciclo, FaseInstrucao::EX);
        if (posicao_UF == -1) return false;
        contagem_regressiva_alocacao = instrucao_atual.getLatenciaEX();
        fase = FaseInstrucao::EX;
        // Marcação atrasada do destino (caso WAR detectado no addIssue)
        if (destino_pendente_no_cdb) {
            Registrador dest = instrucao_atual.getRegDestino();
            if (dest.getTipo() == 'F') cdb.F[dest.getId()].alocarRS(const_cast<std::string&>(id), ciclo);
            else if (dest.getTipo() == 'R') cdb.R[dest.getId()].alocarRS(const_cast<std::string&>(id), ciclo);
            destino_pendente_no_cdb = false;
        }
        return true;
    }
    return false;
}

// ─── atualizaContagem ─────────────────────────────────────────────────────────
// Retorna true quando uma fase termina. Já avança 'fase' para o próximo estado
// para que executaWrTodos possa distinguir as transições sem ambiguidade.
bool RS::atualizaContagem(UnidadesFuncionais& uf, int ciclo) {
    if (!busy || contagem_regressiva_alocacao <= 0) return false;

    contagem_regressiva_alocacao--;
    if (contagem_regressiva_alocacao > 0) return false;

    TipoInstrucao tipo = instrucao_atual.getTipoInstrucao();

    if (tipo == TipoInstrucao::LOAD && fase == FaseInstrucao::EX) {
        liberarUF(uf, ciclo, FaseInstrucao::EX);
        contagem_regressiva_alocacao = -1;
        fase = FaseInstrucao::MEM; // avança para que executaWrTodos detecte EX→MEM
        return true;
    }

    if (tipo == TipoInstrucao::STORE && fase == FaseInstrucao::EX) {
        liberarUF(uf, ciclo, FaseInstrucao::EX);
        contagem_regressiva_alocacao = -1;
        fase = FaseInstrucao::MEM; // avança para que executaWrTodos detecte EX→MEM
        return true;
    }

    if (fase == FaseInstrucao::MEM) {
        liberarUF(uf, ciclo, FaseInstrucao::MEM);
        fase = FaseInstrucao::WB;
        return true;
    }
    liberarUF(uf, ciclo, FaseInstrucao::EX);
    fase = FaseInstrucao::WB;
    return true;
}

// ─── resolverDependencia ──────────────────────────────────────────────────────
// Broadcast do CDB: se esta RS está esperando por rs_id, captura o valor agora.
// Isso evita que uma re-escrita posterior do mesmo registrador (WAW) impeça
// a RS de iniciar — ela guarda o valor no momento em que foi produzido.
void RS::resolverDependencia(const std::string& rs_id, const Registrador& valor) {
    if (Qj == rs_id) { Vj = valor; Qj = ""; }
    if (Qk == rs_id) { Vk = valor; Qk = ""; }
}

// ─── liberar ──────────────────────────────────────────────────────────────────
void RS::liberar(int ciclo) {
    tempos_alocacao.push_back(ciclo);
    busy                         = false;
    contagem_regressiva_alocacao = -1;
    posicao_UF                   = -1;
    Qj = Qk                      = "";
    Vj = Vk                      = Registrador{};
    fase                         = FaseInstrucao::ISSUE;
}

// ─── procuraUFlivre ───────────────────────────────────────────────────────────
// Recebe a fase em que a instrução VAI ENTRAR para escolher a UF correta:
//   LOAD/STORE em EX  → cálculo de endereço → ula_int_basico
//   LOAD/STORE em MEM → acesso à memória    → acessar_memoria
int RS::procuraUFlivre(UnidadesFuncionais& uf, int ciclo, FaseInstrucao fase_destino) const {
    auto buscar = [&](std::vector<UF>& grupo) -> int {
        for (int i = 0; i < (int)grupo.size(); i++) {
            if (!grupo[i].busy) {
                grupo[i].busy     = true;
                grupo[i].RS_atual = id;
                grupo[i].RS_alocadas.push_back(id);
                grupo[i].tempo_alocacao.push_back(ciclo);
                return i;
            }
        }
        return -1;
    };
    TipoInstrucao tipo = instrucao_atual.getTipoInstrucao();
    // LOAD e STORE: EX = cálculo de endereço (ULA inteira), MEM = acesso à memória
    if (tipo == TipoInstrucao::LOAD || tipo == TipoInstrucao::STORE) {
        if (fase_destino == FaseInstrucao::EX)  return buscar(uf.ula_int_basico);
        else                                    return buscar(uf.acessar_memoria);
    }
    switch (tipo) {
        case TipoInstrucao::INT_MUL:
        case TipoInstrucao::INT_DIV:      return buscar(uf.ula_int_mult_div);
        case TipoInstrucao::FLOAT_BASICO: return buscar(uf.ula_float_basico);
        case TipoInstrucao::FLOAT_MUL:
        case TipoInstrucao::FLOAT_DIV:    return buscar(uf.ula_float_mult_div);
        default:                          return buscar(uf.ula_int_basico);
    }
}

// ─── liberarUF ────────────────────────────────────────────────────────────────
// Recebe a fase que ACABOU para saber de qual grupo liberar:
//   LOAD/STORE saindo de EX  → liberou ula_int_basico
//   LOAD/STORE saindo de MEM → liberou acessar_memoria
void RS::liberarUF(UnidadesFuncionais& uf, int ciclo, FaseInstrucao fase_que_terminou) {
    if (posicao_UF == -1) return;
    auto liberar_em = [&](std::vector<UF>& grupo) {
        if (posicao_UF < (int)grupo.size()) {
            grupo[posicao_UF].busy = false;
            grupo[posicao_UF].RS_atual = "";
            grupo[posicao_UF].tempo_alocacao.push_back(ciclo);
        }
    };
    TipoInstrucao tipo = instrucao_atual.getTipoInstrucao();
    if (tipo == TipoInstrucao::LOAD || tipo == TipoInstrucao::STORE) {
        if (fase_que_terminou == FaseInstrucao::EX) liberar_em(uf.ula_int_basico);
        else                                        liberar_em(uf.acessar_memoria);
    } else {
        switch (tipo) {
            case TipoInstrucao::INT_MUL:
            case TipoInstrucao::INT_DIV:      liberar_em(uf.ula_int_mult_div);   break;
            case TipoInstrucao::FLOAT_BASICO: liberar_em(uf.ula_float_basico);   break;
            case TipoInstrucao::FLOAT_MUL:
            case TipoInstrucao::FLOAT_DIV:    liberar_em(uf.ula_float_mult_div); break;
            default:                          liberar_em(uf.ula_int_basico);     break;
        }
    }
    posicao_UF = -1;
}
