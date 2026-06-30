# B-Tree — Gerador de Árvore Binária

Implementação de uma **Árvore B** (B-Tree) de grau `t` em Python, com operações completas de inserção, remoção e busca. Apesar do nome do diretório, a estrutura é uma **B-Tree**, e não uma árvore binária simples.

**Desenvolvedor:** Kayky Moreira Praxedes

---

## O que é uma B-Tree?

Uma B-Tree é uma estrutura de dados balanceada onde cada nó pode armazenar **múltiplas chaves** e ter **múltiplos filhos**. Diferente de uma árvore binária (que tem no máximo 2 filhos por nó), a B-Tree é otimizada para sistemas que trabalham com grandes volumes de dados, como bancos de dados e sistemas de arquivos.

### Propriedades fundamentais

- Cada nó contém entre `t - 1` e `2t - 1` chaves (exceto a raiz, que pode ter de 1 a `2t - 1`)
- Cada nó não-folha com `n` chaves tem exatamente `n + 1` filhos
- Todas as folhas estão na **mesma profundidade**
- As chaves dentro de cada nó são mantidas **ordenadas**

---

## Estrutura do código

### Constantes globais

| Constante | Valor | Significado |
|---|---|---|
| `IRMAO_ESQUERDO` | `-1` | Indica operação com o irmão esquerdo |
| `IRMAO_DIREITO` | `0` | Indica operação com o irmão direito |
| `MAX_DEPTH` | `100` | Limite de segurança contra recursão infinita |

### Classe `bNode` (nó)

Cada nó armazena:

- `leaf` (`bool`) — se o nó é folha
- `keys` (`list`) — chaves armazenadas no nó
- `children` (`list`) — referências para os nós filhos

### Classe `bTree` (árvore)

A árvore em si:

- `root` — referência para o nó raiz
- `t` — grau mínimo da árvore

---

## Funções

### Inserção

#### `insert(k)`
Insere a chave `k` na árvore:
1. Se a árvore está vazia, cria a raiz como folha e insere `k`
2. Senão, navega até a folha apropriada (`goLeaf`)
3. **Caso 1 — folha com espaço** (`len(keys) < 2t - 1`): insere `k` mantendo a ordem
4. **Caso 2 — folha cheia** (`len(keys) == 2t - 1`): divide o nó (`split`) e tenta a inserção novamente

#### `goLeaf(node, k)`
Navega recursivamente da `node` até a folha onde `k` deve ser inserida.

#### `split(node)`
Divide um nó cheio em dois nós irmãos, promovendo a chave mediana para o nó pai:
1. Encontra o pai do nó
2. Se o nó for a raiz, cria uma nova raiz
3. Move as chaves maiores que a mediana para um novo nó
4. Insere a mediana no pai
5. Se o pai também estiver cheio, divide-o recursivamente

---

### Remoção

#### `remove(k)`
Remove a chave `k` da árvore. Abrange todos os casos da literatura:

**Caso especial:** árvore com apenas a raiz — remove diretamente.

**Caso 1 — remoção em folha:**
- **1a** — folha com mais que o mínimo (`> t - 1`): remove diretamente
- **1b** — folha no mínimo, mas um irmão tem chaves extras: pega emprestado do irmão via `passKey`, depois remove
- **1c** — folha e irmãos no mínimo: funde nós via `merge`, depois tenta remover novamente

**Caso 2 — remoção em nó interno:**
- **2a** — filho esquerdo ou direito tem mais que o mínimo: substitui a chave pelo predecessor ou sucessor, remove do filho
- **2b** — ambos os filhos no mínimo: funde os dois filhos com a chave via `merge`, depois tenta remover novamente

#### `findParent(current, child)`
Busca recursivamente o nó pai de `child` a partir de `current`.

#### `passKey(node, sibling, parent, index, pm)`
Transfere uma chave do `parent` para `node`, pegando uma chave compensatória de `sibling` (empréstimo entre irmãos).

#### `merge(node, sibling, parent, index, pm)`
Funde dois nós irmãos quando ambos estão no número mínimo de chaves. Se o pai também não tiver chaves extras, a fusão pode cascatear recursivamente para cima (envolvendo o avô).

---

### Busca

#### `search(k, node)`
Busca a chave `k` recursivamente a partir de `node`:
- Se `k` for igual a alguma chave do nó atual, retorna o nó
- Se o nó for folha, retorna `None` (não encontrado)
- Caso contrário, desce para o filho apropriado

---

### Visualização

#### `printTree(node, level)`
Exibe a árvore formatada por níveis, mostrando as chaves de cada nó e indicando raiz e folhas.

---

## Segurança contra loops infinitos

Todas as funções recursivas (`goLeaf`, `findParent`, `split`, `insert`, `search`, `merge`, `remove`) possuem o parâmetro `_call_depth` com valor padrão `0`. A cada chamada recursiva, o valor é incrementado. Se `MAX_DEPTH` (100) for excedido, um `RecursionError` é levantado, prevenindo loops infinitos causados por bugs na lógica de reorganização.

---

## Testes

A função `main()` executa uma bateria de testes manuais que cobrem todos os casos de inserção e remoção:

1. **Inserções** — 20 chaves inseridas, alternando entre nós com espaço e nós cheios (disparando splits)
2. **Remoções** — 13 chaves removidas, cobrindo os casos 1a, 1b, 1c, 2a, 2b e remoção recursiva
3. **Pesquisas** — busca de chave existente (28) e inexistente (51)

---

## Grau mínimo `t`

O grau `t` é definido na criação da árvore:

```python
btree = bTree(2)  # grau mínimo = 2
```

Com `t = 2` (usado nos testes), cada nó pode ter entre 1 e 3 chaves, e cada nó não-folha tem entre 2 e 4 filhos.
