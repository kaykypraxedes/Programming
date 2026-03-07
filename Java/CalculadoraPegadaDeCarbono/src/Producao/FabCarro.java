package pegadaDeCarbono.Producao;

import pegadaDeCarbono.Excecao.ImpossivelCalcularPegadaException;

public class FabCarro extends Fabricacao{
    public FabCarro(double M, double T, double F) {
        super(M, T, F);
        EFf = 0.1;
        EFt = 0.25;
        EFm = 6;
    }

    @Override
    public double getPegadaDeCarbono() throws ImpossivelCalcularPegadaException {
        return super.getPegadaDeCarbono();
    }
}
