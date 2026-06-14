# pdfEditor.py
# - Desenvolvedor:
#     Kayky Moreira Praxedes
import os
import re
import shlex

from PIL import Image
from pypdf import PdfReader, PdfWriter


# FUNÇÕES INTERMEDIÁRIAS
# Função para verificar se o nome já está em uso (não sobrescrever)
def defineNomeDisponivel(nome):  # (str) -> (str)
    base, ext = os.path.splitext(nome)  # base = "/caminho/arquivo", ext = ".extensão"
    match = re.search(
        r"\(\d+\)$", base
    )  # Verifica se já existe uma cópia ("arquivo(n).pdf") e aumenta o número
    if match:
        base = base[: match.start()]
    i = 1
    novoNome = nome
    while os.path.exists(novoNome):  # Verifica qual número colocar
        novoNome = f"{base}({i}){ext}"
        i += 1
    return novoNome


# Em caso de imagens .png, coloca um fundo branco onde era transparente
def removerTransparencia(img):  # (Image) -> (Image)
    if img.mode in ("RGBA", "LA") or (img.mode == "P" and "transparency" in img.info):
        fundo = Image.new("RGB", img.size, (255, 255, 255))
        fundo.paste(img, mask=img.convert("RGBA").split()[3])
        return fundo
    return img.convert("RGB")


# Retorna qual vai ser a largura dos arquivos
def calculaLargura(arquivos):  # (str[]) -> (int)
    for arquivo in arquivos:
        ext = os.path.splitext(arquivo)[1].lower()
        if ext == ".pdf":
            leitor = PdfReader(arquivo)
            return leitor.pages[0].mediabox.width
    return 595


# Redimensiona as páginas de um PDF para uma largura padrão (mantendo proporção)
def redimensionaPdf(pagina, larguraMax):  # (PageObject, int) -> (PageObject)
    # Calcula a proporção e novas dimensões
    proporcao = larguraMax / pagina.mediabox.width
    novaLargura = proporcao * pagina.mediabox.width
    novaAltura = proporcao * pagina.mediabox.height
    # Aplica a transformação de escala
    pagina.scale_to(novaLargura, novaAltura)
    return pagina


# FUNÇÕES DO PROGRAMA
# Função para dividir o pdf
def extrairPdf(paginaInicial, paginaFinal, arquivoEntrada):  # (int, int, str) -> (bool)
    try:  # Cria o nome do arquivo de saída
        nomeBase = os.path.splitext(arquivoEntrada)[0]
        arquivoSaida = f"{nomeBase}({paginaInicial}-{paginaFinal}).pdf"
        arquivoSaida = defineNomeDisponivel(arquivoSaida)  # (str) -> (str)
        # Elementos para operações com PDFs
        leitor = PdfReader(arquivoEntrada)
        escritor = PdfWriter()
        # Verifica o limite de páginas
        totalPaginas = len(leitor.pages)
        if paginaInicial < 1 or paginaFinal > totalPaginas:
            print(f"Erro: O PDF tem {totalPaginas} páginas")
            return False
        # Adiciona as páginas ao novo PDF
        for paginaNum in range(paginaInicial - 1, paginaFinal):
            escritor.add_page(leitor.pages[paginaNum])
        # Cria o arquivo final e salva ele
        with open(arquivoSaida, "wb") as arquivo:
            escritor.write(arquivo)
        print(f"📁 Arquivo salvo: {arquivoSaida}")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False


