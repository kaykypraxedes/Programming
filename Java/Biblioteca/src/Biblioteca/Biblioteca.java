package Biblioteca;

import Elementos.Artigo;
import Elementos.Autor;
import Elementos.Livro;
import Elementos.Publicacao;

import java.util.ArrayList;

public class Biblioteca {
    private ArrayList<Publicacao> publicacoes;
    private ArrayList<Autor> autores;

    // construtor
    public Biblioteca(){
        publicacoes = new ArrayList<>();
        autores = new ArrayList<>();
    }

    public void cadastrarAutores(Autor autor){
        autores.add(autor);
    }

    public boolean cadastrarPublicacao(Publicacao publicacao){
        if(publicacoes.size() < 65535){
            publicacoes.add(publicacao);
            return true;
        }
        return false;
    }

    public boolean listarPublicacoes(){
        int valor = 1;
        if(!publicacoes.isEmpty()){
            for(Publicacao p: publicacoes){
                System.out.printf("%d) ", valor);
                if(p instanceof Artigo){
                    System.out.printf("Artigo: %s%n", p);
                } else if(p instanceof Livro){
                    System.out.printf("Livro: %s%n", p);
                }
                valor++;
            }
            return true;
        }
        return false;
    }

    public boolean buscarPorTitulo(String nome){
        int valor = 1;
        nome = nome.toLowerCase();
        boolean achou = false;
        String[] partesNome = nome.split(" ");
        // Exatos
        for(Publicacao p: publicacoes){
            if(nome.equals(p.getTitulo().toLowerCase())){
                if(valor == 1)
                    System.out.println("Resultados:");
                System.out.printf("%d) ", valor);
                if (p instanceof Artigo) {
                    System.out.printf("Artigo: %s%n", p);
                } else if (p instanceof Livro) {
                    System.out.printf("Livro: %s%n", p);
                }
                valor++;
                achou = true;
            }
        }
        if(!achou){
            // Semelhantes
            for(Publicacao p: publicacoes){
                String[] partesTitulo = p.getTitulo().toLowerCase().split(" ");
                for(String primeira: partesTitulo){
                    for(String segunda: partesNome){
                        if(primeira.equals(segunda)) {
                            if(valor == 1)
                                System.out.println("Não foram achados resultados exatos:");
                            System.out.println("Semelhantes:");
                            System.out.printf("%d) ", valor);
                            if (p instanceof Artigo) {
                                System.out.printf("Artigo: %s%n", p);
                            } else if (p instanceof Livro) {
                                System.out.printf("Livro: %s%n", p);
                            }
                            valor++;
                            achou = true;
                        }
                    }
                }
            }
        }
        return achou;
    }

    public boolean buscarPorAutor(String nome){
        int valor = 1;
        nome = nome.toLowerCase();
        boolean achou = false;
        String[] partesNome = nome.split(" ");
        // Exatos
        for(Publicacao p: publicacoes){
            ArrayList<Autor> osAutores = p.getAutores();
            for(Autor a: osAutores){
                if(nome.equals(a.getNome().toLowerCase())) {
                    if(valor == 1)
                        System.out.println("Resultados:");
                    System.out.printf("%d) ", valor);
                    if (p instanceof Artigo) {
                        System.out.printf("Artigo: %s%n", p);
                    } else if (p instanceof Livro) {
                        System.out.printf("Livro: %s%n", p);
                    }
                    valor++;
                    achou = true;
                }
            }
        }
        if(!achou){
            // Semelhantes
            for(Publicacao p: publicacoes){
                ArrayList<Autor> osAutores = p.getAutores();
                ArrayList<String[]> nomesAutores = new ArrayList<>();
                for(Autor a: osAutores){
                    nomesAutores.add(a.getNome().toLowerCase().split(" "));
                }
                for(String[] primeira: nomesAutores){
                    for(String segunda: primeira){
                        for(String terceira: partesNome){
                            if(terceira.equals(segunda)) {
                                if(valor == 1)
                                    System.out.println("Não foram achados resultados exatos:");
                                System.out.println("Semelhantes:");
                                System.out.printf("%d) ", valor);
                                if (p instanceof Artigo) {
                                    System.out.printf("Artigo: %s%n", p);
                                } else if (p instanceof Livro) {
                                    System.out.printf("Livro: %s%n", p);
                                }
                                valor++;
                                achou = true;
                            }
                        }
                    }
                }
            }
        }
        return achou;
    }

    public ArrayList<Autor> getAutores() {
        return autores;
    }
}
