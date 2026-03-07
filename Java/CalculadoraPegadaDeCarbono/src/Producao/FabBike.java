package pegadaDeCarbono.Producao;

import pegadaDeCarbono.Excecao.ImpossivelCalcularPegadaException;

public class FabBike extends Fabricacao{
    public FabBike(double M, double T, double F) {
        super(M, T, F);
        EFf = 0.1;
        EFt = 0.25;
        EFm = 10;
    }
    @Override
    public double getPegadaDeCarbono() throws ImpossivelCalcularPegadaException {
        return super.getPegadaDeCarbono();
    }
}
