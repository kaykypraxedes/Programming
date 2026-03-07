# - Desenvolvedor:
#     Kayky Moreira Praxedes
#
# - Resumo do projeto:
#     Construtor de uma B-Tree de grau t, com funções para inserir, remover e 
#     buscar chaves.
#     
#     O projeto, além de se utilizar dos princípios da Programação Orientada a 
#     Objetos, implementa um algoritmo de ordenação e pesquisa de alta 
#     complexidade, mas de grande utilidade.
#
#     Nenhum elemento da árvore fica com mais do que 2t - 1, ou com menos
#     do que t - 1 chaves em nenhum momento, de modo que a árvore é sempre 
#     reorganizada antes, em caso de uma inserção ou de uma remoção que geraria
#     um estado ilegal.
#
#     A inserção é sempre realizada em uma folha, de modo a abrager 2 casos 
#     principais: 
#         1) O nó possui é menos que 2t - 1 chaves: a chave é inserida sem 
#            alterações na estrutura da árvore.
#         2) O nó possui 2t - 1 chaves (limite): Antes da inserção, são 
#            realizadas divisões dos nós de maneira recursiva até que 
#            inserção se torne legal
#     
#     Na remoção, os casos são:
#         1) Remoção na folha:
#             a) Nó com mais de t - 1 chaves: a chave é retirada
#             b) Nó tem t - 1 chaves, mas algum irmão tem mais: a chave pai 
#                passa para o nó filho e o sucessor ou o antecessor dele no 
#                nó irmão vai para seu lugar, depois a chave é removida.
#             c) Nó e os irmãos tem t - 1 chaves: Antes da remoção, são 
#                realizadas funsões recursivas do com a chave nó com o nó pai e
#                com um irmão.
#         2) Remoção em um nó interno:
#             a) Um dos filhos tem mais de t - 1 chaves: O antecessor ou sucessor
#                da chave vai para o seu lugar, e depois ela é removida.
#             b) Ambos os filhos t - 1 chaves: São realizadas fusões recursivas 
#                chave se funde com o da chave com filho direito e esquerdo, e, 
#                posteriormente, a chave é eliminada.

class bNode:
    def __init__ (self, leaf):
        self.leaf = leaf
        self.keys = []
        self.children = []

