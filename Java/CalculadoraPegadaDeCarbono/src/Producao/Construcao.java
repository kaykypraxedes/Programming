package pegadaDeCarbono.Producao;

import pegadaDeCarbono.Excecao.ImpossivelCalcularPegadaException;

public class Construcao extends Producao{
    private double R;
    private double E;
    private double EFe;
    private double EFr;

    public Construcao(double M, double T, double E, double R){
        super(M, T);
        this.E = E;
        this.R = R;
        EFm = 400;
        EFt = 0.25;
        EFe = 0.1;
        EFr = 150;
    }

    @Override
    public double getPegadaDeCarbono() throws ImpossivelCalcularPegadaException {
        double pegada = (M * EFm + T * EFt + E * EFe + R * EFr);
        if(pegada == 0){
            throw new ImpossivelCalcularPegadaException("");
        }
        return pegada;
    }
}
