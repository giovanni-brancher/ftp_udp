#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define TAM_PACOTE 2048
#define TAM_JANELA 4

int obterNumPacotesArquivo(const char *caminho);
const char *obterNomeArquivo(const char *caminho);
char *validarComandoEntrada(int argc, char *argv[]);

struct Pacote
{
    int numero;
    int buffer[TAM_PACOTE];
    int ack;
};

int main(int argc, char *argv[])
{
    int cont, i, src, dest, j, k;
    int buffer[TAM_PACOTE]; // int tem 4 bytes, tamanho do buffer é 4 * TAM_PACOTE
    char *acao = validarComandoEntrada(argc, argv);

    if (strcmp(acao, "upload") == 0)
    {
        printf("Preparando Upload...\n");
        char *caminhoArquivo = argv[2];
        int numPacotesTotal = obterNumPacotesArquivo(caminhoArquivo);

        // Abre o arquivo
        src = open(caminhoArquivo, O_RDONLY);
        if (src == -1)
        {
            printf("Impossivel abrir o arquivo %s\n", caminhoArquivo);
            exit(1);
        }

        char destinoArquivo[50] = "uploads/";
        const char *nomeArquivo = obterNomeArquivo(caminhoArquivo);
        strcat(destinoArquivo, nomeArquivo); // Monta caminho destino do arquivo

        printf("Iniciando Upload de %s\n", nomeArquivo);
        dest = creat(destinoArquivo, 0666);

        if (dest == -1)
        {
            printf("Impossivel criar o arquivo %s\n", caminhoArquivo);
            exit(1);
        }

        int pacoteAtual = 1;

        while ((cont = read(src, &buffer, sizeof(buffer))) > 0)
        {
            // aqui realiza upload

            write(dest, &buffer, cont);
            int porcentagemConcluida = pacoteAtual * 100 / numPacotesTotal;
            // printf("(%s) Pacote %d / %d enviado com sucesso (%d%%)\n", nomeArquivo, pacoteAtual, numPacotesTotal, porcentagemConcluida);
            pacoteAtual++;
        }

        printf("Terminei a copia");
    }

    if (strcmp(acao, "download") == 0)
    {
        printf("Solicita os pacotosos do servidor\n");
    }

    return 0;
}

char *validarComandoEntrada(int argc, char *argv[])
{
    // Valida número de argumentos
    if (argc != 3)
    {
        printf("Numero de argumentos insuficientes\n");
        exit(1);
    }

    // Valida ação a ser realizada (download/upload)
    if (strcmp(argv[1], "upload") != 0 && strcmp(argv[1], "download") != 0)
    {
        printf("Parametro incorreto, encerrando...\n");
        exit(1);
    }

    return argv[1];
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