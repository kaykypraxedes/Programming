package pegadaDeCarbono.Producao;

import pegadaDeCarbono.Excecao.ImpossivelCalcularPegadaException;

public abstract class Fabricacao extends Producao{
    protected double F;
    protected double EFf;
    public Fabricacao(double M, double T, double F){
        super(M, T);
        this.F = F;
    }

    @Override
    public double getPegadaDeCarbono() throws ImpossivelCalcularPegadaException {
        double pegada = (M * EFm + T * EFt + F * EFf);
        if(pegada == 0) {
            throw new ImpossivelCalcularPegadaException();
        }
        return pegada;
    }


}