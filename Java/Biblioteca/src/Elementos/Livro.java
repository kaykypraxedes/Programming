package Elementos;

import java.util.ArrayList;

public class Livro extends Publicacao {
    // construtor
    public Livro(int dia, int mes, int ano, String titulo, String editora,
                 int edicao, int ISBN){
        super(dia, mes, ano, titulo, editora, edicao, ISBN);
    }

    // set
    @Override
    public boolean setAutores(Autor autor) {
        return super.setAutores(autor);
    }

    // gets
    @Override
    public int[] getData() {
        return super.getData();
    }
    @Override
    public String getTitulo() {
        return super.getTitulo();
    }
    @Override
    public ArrayList<Autor> getAutores() {
        return super.getAutores();
    }
    @Override
    public String getEditora() {
        return super.getEditora();
    }
    @Override
    public int getEdicao() {
        return super.getEdicao();
    }
    @Override
    public int getISBN() {
        return super.getISBN();
    }
    @Override
    public String toString() {
        return super.toString();
    }
}
