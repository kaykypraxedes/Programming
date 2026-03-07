/*
- Desenvolvedor:
    Kayky Moreira Praxedes

- Resumo do projeto:
    Agenda digital que mantém os dados armazenados mesmo após o fechamento 
    do programa com um menu interativo para o usuário. 
    
    Dentre as funcionalidades estão  adicionar e excluir um contato, ver 
    todos os contatos ou informações de um em específico. 
    
    O projeto segue os princípios da Programação Orientada a Objetos, com 
    separação dos módulos em packages com suas devidas responsabilidades, 
    utilizando ferramentas de manipulação de arquivos de texto e exceções 
    para garantir o bom funcionamento do código.
*/

package Visao;

import Negocios.Agenda;
import Negocios.Contato;
import Negocios.ContatoInexistenteException;

import java.util.Scanner;

public class AppAgenda {
    public static void main(String[] args) {
        Scanner input = new Scanner(System.in);
        Agenda agenda = new Agenda();

        System.out.println("Insira sua ação:");
        System.out.println("1 - Adicionar um contato;");
        System.out.println("2 - Ver detalhes de um contato;");
        System.out.println("3 - Excluir um contato;");
        System.out.println("4 - Visualizar a agenda;");
        System.out.println("0 - Encerrar;");
        System.out.print("? ");
        int escolha = input.nextInt();
        input.nextLine();
        while(escolha != 0){
            switch (escolha){
                case 1:
                    System.out.println("Insira o nome:");
                    String nome1 = input.nextLine();
                    System.out.println("Insira o numero:");
                    String numero1 = input.nextLine();
                    System.out.println("Insira o endereço:");
                    String endereco1 = input.nextLine();
                    System.out.println("Insira o email:");
                    String email1 = input.nextLine();
                    agenda.incluirContato(new Contato(nome1,
                            numero1, endereco1, email1));
                    System.out.println("Contato adicionado com sucesso!");
                    break;
                case 2:
                    try {
                        System.out.println("Insira o nome:");
                        String nome2 = input.nextLine();
                        if (agenda.existeContato(nome2)) {
                            System.out.printf("%-15s | %-15s | %-25s | %s%n", "Nome",
                                    "Telefone", "Endereço", "E-Mail");
                            System.out.println("------------------------------------" +
                                    "-----------------------------------------------" +
                                    "---");
                            Contato contato = agenda.consultarContato(nome2);
                            System.out.printf("%-15s | %-15s | %-25s | %s%n",
                                    contato.getNome(), contato.getTelefone(),
                                    contato.getEndereco(), contato.getEmail());
                        }
                    } catch (ContatoInexistenteException e){
                        System.err.println(e);
                    }
                    break;
                case 3:
                    try {
                        System.out.println("Insira o nome:");
                        agenda.removerContato(input.nextLine());
                        System.out.println("Contato excluido!");
                    } catch (ContatoInexistenteException e){
                        System.err.println(e);
                    }
                    break;
                case 4:
                    agenda.lerAgenda();
                    break;
                default:
                    System.out.println("Opção inválida!");
            }
            System.out.println();
            System.out.println("Insira sua ação:");
            System.out.println("1 - Adicionar um contato;");
            System.out.println("2 - Ver detalhes de um contato;");
            System.out.println("3 - Excluir um contato;");
            System.out.println("4 - Visualizar a agenda;");
            System.out.println("0 - Encerrar;");
            System.out.print("? ");
            escolha = input.nextInt();
            //  limpa buffer depois
            input.nextLine();
        }
        System.out.println("Encerrando...");
        input.close();
    }
}