class bTree:
    def __init__ (self, t):
        self.root = None
        self.t = t
    
    def goLeaf(self, node, k):
        if node.leaf is True:
            return node
        else:
            i = 0
            for i in range(len(node.keys)):
                if k < node.keys[i]:
                    return self.goLeaf(node.children[i], k)
            return self.goLeaf(node.children[i + 1], k)
    
    def findParent(self, current, child):
        if current.leaf is True:
            return None
        for i in range(len(current.keys) + 1):
            if current.children[i] == child:
                return current
            else:
                parent = self.findParent(current.children[i], child)
                if parent is not None:
                    return parent
        return None

    def split(self, node):
        # Encontra o pai do nó a ser dividido
        parent = self.findParent(self.root, node)
        # Se o nó a ser dividido for a raiz
        if parent is None:
            newRoot = bNode(False)
            newRoot.children.append(node)
            self.root = newRoot
            parent = newRoot
        # Cria um novo nó
        newNode = bNode(node.leaf)
        # Verifica se o pai tem espaço para a nova chave
        if len(parent.keys) == 2 * self.t - 1:
            self.split(parent)
            parent = self.findParent(self.root, node)
        # Encontra a posição do nó no pai
        i = 0
        while i < len(parent.keys) and parent.children[i] != node:
            i += 1
        # Calcula a mediana
        mediana_index = self.t - 1
        mediana = node.keys[mediana_index]
        # Move as chaves maiores que a mediana para o novo nó
        newNode.keys = node.keys[mediana_index + 1:]
        # Se não for folha, move os filhos também
        if not node.leaf:
            newNode.children = node.children[mediana_index + 1:]
        # Atualiza o nó original
        node.keys = node.keys[:mediana_index]
        # Se não for folha, atualiza os filhos do nó original
        if not node.leaf:
            node.children = node.children[:mediana_index + 1]
        # Insere a mediana no pai
        parent.keys.insert(i, mediana)
        parent.children.insert(i + 1, newNode)

    def insert(self, k):
        if self.root is None:
            self.root = bNode(True)
            self.root.keys.append(k)
        else:
            leaf = self.goLeaf(self.root, k)
            if len(leaf.keys) < 2 * self.t - 1:
                # Insere normalmente na folha
                i = 0
                while i < len(leaf.keys) and k > leaf.keys[i]:
                    i += 1
                leaf.keys.insert(i, k)
            else:
                # Separa o nó para que possa ser inserido
                self.split(leaf)
                self.insert(k)

    def search(self, k, node):
        # Se o nó estiver vazio
        if node is None:
            return None
        i = 0
        while i < len(node.keys) and k > node.keys[i]:
            i += 1
        # k <= node.keys[i] (menor ou igual)
        if i < len(node.keys):
            if k == node.keys[i]:
                return node
            elif node.leaf:
                return None
            else:
                return self.search(k, node.children[i])
        # i == len(node.keys) (maior)
        else:
            if node.leaf:
                return None
            else:
                return self.search(k, node.children[i])
    
    def passKey(self, node, sibling, parent, index, pm):
        # Move a chave do pai para o nó 
        if pm == -1: node.keys.insert(0, parent.keys[index])
        else: node.keys.append(parent.keys[index])
        # Pega a chave do irmão
        parent.keys[index] = sibling.keys.pop(-1 if pm == -1 else 0)
        if not sibling.leaf:
            if pm == -1: node.children.insert(0, sibling.children.pop(-1))
            else: node.children.append(sibling.children.pop(0))

    # Junta dois nós
    def merge(self, node, sibling, parent, index, pm):
        grandParent = self.findParent(self.root, parent)
        # Move a chave do pai para o nó
        # Caso 1 (pai tem mais de t - 1 chaves ou é raiz)
        if len(parent.keys) > self.t - 1 or grandParent is None:
            # Do irmão esquerdo
            if pm == -1: 
                sibling.keys.append(parent.keys[index])
                parent.keys.pop(index)
                parent.children.pop(index)
                sibling.keys.extend(node.keys)
                if not node.leaf: sibling.children.extend(node.children)
            # Do irmão direito
            else: 
                node.keys.append(parent.keys[index])
                parent.keys.pop(index)
                parent.children.pop(index + 1)
                node.keys.extend(sibling.keys)
                if not sibling.leaf: node.children.extend(sibling.children)
                # Caso de descer a última chave da raiz e ela ficar vazia
                if parent == self.root and len(parent.keys) == 0:
                    if pm == -1:
                        self.root = sibling
                    else:
                        self.root = node
        # Caso onde pai não tem chaves extras e precisamos reorganizar com avô
        else:
            indexParent = grandParent.children.index(parent)
            # Irmão esquerdo do pai tem chaves extras
            if indexParent > 0 and len(grandParent.children[indexParent - 1].keys) > self.t - 1:
                self.passKey(parent, grandParent.children[indexParent - 1], grandParent, indexParent - 1, -1)
                self.merge(node, sibling, parent, index, pm)
            # Irmão direito do pai tem chaves extras  
            elif indexParent < len(grandParent.keys) and len(grandParent.children[indexParent + 1].keys) > self.t - 1:
                self.passKey(parent, grandParent.children[indexParent + 1], grandParent, indexParent, 0)
                self.merge(node, sibling, parent, index, pm)
            # Nem os irmãos do pai têm chaves extras - precisa fundir o pai com um irmão
            else:
                # Funde pai com irmão dele
                if indexParent > 0: self.merge(parent, grandParent.children[indexParent - 1], grandParent, indexParent - 1, -1)
                else: self.merge(parent, grandParent.children[indexParent + 1], grandParent, indexParent, 0)

    # Função de remoção
    def remove(self, k):
        node = self.search(k, self.root)
        if node is None:
            print("Chave não encontrada")
            return
        # Caso especial: A árvore tem apenas a raiz
        elif node == self.root and node.leaf is True:            
            node.keys.remove(k)
        else:
        # Caso 1: O nó é uma folha
            if node.leaf is True:
                # Caso 1a: O nó tem mais que o número mínimo de chaves
                if len(node.keys) > self.t - 1:
                    node.keys.remove(k)
                # Caso 1b: O nó tem o número mínimo de chaves, mas um irmão tem mais que o mínimo
                else:
                    parent = self.findParent(self.root, node)
                    index = parent.children.index(node)
                    # Verifica o irmão à esquerda
                    if index > 0 and len(parent.children[index - 1].keys) > self.t - 1:
                        self.passKey(node, parent.children[index - 1], parent, index - 1, -1)
                        node.keys.remove(k)
                        return
                    # Verifica o irmão à direita
                    elif index < len(parent.keys) and len(parent.children[index + 1].keys) > self.t - 1:
                        self.passKey(node, parent.children[index + 1], parent, index, 0)
                        node.keys.remove(k)
                        return
                    # Caso 1c: O nó e seus irmãos têm o número mínimo de chaves
                    # Reorganiza os nós pra apenas depois eliminar a chave
                    if index > 0:
                        self.merge(node, parent.children[index - 1], parent, index - 1, -1)
                        self.remove(k)
                    else:
                        self.merge(node, parent.children[index + 1], parent, index, 0)
                        self.remove(k)
            # Caso 2: O nó é interno
            else:
                # Caso 2a: filhos tem mais que o número mínimo de chaves
                index = node.keys.index(k)
                # Tenta filho à esquerda
                if len(node.children[index].keys) > self.t - 1:
                    predessor = node.children[index].keys[len(node.children[index].keys) - 1]
                    node.keys[index] = predessor
                    node.children[index].keys.pop(-1)
                    return
                # Tenta filho à direira
                elif len(node.children[index + 1].keys) > self.t - 1:
                    sucessor = node.children[index + 1].keys[0]
                    node.keys[index] = sucessor
                    node.children[index + 1].keys.pop(0)
                    return
                # Caso 2b: filhos tem o número mínimo de chaves
                # Reorganiza os nós pra apenas depois eliminar a chave
                self.merge(node.children[index], node.children[index + 1], node, index, 0)
                self.remove(k)

    def printTree(self, node=None, level=0):
        if node is None:
            node = self.root
        print("  " * level + f"Level {level}: {node.keys} {'(raiz)' if level == 0 else ''} {'(folha)' if node.leaf else ''}")
        if not node.leaf:
            for child in node.children:
                self.printTree(child, level + 1)

