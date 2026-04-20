/* Excecao/ImpossivelCalcularPegadaException.java */
package pegadaDeCarbono.Excecao;

public class ImpossivelCalcularPegadaException extends Exception {
    public ImpossivelCalcularPegadaException(String message) {
        super(message);
    }
    public ImpossivelCalcularPegadaException() {
      super("Os fatores não podem ser todos iguais a 0");
    }
}