# Função para mesclar arquivos em 1 pdf
def mesclarPdf(arquivosEntrada):  # (str[]) -> (bool)
    try:  # Cria o nome do arquivo de saída
        nomeBase = os.path.splitext(arquivosEntrada[0])[0]
        arquivoSaida = nomeBase + "(mesclado).pdf"
        arquivoSaida = defineNomeDisponivel(arquivoSaida)  # (str) -> (str)
        # Elemento para operação com PDF (escrita)
        escritor = PdfWriter()
        largura = calculaLargura(arquivosEntrada)
        # Mescla os arquivos no escritor
        for arquivo in arquivosEntrada:
            extensao = os.path.splitext(arquivo)[1].lower()
            temp_pdf = None
            if extensao in [
                ".png",
                ".jpg",
                ".jpeg",
                ".bmp",
                ".tiff",
            ]:  # É um arquivo de imagem
                nomeBase = os.path.splitext(arquivo)[0]
                temp_pdf = nomeBase + "_temp.pdf"
                imagemParaPdf(
                    arquivo, temp_pdf, largura
                )  # (int, str, str, str[]) -> (bool)
                leitor = PdfReader(temp_pdf)
                # Adiciona a imagem redimensionada ao arquivo mesclado
                escritor.add_page(leitor.pages[0])
            else:  # Arquivo é um .pdf
                leitor = PdfReader(arquivo)
                largura = leitor.pages[-1].mediabox.width
                # Adiciona as páginas ao novo arquivo mesclado
                for pagina in leitor.pages:
                    escritor.add_page(pagina)
            if temp_pdf:  # Apaga os tmp files
                os.remove(temp_pdf)
        with open(arquivoSaida, "wb") as saida:
            escritor.write(saida)
        print(f"📁 Arquivo salvo: {arquivoSaida}")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False


# Função para converter imagens (.jpg, .png, etc.) para pdf
def imagemParaPdf(
    arquivoImagem, arquivoSaida=None, largura=None
):  # (int, str, str=None, int=None) -> (bool)
    try:  # Cria o nome do arquivo de saída
        nome = "Nada"
        if arquivoSaida is None:
            nome = os.path.splitext(arquivoImagem)[0]
            nome = nome + ".pdf"  # Muda a extensão para .pdf
            nome = defineNomeDisponivel(nome)  # (str) -> (str)
        else:
            nome = arquivoSaida
        # Coloca fundo branco em imagens .png
        imagem = Image.open(arquivoImagem).convert("RGBA")
        imagem = removerTransparencia(imagem)
        imagem.save(nome, "PDF")
        # Redimensiona a imagem se necessário
        if largura is not None:
            leitor = PdfReader(nome)
            escritor = PdfWriter()
            escritor.add_page(redimensionaPdf(leitor.pages[0], largura))
            with open(nome, "wb") as folha:
                escritor.write(folha)
        # Verifica se é um arquivo temporário (chamado pelo mesclaPdf())
        if arquivoSaida is None:
            print(f"📁 Arquivo salvo: {nome}")
        return True
    except Exception as e:
        print(f"❌ Erro: {e}")
        return False


if __name__ == "__main__":
    opcao = int(
        input(
            "Escolha uma opção (apenas o número):\n"
            "(1)- Extrair páginas de PDF\n"
            "(2)- Mesclar PDFs / imagens\n"
            "(3)- Converter imagem para PDF\n"
            "(0)- Encerrar a aplicação\n- "
        )
    )
    match opcao:
        case 1:  # Separa o PDF
            original = shlex.split(
                input('Insira o path entre aspas ("") do arquivo (com extensão)\n- ')
            )[0]
            # Remove todos os espaços
            textoFormatado = input(
                "Digite as páginas (ex: 1-3-10, 12-15, 1-7-9)\n- "
            ).replace(" ", "")
            conjuntos = textoFormatado.split(",")
            for elemento in conjuntos:
                paginas = list(map(int, elemento.split("-")))
                for i in range(1, len(paginas)):
                    adicional = 0 if i == 1 else 1
                    # (int, int, str) -> (bool)
                    extrairPdf(paginas[i - 1] + adicional, paginas[i], original)
        case 2:  # Mescla os arquivos
            entrada = input(
                'Insira os paths entre aspas ("") do arquivos (com extensão), separados por "|"\n- '
            )
            arquivos = [shlex.split(x)[0] for x in entrada.split("|")]
            mesclarPdf(arquivos)  # (str[]) -> (bool)
        case 3:  # Converte imagem para PDF
            original = shlex.split(
                input('Insira o path entre aspas ("") do arquivo (com extensão)\n- ')
            )[0]
            tipoDeConversao = int(
                input(
                    "Tipo de conversão:\n"
                    "(1)- Conversão direta\n"
                    "(2)- Imagem redimensionada para largura de folha A4\n- "
                )
            )
            conversao = None
            if tipoDeConversao == 2:
                conversao = 595
            imagemParaPdf(original, None, conversao)  # (str) -> (bool)
        case 0:
            print("Operação encerrada!")
        case _:
            print("❌ Opção inválida!")
