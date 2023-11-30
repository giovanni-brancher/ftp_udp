#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define TAM_PACOTE 2048

const char *obterNomeArquivo(const char *caminho);
int obterNumPacotesArquivo(const char *caminho);

int main(int argc, char *argv[])
{
    int cont, i, src, dest, j, k;
    int buffer[TAM_PACOTE]; // tipo int tem 4 bytes, logo tamanho do buffer é 4 * TAM_PACOTE

    // Valida número de argumentos
    if (argc != 3)
    {
        printf("Numero de argumentos insuficientes\n");
        exit(1);
    }

    // Valida ação a ser realizada (download/upload)
    if (strcmp(argv[1], "upload") == 0)
    {
        printf("Iniciando upload...\n");
    }
    else
    {
        printf("deu ruim\n");
        return 0;
    }

    // Abre o arquivo para upload
    char *arq = argv[2];
    int numPacotesArquivo = obterNumPacotesArquivo(arq);

    src = open(arq, O_RDONLY);
    if (src == -1)
    {
        printf("Impossivel abrir o arquivo %s\n", argv[1]);
        exit(1);
    }

    // dest = creat(argv[2], 0666);
    char str1[50] = "uploads/"; // diretorio padrão
    const char *nomeArquivo = obterNomeArquivo(arq);
    strcat(str1, nomeArquivo); // Concatena

    printf("Realizando upload de %s\n", nomeArquivo);
    dest = creat(str1, 0666);
    if (dest == -1)
    {
        printf("Impossivel criar o arquivo %s\n", argv[2]);
        exit(1);
    }

    int pacoteAtual = 1;

    while ((cont = read(src, &buffer, sizeof(buffer))) > 0)
    {
        write(dest, &buffer, cont);
        int porcentagemConcluida = pacoteAtual * 100 / numPacotesArquivo;
        printf("(%s) Pacote %d / %d enviado com sucesso (%d%%)\n", nomeArquivo, pacoteAtual, numPacotesArquivo, porcentagemConcluida);
        pacoteAtual++;
    }

    printf("Terminei a copia");
    return 0;
}

const char *obterNomeArquivo(const char *caminho)
{
    char *ultima_barra = strrchr(caminho, '/');
    if (ultima_barra != NULL)
        return ultima_barra + 1; // Retorna o texto após a última barra
    else
        return caminho; // Retorna a string original se não houver barra
}

int obterNumPacotesArquivo(const char *caminho)
{
    int src = open(caminho, O_RDONLY);
    off_t tamanho = lseek(src, 0, SEEK_END); // Move o cursor para o final do arquivo

    if (tamanho == -1)
    {
        perror("Erro ao obter o tamanho do arquivo");
        close(src);
        return 1;
    }

    // Move o cursor de volta para o início do arquivo
    if (lseek(src, 0, SEEK_SET) == -1)
    {
        perror("Erro ao mover o cursor para o início do arquivo");
        close(src);
        return -1;
    }

    double numPacotes = tamanho / TAM_PACOTE / sizeof(int);

    if (numPacotes * (TAM_PACOTE / sizeof(int)) < tamanho)
    { // testa se tem resto
        numPacotes++;
    }

    return numPacotes;
}