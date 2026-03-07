package Servicos;

import Negocios.Contato;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.file.Paths;
import java.util.Formatter;
import java.util.HashMap;
import java.util.Scanner;

public class ManipulaArquivoTexto {

    private static Formatter gravador;
    private static Scanner leitor;
    public static final String caminho = "src/Agenda.txt";

    // Gravação do arquivo
    public static void abrirArquivoGravar(){
        try {
            gravador = new Formatter(caminho);
        } catch (FileNotFoundException e){
            System.err.println("Erro ao abrir o arquivo! Encerrando");
            System.exit(1);
        }
    }

    public static void gravarContato(HashMap<String, Contato> contatos){
        for(String nome: contatos.keySet()){
            Contato contato = contatos.get(nome);
            gravador.format("%s;%s;%s;%s%n", contato.getNome(),
                    contato.getTelefone(), contato.getEndereco(), contato.getEmail());
        }
    }

    public static void fecharArquivosGravar(){
        if(gravador != null)
            gravador.close();
    }

    // Leitura do arquivo
    public static void abrirArquivoLer(){
        try{
            // Paths.get("Agenda.txt") = fluxo de dados
            leitor = new Scanner(Paths.get(caminho));
        } catch (IOException e){
            System.err.println("Erro ao acessar o arquivo! Encerrando!");
            System.exit(1);
        }
    }

    public static HashMap<String, Contato> lerArquivo(){
        HashMap<String, Contato> contatos = new HashMap<>();
        leitor.useDelimiter(";|\\n");
        while(leitor.hasNext()){
            Contato provisorio = new Contato(leitor.next().trim(), leitor.next().trim(),
                    leitor.next().trim(), leitor.next().trim());
            contatos.put(provisorio.getNome(), provisorio);
        }
        return contatos;
    }

    public static void fecharArquivoLer(){
        if(leitor != null)
            leitor.close();
    }
}

