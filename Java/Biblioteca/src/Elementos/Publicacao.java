package Elementos;

import java.util.ArrayList;

public abstract class Publicacao {
    protected int[] data;
    protected String titulo;
    protected ArrayList<Autor> autores;
    protected String editora;
    protected int edicao;
    protected int ISBN;


    // construtor
    public Publicacao(int dia, int mes, int ano, String titulo, String editora,
                      int edicao, int ISBN){
        data = new int[3];
        data[0] = dia;
        data[1] = mes;
        data[2] = ano;
        this.titulo = titulo;
        this.editora = editora;
        this.edicao = edicao;
        this.ISBN = ISBN;
        autores = new ArrayList<>();
    }
    // set
    public boolean setAutores(Autor autor) {
        if(autores.size() < 20) {
            autores.add(autor);
            return true;
        }
        return false;
    }

    // gets
    public int[] getData() {
        return data;
    }
    public String getTitulo() {
        return titulo;
    }
    public ArrayList<Autor> getAutores() {
        return autores;
    }
    public String getEditora() {
        return editora;
    }
    public int getEdicao() {
        return edicao;
    }
    public int getISBN() {
        return ISBN;
    }
    // toString

    @Override
    public String toString() {
        int contador = 1;
        String quaisAutores = "";
        for(Autor a: autores){
            quaisAutores += a;
            if(contador != autores.size())
                quaisAutores += ", ";
            contador++;
        }
        return String.format("Título: %s, Data de publicação: %d/%d/%d%n" +
                "Autor(es): %s%n" +
                "%d° Edição, Editora: %s, ISBN: %d%n", titulo, data[0], data[1],
                data[2], quaisAutores, edicao, editora, ISBN);
    }
}
