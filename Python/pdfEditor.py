import os
import shlex
import re
from pypdf import PdfReader, PdfWriter
from PIL import Image

# FUNÇÕES INTERMEDIÁRIAS
# Função para verificar se o nome já está em uso (não sobrescrever)
def verificaNome(nome): 
    # (str) -> (str)
    base, ext = os.path.splitext(nome) # base = "/caminho/arquivo", ext = ".extensão"
     # Procura um "(numero)" no nome do arquivo e só remove se estiver no final
    match = re.search(r"\(\d+\)$", base)
    if match:
        base = base[:match.start()]
    i = 1
    # Verifica a disponibilidade de um novo nome e testa até encontrar
    novoNome = nome
    while os.path.exists(novoNome):
        novoNome = f"{base}({i}){ext}"
        i += 1
    return novoNome

# Redimensiona o tamanho de uma imagem
def redimensionaImagem(imagem, larguraMax, alturaMax): 
    # (Image, int, int) -> (Image)
    # Calcula a proporção
    proporcao = min(larguraMax / imagem.width, alturaMax / imagem.height)
    novaLargura = int(imagem.width * proporcao)
    novaAltura = int(imagem.height * proporcao)
    return imagem.resize((novaLargura, novaAltura), Image.LANCZOS)

# Coloca uma imagem no centro de uma folha branca
def imagemFolha(imagem, arquivos): 
    # (Image, str[]) -> (Image)
    margem = 14 # Aproximadamente 0.5 cm
    larguraPag = 595 # Padrão para folha A4
    alturaPag = 842
    # As imagens são redimensionadas com base no primeiro PDF da lista
    if arquivos is not None:
        pagina=None
        for arquivo in arquivos:
            ext = os.path.splitext(arquivo)[1].lower()
            if ext == ".pdf":
                leitor = PdfReader(arquivo)
                pagina = leitor.pages[0]
                break
        larguraPag, alturaPag = pagina.mediabox.width, pagina.mediabox.height
    # Cria uma página do tamanho do padrão para insetir a imagem redimensionada
    paginaBranca = Image.new("RGB", (int(larguraPag), int(alturaPag)), (255, 255, 255))
    maxLargura = larguraPag - 2 * margem
    maxAltura = alturaPag - 2 * margem
    imagem = redimensionaImagem(imagem, maxLargura, maxAltura) # (Image, int, int) -> (Image)
    # Calcula posição centralizada
    posicaoX = int((larguraPag - imagem.width) / 2)
    posicaoY = int((alturaPag - imagem.height) / 2)
    paginaBranca.paste(imagem, (posicaoX, posicaoY))
    return paginaBranca

# FUNÇÕES DO PROGRAMA
# Função para dividir o pdf
def extrairPdf(paginaInicial, paginaFinal, arquivoEntrada):
    # (int, int, str) -> (bool)
    try: 
        # Cria o nome do arquivo de saída
        nomeBase = os.path.splitext(arquivoEntrada)[0]
        arquivoSaida = f"{nomeBase}({paginaInicial}-{paginaFinal}).pdf"
        arquivoSaida = verificaNome(arquivoSaida) # (str) -> (str)
        # Elementos para operações com PDFs
        leitor = PdfReader(arquivoEntrada)
        escritor = PdfWriter()
        totalPaginas = len(leitor.pages) # Verifica o limite de páginas
        if paginaInicial < 1 or paginaFinal > totalPaginas:
            print(f"Erro: O PDF tem {totalPaginas} páginas")
            return False
        # Adiciona as páginas ao novo PDF
        for paginaNum in range(paginaInicial - 1, paginaFinal):
            escritor.add_page(leitor.pages[paginaNum])
        # Cria o arquivo final e salva ele
        with open(arquivoSaida, 'wb') as arquivo:
            escritor.write(arquivo)
        print(f"📁 Arquivo salvo: {arquivoSaida}")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False
    
