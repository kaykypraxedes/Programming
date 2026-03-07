/*
- Desenvolvedores:
    Kayky Moreira Praxedes
    Carlos Ernesto Cardoso dos Reis

- Resumo do projeto:
    Calculadora de pegada de carbono de diversos elementos. 
    
    O objetivo principal do projeto era, além de fazer o cálculo da
    pegada de carbono de diversos elementos dadas certas variáveis,
    mostrar domínio de herança e recursos de exceção, mostrando o 
    funcionamento do código apesar de uma ação inválida. 
    
    O projeto segue os princípios da Programação Orientada a Objetos, com 
    separação dos módulos em packages com suas devidas responsabilidades.
*/


package pegadaDeCarbono.Visao;

import pegadaDeCarbono.Excecao.ImpossivelCalcularPegadaException;
import pegadaDeCarbono.Producao.Construcao;
import pegadaDeCarbono.Producao.FabBike;
import pegadaDeCarbono.Producao.FabCarro;
import pegadaDeCarbono.Producao.Producao;

public class Calculadora {
    public static void main(String[] args) {
        Producao[] producoes = new Producao[5];
        producoes[0]  = new Construcao(10, 150, 100, 50);
        producoes[1] = new FabBike(10, 150, 100);
        producoes[2] = new FabCarro(20, 200, 80);
        producoes[3] = new FabCarro(0,0,0);
        producoes[4] = new FabCarro(20, 250, 80);
        try {
            System.out.println("Construção: " + producoes[0].getPegadaDeCarbono());
            System.out.println("Fabricação Bicicleta: " + producoes[1].getPegadaDeCarbono());
            System.out.println("Fabricação Carro: " + producoes[2].getPegadaDeCarbono());
            System.out.println("Teste do 0: " + producoes[3].getPegadaDeCarbono());
            System.out.println("Teste depois do 0: " + producoes[4].getPegadaDeCarbono());
        } catch (ImpossivelCalcularPegadaException e){
            System.err.println(e.getMessage());
        }
        try {
            System.out.println("Mostra que o código não para por causa da Exception: " + producoes[4].getPegadaDeCarbono());
        } catch (ImpossivelCalcularPegadaException e){
            System.err.println(e.getMessage());
        }
    }
}
