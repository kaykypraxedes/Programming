package Negocios;

public class ContatoInexistenteException extends Exception{

    public ContatoInexistenteException(){
        super();
    }
    public ContatoInexistenteException(String string){
        super(string);
    }
}
