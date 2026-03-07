package Elementos;

public class Autor {
    private final String nome;
    private final String titulacao;

    // construtor
    public Autor(String nome, String titulacao){
        this.nome = nome;
        this.titulacao = titulacao;
    }

    // gets
    public String getNome() {
        return nome;
    }

    @Override
    public String toString() {
        return String.format("%s %s", titulacao, nome);
    }
}
