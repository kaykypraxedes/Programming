/*
- Desenvolvedor:
    Kayky Moreira Praxedes

- Resumo do projeto:
    Biblioteca com um menu interativo para o usuário. 
    
    A biblioteca permite cadastrar novos autores e obras, bem como a 
    pesquisa de obras gerais, específicas e de textos semelhantes caso a 
    a obra buscada não esteja cadastrada.
    
    O projeto segue os princípios da Programação Orientada a Objetos, com 
    separação dos módulos em packages com suas devidas responsabilidades.
*/

package Visao;

import Biblioteca.Biblioteca;
import Elementos.Publicacao;
import Elementos.Artigo;
import Elementos.Autor;
import Elementos.Livro;

import java.util.ArrayList;
import java.util.Scanner;

public class Interface {
    public static void main(String[] args) {
        String e = "Opção inválida!\n";
        Scanner input = new Scanner(System.in);
        Biblioteca biblioteca = new Biblioteca();

        // Elementos criados para o arquivo não começar vazio
        Autor autor1 = new Autor("João", "Mestre");
        Autor autor2 = new Autor("Mario", "Doutor");
        Autor autor3 = new Autor("Luigi", "Mestre");

        Livro livro1 = new Livro(1,1,1967,"Belas Histórias","Leitura", 1, 123456);
        Livro livro2 = new Livro(3,5,1899,"Alice","Ática", 1, 654321);

        livro1.setAutores(autor1);
        livro1.setAutores(autor2);
        livro2.setAutores(autor3);

        biblioteca.cadastrarAutores(autor1);
        biblioteca.cadastrarAutores(autor2);
        biblioteca.cadastrarAutores(autor3);

        biblioteca.cadastrarPublicacao(livro1);
        biblioteca.cadastrarPublicacao(livro2);

        System.out.println("Insira suas ações:");
        System.out.println("1) Cadastrar.");
        System.out.println("2) Listar as publicações.");
        System.out.println("3) Buscar publicações.");
        System.out.println("0) Encerrar operação");
        int opcao = input.nextInt();
        input.nextLine();
        while (opcao != 0){
            switch (opcao){
                case 1:
                    System.out.println("O que você deseja cadastrar:");
                    System.out.println("1) Autor.");
                    System.out.println("2) Publicações.");
                    int opcao2 = input.nextInt();
                    input.nextLine();
                    switch (opcao2){
                        case 1:
                            String nome, titulacao;
                            System.out.print("Insira o nome do autor: ");
                            nome = input.nextLine();
                            System.out.println("Insira a titulação do autor: ");
                            System.out.println("1) Mestre.");
                            System.out.println("2) Doutor.");
                            int opcao3 = input.nextInt();
                            input.nextLine();
                            if(opcao3 < 1 || opcao3 > 2){
                                System.out.println(e);
                            } else{
                                if(opcao3 == 1)
                                    titulacao = "Doutor";
                                else
                                    titulacao = "Mestre";
                                biblioteca.cadastrarAutores(new Autor(nome, titulacao));
                                System.out.println("Autor cadastrado com sucesso!\n");
                            }
                            break;
                        case 2:
                            String titulo, editora;
                            int dia, mes, ano, edicao, ISBN;
                            ArrayList<Autor> autoresSobrando = new ArrayList<>();
                            autoresSobrando.addAll(biblioteca.getAutores());
                            ArrayList<Autor> autoresPublicacao = new ArrayList<>();
                            Publicacao publicacao;
                            if(!autoresSobrando.isEmpty()){
                                System.out.print("Insira o título do livro: ");
                                titulo = input.nextLine();
                                System.out.print("Insira a Editora do livro: ");
                                editora = input.nextLine();
                                System.out.print("Insira o dia de publicação: ");
                                dia = input.nextInt();
                                System.out.print("Insira o mes de publicação: ");
                                mes = input.nextInt();
                                System.out.print("Insira o ano de publicação: ");
                                ano = input.nextInt();
                                System.out.print("Insira a edição: ");
                                edicao = input.nextInt();
                                System.out.print("Insira o ISBN: ");
                                ISBN = input.nextInt();
                                System.out.println("Insira se é um Artigo(1) ou Livro(2)");
                                int tipo = input.nextInt();
                                input.nextLine();
                                if(tipo == 1){
                                    publicacao = new Artigo(dia, mes, ano, titulo, editora,
                                            edicao, ISBN);
                                } else if (tipo == 2){
                                    publicacao = new Livro(dia, mes, ano, titulo, editora,
                                            edicao, ISBN);
                                } else{
                                    System.out.println(e);
                                    break;
                                }
                                System.out.println("1)Inserir um autor:");
                                System.out.println("0)Encerrar");
                                int opcao4 = input.nextInt();
                                input.nextLine();
                                while(opcao4 != 0) {
                                    if (opcao4 != 1)
                                        System.out.println(e);
                                    else{
                                        int posicao = 1;
                                        for(Autor a: autoresSobrando){
                                            System.out.printf("%d) %s%n", posicao, a);
                                            posicao++;
                                        }
                                        System.out.println("Insira qual autor deseja adicionar" +
                                                "(Seu número)");
                                        int opcao5 = input.nextInt();
                                        input.nextLine();
                                        posicao = 1;
                                        boolean adicionou = false;
                                        for(Autor a: autoresSobrando){
                                            if(posicao == opcao5){
                                                publicacao.setAutores(a);
                                                autoresPublicacao.add(a);
                                                autoresSobrando.remove(a);
                                                System.out.println("Autor adicionado com sucesso!");
                                                adicionou = true;
                                                break;
                                            }
                                            posicao++;
                                        }
                                        if(!adicionou){
                                            System.out.println(e);
                                        }
                                        if(autoresSobrando.isEmpty()) {
                                            System.out.println("Não há mais autores cadastrados para adicionar!");
                                            break;
                                        }
                                        System.out.println("1)Inserir um autor:");
                                        System.out.println("0)Encerrar");
                                        opcao4 = input.nextInt();
                                        input.nextLine();
                                    }
                                }
                                if(!autoresPublicacao.isEmpty()){
                                    biblioteca.cadastrarPublicacao(publicacao);
                                    System.out.println("Publicação cadastrada com sucesso!\n");
                                } else {
                                    System.out.println("ERRO! Nenhum autor foi cadastrado!\n");
                                }
                            } else {
                                System.out.println("Nenhum autor foi cadastrado ainda!\n");
                            }
                            break;
                        default:
                            System.out.println(e);
                    }
                    break;
                case 2:
                    if(!biblioteca.listarPublicacoes()){
                        System.out.println("Nenhuma publicação ainda foi cadastrada!\n");
                    }
                    break;
                case 3:
                    System.out.println("Buscar por autor(1) ou por título(2)");
                    int opcao3 = input.nextInt();
                    input.nextLine();
                    String nome;
                    switch (opcao3){
                        case 1:
                            System.out.println("Insira o nome do autor: ");
                            nome = input.nextLine();
                            if(!biblioteca.buscarPorAutor(nome.toLowerCase()))
                                System.out.println("Nenhum livro desse autor achado!\n");
                            break;
                        case 2:
                            System.out.println("Insira o nome do livro: ");
                            nome = input.nextLine();
                            if(!biblioteca.buscarPorTitulo(nome.toLowerCase()))
                                System.out.println("Nenhum livro com esse título achado!");
                            break;
                        default:
                            System.out.println(e);
                    }
                    break;
                default:
                    System.out.println(e);
            }
            System.out.println("Insira suas ações:");
            System.out.println("1) Cadastrar.");
            System.out.println("2) Listar as publicações.");
            System.out.println("3) Buscar publicações.");
            System.out.println("0) Encerrar operação");
            opcao = input.nextInt();
            input.nextLine();
        }
        System.out.println("Encerrando...");
    }
}
