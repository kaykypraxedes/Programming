package pegadaDeCarbono.Producao;

import pegadaDeCarbono.Excecao.ImpossivelCalcularPegadaException;

public abstract class Producao {
    protected double M;
    protected double EFm;
    protected double T;
    protected double EFt;

    public Producao(double M, double T) {
        this.M = M;
        this.EFm = EFm;
        this.T = T;
        this.EFt = EFt;
    }

    public abstract double getPegadaDeCarbono()
            throws ImpossivelCalcularPegadaException;

    // Getters

    public double getM() {
        return M;
    }

    public double getEFm() {
        return EFm;
    }

    public double getT() {
        return T;
    }

    public double getEFt() {
        return EFt;
    }

    // Setters

    public void setM(double m) {
        M = m;
    }

    public void setEFm(double EFm) {
        this.EFm = EFm;
    }

    public void setT(double t) {
        T = t;
    }

    public void setEFt(double EFt) {
        this.EFt = EFt;
    }

    //public abstract boolean verificacaoInterna();
}