# Teste 
def main():
    btree = bTree(2)
    
    print("~~~~Inserção~~~~")
    # Caso 1: Adição em folha com chaves suficientes
    print("\n10, 20 e 5 (caso 1):")
    btree.insert(10)
    btree.insert(20)
    btree.insert(5)
    btree.printTree()
    # Caso 2: Remoção em folha cheia
    print("\n15 (caso 2):")
    btree.insert(15)
    btree.printTree()
    print("\n25, 3 e 7 (caso 1):")
    btree.insert(25)
    btree.insert(3)
    btree.insert(7)
    btree.printTree()
    print("\n12 (caso 2):")
    btree.insert(12)
    btree.printTree()
    print("\n17, 22 e 27 (caso 1):")
    btree.insert(17)
    btree.insert(22)
    btree.insert(27)
    btree.printTree()
    print("\n2 (caso 2):")
    btree.insert(2)
    btree.printTree()
    print("\n4, 6 e 8 (caso 1):")
    btree.insert(4)
    btree.insert(6)
    btree.insert(8)
    btree.printTree()
    print("\n11 (caso recursivo):")
    btree.insert(11)
    btree.printTree()
    print("\n13, 16, 18 e 21 (caso 1):")
    btree.insert(13)
    btree.insert(16)
    btree.insert(18)
    btree.insert(21)
    btree.printTree()
    print("\n23 (caso 2):")
    btree.insert(23)
    btree.printTree()
    print("\n26 e 28 (caso 1):")
    btree.insert(26)
    btree.insert(28)
    btree.printTree()
    
    print("\n~~~~Remoção~~~~")
    # Caso 1a: Remoção em folha com chaves suficientes
    print("\n2 e 4 (casos 1a):")
    btree.remove(2)
    btree.remove(4)
    btree.printTree()
    # Caso 1b: Remoção em folha com empréstimo de irmão
    print("\n3 (caso 1b):")
    btree.remove(3)
    btree.printTree()
    print("\n8, 11, 12, 16 e 17 (casos 1a):")
    btree.remove(8)
    btree.remove(11)
    btree.remove(12)
    btree.remove(16)
    btree.remove(17)
    btree.printTree()
    # Caso 1c: Remoção em folha com fusão do irmão
    print("\n13 (caso 1c):")
    btree.remove(13)
    btree.printTree()
    # Caso 2a: Remoção em nó interno com empréstimo de filho
    print("\n20 (caso 2a):")
    btree.remove(20)
    btree.printTree()
    print("\n22 e 23(casos 1a):")
    btree.remove(22)
    btree.remove(23)
    btree.printTree()
    # Caso 2b: Remoção em nó interno com fusão da chave com os filhos
    print("\n18 (caso 2b):")
    btree.remove(18)
    btree.printTree()
    print("\n6 (Caso recursivo):")
    btree.remove(6)
    btree.printTree()
    print("\n~~~~Pesquisa~~~~")
    # Valor existente na árvore
    if btree.search(28, btree.root) is not None: print("\nNúmero 28 encontrado!")
    else: print("\nNúmero 28 não foi encontrado!")
    # Valor inexistente
    if btree.search(51, btree.root) is not None: print("\nNúmero 51 encontrado!")
    else: print("\nNúmero 51 não foi encontrado!")

if __name__ == "__main__":
    main()