# Função para mesclar pdfs
def mesclarPdf(arquivosEntrada): 
    # (str[]) -> (bool)
    try:
        # Cria o nome do arquivo de saída
        nomeBase = os.path.splitext(arquivosEntrada[0])[0]
        arquivoSaida = nomeBase + "(mesclado).pdf"
        arquivoSaida = verificaNome(arquivoSaida) # (str)
        # Elemento para operação com PDF (escrita)
        escritor = PdfWriter()
        for arquivo in arquivosEntrada:
            extensao = os.path.splitext(arquivo)[1].lower()
            # Verifica se o arquivo a ser anexado é uma imagem
            temp_pdf = None
            if extensao in [".png", ".jpg", ".jpeg", ".bmp", ".tiff"]:
                nomeBase = os.path.splitext(arquivo)[0]
                temp_pdf = nomeBase + "_temp.pdf"
                # (int, str, str, str[]) -> (bool)
                imagemParaPdf(2, arquivo, temp_pdf, arquivosEntrada)
                leitor = PdfReader(temp_pdf)
            else: # Arquivo é um .pdf
                leitor = PdfReader(arquivo)
            # Adiciona as páginas ao novo arquivo mesclado e apaga os tmp files
            for pagina in leitor.pages:
                escritor.add_page(pagina)
            if temp_pdf:
                os.remove(temp_pdf)
        with open(arquivoSaida, "wb") as saida:
            escritor.write(saida)
        print(f"📁 Arquivo salvo: {arquivoSaida}")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False
    
# Função para converter imagens (.jpg, .png, etc.) para pdf
def imagemParaPdf(tipoConversao, arquivoImagem, arquivoSaida=None, arquivos=None):
    # (int, str, str=None, str[]=None) -> (bool)
    try:
        # Cria o nome do arquivo de saída
        if arquivoSaida is None:
            nome = os.path.splitext(arquivoImagem)[0]
            arquivoSaida = nome + ".pdf" # Muda a extensão para .pdf
            # (str) -> (str)
            arquivoSaida = verificaNome(arquivoSaida)
        imagem = Image.open(arquivoImagem)
        if (tipoConversao == 1): # Conversão do arquivo diretamente
            if imagem.mode != "RGB":
                imagem = imagem.convert("RGB")
        else: # Coloca a imagem em um PDF A4 branco
            # (Image, str[]) -> (Image)
            imagem = imagemFolha(imagem, arquivos)
        imagem.save(arquivoSaida, "PDF")
        if arquivos is None: print(f"📁 Arquivo salvo: {arquivoSaida}")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False

if __name__ == "__main__":
    opcao = int(input("Escolha uma opção (apenas o número):\n"
                        "(1)- Extrair páginas de PDF\n"
                        "(2)- Mesclar PDFs / imagens\n"
                        "(3)- Converter imagem para PDF\n- "))
    match opcao:
        case 1: # Separa o PDF
            original = shlex.split(input("Insira o path entre aspas (\"\") do arquivo (com extensão)\n- "))[0]
            # Remove todos os espaços
            textoFormatado = input("Digite as páginas (ex: 1-3-10, 12-15, 1-7-9)\n- ").replace(" ", "")
            conjuntos = textoFormatado.split(",")
            for elemento in conjuntos:
                paginas = list(map(int, elemento.split("-")))
                for i in range (1, len(paginas)):
                    adicional = 0 if i == 1 else 1
                    # (int, int, str) -> (bool)
                    extrairPdf(paginas[i-1] + adicional, paginas[i], original)
        case 2: # Mescla os arquivos
            entrada = input("Insira os paths entre aspas (\"\") do arquivos (com extensão), separados por \"|\"\n- ")
            arquivos = [shlex.split(x)[0] for x in entrada.split("|")]
            mesclarPdf(arquivos) # (str[]) -> (bool)
        case 3: # Converte imagem para PDF
            original = shlex.split(input("Insira o path entre aspas (\"\") do arquivo (com extensão)\n- "))[0]
            tipoDeConversao = int(input("Tipo de conversão:\n"
                                        "(1)- Conversão direta\n"
                                        "(2)- Imagem em folha\n- "))
            imagemParaPdf(tipoDeConversao, original) # (int, str) -> (bool)
        case _:
            print("❌ Opção inválida!")
