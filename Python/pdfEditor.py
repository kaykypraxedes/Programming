import os
import shlex
import re
from pypdf import PdfReader, PdfWriter
from PIL import Image

# Função para verificar se o nome já está em uso (não sobrescrever)
def verificaNome(nome):
    base, ext = os.path.splitext(nome) # base = "/caminho/arquivo", ext = ".extensão"
    match = re.search(r"\((\d+)\)$", base) # Procura (num) ao final
    if match: # Remove "(numero)" no final se existir
        base = base[:match.start()]
    i = 1
    while os.path.exists(nome): # Verifica se um arquivo com esse nome já existe
        nome = f"{base}({i}){ext}"
        i += 1
    return nome
# Função para converter imagens (.jpg, .png, etc.) para pdf
def imagemParaPdf(arquivoImagem, arquivoSaida=None):
    try:
        if arquivoSaida is None:
            nome = os.path.splitext(arquivoImagem)[0] # Arquivo sem extensão
            arquivoSaida = nome + ".pdf" # Muda a extensão para .pdf
            arquivoSaida = verificaNome(arquivoSaida)
        imagem = Image.open(arquivoImagem)
        if imagem.mode == "RGBA":
            imagem = imagem.convert("RGB")
        imagem.save(arquivoSaida, "PDF")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False
# Função para mesclar pdfs
def mesclarPdf(arquivosEntrada):
    try:
        nomeBase = os.path.splitext(arquivosEntrada[0])[0] # Arquivo sem extensão
        arquivoSaida = nomeBase + "(mesclado).pdf"
        arquivoSaida = verificaNome(arquivoSaida)
        escritor = PdfWriter()
        for arquivo in arquivosEntrada:
            extensao = os.path.splitext(arquivo)[1].lower()
            # Verifica se o arquivo a ser anexado é uma imagem
            if extensao in [".png", ".jpg", ".jpeg", ".bmp", ".tiff"]:
                nomeBase = os.path.splitext(arquivo)[0]
                temp_pdf = nomeBase + "_temp.pdf"
                imagemParaPdf(arquivo, temp_pdf)
                leitor = PdfReader(temp_pdf)
                os.remove(temp_pdf)
            else:
                leitor = PdfReader(arquivo)
            for pagina in leitor.pages:
                escritor.add_page(pagina)
        with open(arquivoSaida, "wb") as saida:
            escritor.write(saida)
        print(f"📁 Arquivo salvo: {arquivoSaida}")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False
# Função para dividir o pdf
def extrairPdf(paginaInicial, paginaFinal, arquivoEntrada):
    try:
        nomeBase = os.path.splitext(arquivoEntrada)[0] # Arquivo sem extensão
        arquivoSaida = f"{nomeBase}({paginaInicial}-{paginaFinal}).pdf"
        arquivoSaida = verificaNome(arquivoSaida)
        # Ler o PDF original
        leitor = PdfReader(arquivoEntrada)
        escritor = PdfWriter()
        # Verificar limites
        totalPaginas = len(leitor.pages)
        if paginaInicial < 1 or paginaFinal > totalPaginas:
            print(f"Erro: O PDF tem {totalPaginas} páginas")
            return False
        # Adicionar páginas ao novo PDF
        for paginaNum in range(paginaInicial - 1, paginaFinal):
            escritor.add_page(leitor.pages[paginaNum])
        # Salvar o arquivo
        with open(arquivoSaida, 'wb') as arquivo:
            escritor.write(arquivo)
        print(f"📁 Arquivo salvo: {arquivoSaida}")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False

if __name__ == "__main__":
    # Menu pensado para rodar no terminal através de um arquivo.sh
    opcao = int(input(""))
    match opcao:
        case 1:
            original = shlex.split(input("Arquivo (\"/caminho/do/arquivo.pdf\"): "))[0]
            paginas = list(map(int, input("Páginas dos pdfs (n-n-n-n-n-n): ").split("-")))
            for i in range (1, len(paginas)):
                adicional = 0 if i == 1 else 1
                extrairPdf(paginas[i-1] + adicional, paginas[i], original)
        case 2:
            arquivos = shlex.split("Arquivos (\"/caminho/do/a1.pdf\" \"/caminho/do/a2.jpg\" \"/caminho/do/a3.png\"): ")
            mesclarPdf(arquivos)
        case 3:
            original = shlex.split(input("Imagem (\"/caminho/do/arquivo.extensão\"): "))[0]
            imagemParaPdf(original)
        case _:
            print("Opção inválida!")
