package Negocios;

import java.nio.file.Files;
import java.nio.file.Paths;
import Servicos.ManipulaArquivoTexto;
import java.util.HashMap;

public class Agenda {

    private HashMap<String, Contato> contatos;

    // Construtor
    public Agenda(){
        contatos = new HashMap<>();
        // Verifica se o arquivo existe
        if (Files.exists(Paths.get(ManipulaArquivoTexto.caminho))) {
            // Se o arquivo existe, carrega os dados
            atualizarLer();
        } else {
            // Se não existe, cria um arquivo inicial vazio
            ManipulaArquivoTexto.abrirArquivoGravar();
            ManipulaArquivoTexto.fecharArquivosGravar();
        }
    }

    public void incluirContato(Contato contato){
        contatos.put(contato.getNome(), contato);
        atualizarGravar();
    }

    public boolean existeContato(String nome) throws ContatoInexistenteException{
        if(contatos.get(nome) == null)
            throw new ContatoInexistenteException("Contato Inexistente!");
        return true;
    }

    public Contato consultarContato(String nome) throws ContatoInexistenteException{
        if(!existeContato(nome))
            throw new ContatoInexistenteException("Contato Inexistente!");
        return contatos.get(nome);
    }

    public void removerContato(String nome) throws ContatoInexistenteException{
        if(!existeContato(nome))
            throw new ContatoInexistenteException("Contato Inexistente!");
        contatos.remove(nome);
        atualizarGravar();
    }

    public void lerAgenda(){
        if(!contatos.isEmpty()){
            System.out.printf("%-15s | %-15s | %-25s | %s%n", "Nome", "Telefone",
                    "Endereço", "E-Mail");
            System.out.println("------------------------------------" +
                    "-----------------------------------------------" +
                    "---");
            for(String s: contatos.keySet()){
                System.out.printf("%-15s | %-15s | %-25s | %s%n", contatos.get(s).getNome(),
                        contatos.get(s).getTelefone(), contatos.get(s).getEndereco(),
                        contatos.get(s).getEmail());
            }
        } else{
            System.out.println("Agenda vazia!");
        }
    }

    // Auxiliar
    private void atualizarLer(){
        ManipulaArquivoTexto.abrirArquivoLer();
        contatos = ManipulaArquivoTexto.lerArquivo();
        ManipulaArquivoTexto.fecharArquivoLer();
    }
    private void atualizarGravar(){
        ManipulaArquivoTexto.abrirArquivoGravar();
        ManipulaArquivoTexto.gravarContato(contatos);
        ManipulaArquivoTexto.fecharArquivosGravar();
    }
}