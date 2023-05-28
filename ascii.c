/*---------------------------------------------------------------+
|            UNIFAL - Universidade Federal de Alfenas            |
|              BACHARELADO EM CIÊNCIA DA COMPUTAÇÃO              |
|       Trabalho....:   Imagem ASCII                             |
|       Disciplina..:   Processamento de Imagens                 |
|       Professor...:   Luiz Eduardo da Silva                    |
|       Aluno.......:   Daniel da Costa Lima                     |
|       Data........:   01/05/2023                               |
+----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "imagelib.h"

void img_info2 (int nr, int nc, int nrr, int ncr, int ml) {
    char name[] = "result.txt";

    printf("Transformations: ...........: sampling and quantization\n");
    printf("Final file name ............: %s \n", name);
    printf("Number of rows..............: %d \n", nrr);
    printf("Number of columns...........: %d \n", ncr);
    printf("Rows scale..................: 1:%d \n", nr/nrr);
    printf("Columns scale...............: 1:%d \n", nc/ncr);
    printf("Max intensity level.........: %d \n\n", ml);
}

/*-----------------------------------------------------------------------------------
*   Função que reduz a dimensão da imagem de entrada de acordo com os parâmetros
*-----------------------------------------------------------------------------------*/ //”@$#*%o!=+;: -,. 
void amostragem (image In, image Out, int nr, int nc, int ncr, int nrr) {
    int pc = nc/ncr;    // Cálculo da proporção de colunas (colReal/colAmostra)
    int pr = nr/nrr;    // Cálculo da proporção de linhas (linReal/linAmostra)
  
    for (int i = 0, j = 0; i < nr; i += pr) {
        for (int k = 0; k < nc; k += pc) {
            Out[j++] = In[i*nc + k];
        }
    }  
}

/*-----------------------------------------------------------------------------------
*   Função que copia os dados de uma imagem, sem o cabeçalho, para um arquivo.txt 
*-----------------------------------------------------------------------------------*/
void copiaArq (char *name, int nc) {
    char lines[100];
    FILE *file1, *file2;

    /* Abertura da imagem */
    file1 = fopen(name, "r");
    if (file1 == NULL) { 
        printf("Erro na abertura de arquivo: aqui <%s>\n", name);
        exit(0); 
    }

    /* Abertura do arquivo.txt */
    file2 = fopen("result.txt", "w");
    
    /* Número mágico P1, P2 ou P3 */
    fgets(lines, 80, file1);

    /* Linhas de comentários e dimensão da imagem */
    fgets(lines, 80, file1);
    while (strchr(lines, '#')) {
        fgets(lines, 80, file1);
    }

    /* Máximo de cinza */
    fgets(lines, 80, file1);

    /* Copiando o conteúdo da imagem pro arquivo.txt sem o cabeçalho e transformando cada valor inteiro em caracter */
    int i = 0, n;
    char caracter;

    while (fscanf(file1, "%d", &n) == 1) {      // Lê um valor no arquivo, armazena na variável "n" e retorna 1. Caso não consiga ler algum valor, retorna 0
        if (i == nc){     // Quebra de linha de acordo com a quantidade de colunas (semelhante a função img_put)
            fprintf(file2, "%c", '\n');
            i = 0;
        }
        
        caracter = (char)n;     // A variável do tipo char recebe a variável do tipo int convertida para char
        fprintf(file2, "%c", caracter);
        i++;
    }

    fclose(file1);
    fclose(file2);
}

/*-----------------------------------------------------------------------------------
*   Função que realiza o mapeamento dos tons de cinza para cada caracter informado 
*   na lista de argumentos da chamada do programa.
*   Lógica do mapeamento:    
*       - Proporção: Máximo tom de cinza / Quantidade de caracteres da string
*       - Escala de cinza: Valor do pixel / Proporção
*   A escala começa em 0 e pode chegar ao valor máximo que corresponde ao tamanho
*   da string. Esse valor é usado para indicar o caracter corresponde na string e,
*   assim, sobrescrever o pixel com o valor do caracter do tipo INTEIRO. Caso o 
*   caracter seja 0, o seu valor será substituído por 32, para que corresponda ao
*   "espaço" no momento da conversão e não "NULL"; 
*-----------------------------------------------------------------------------------*/
void quantizacao (image In, int nc, int nr, char *caracteres, int mn) {
    int tamString = 0;
    int pMn;

    tamString = strlen(caracteres);

    pMn = mn/tamString;     // Proporção para substuir os tons de cinza da imagem

    int difZero;
    for (int i = 0; i < nr * nc; i++) {
        difZero = caracteres[In[i]/pMn];
        if (difZero == 0) {         // Caso o caracter a ser impresso seja 0 (nulo), será impresso o caracter 32 (espaço)
            In[i] = 32;     
        } else {
            In[i] = difZero;
        }
    }
}

void intensidade(image In, image Out, int nl, int nc, int mn)
{
    float T[mn + 1];
    float expo = 2;
    for (int i = 0; i < mn + 1; i++)
    {
        //--- Transformacao Potência/Raiz
        T[i] = pow(i, expo) / pow(mn, expo) * mn;
        //--- Transformacao Logaritmica
        // T[i] = log(i + 1) / log(mn + 1) * mn;
        // printf("T[%d] = %.0f\n", i, T[i]);
    }
    for (int i = 0; i < nl * nc; i++)
        Out[i] = T[In[i]];
}

void msg(char *s)
{
    printf("\nArte ASCII");
    printf("\n-------------------------------");
    printf("\nUso:  %s  nome-imagem[.pgm] \n\n", s);
    printf("    nome-imagem[.pgm] é o nome do arquivo da imagem \n");
    exit(1);
}

/*-------------------------------------------------------------------------
 * main function
 *-------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
/*-------------------------------------------------------------------------
*   nc - número de colunas
*   nr - número de linhas
*   ml ou mn - valor máximo de intensidade
*   ncr - número de colunas redimensionado
*   nrr - número de linhas redimensionadas
*-------------------------------------------------------------------------*/
    int nc, nr, ml, ncr, nrr;
    char *p, nameIn[100], nameOut[100], cmd[110];
    image In, Out, Out2;

    if (argc < 2)
        msg(argv[0]);

    img_name(argv[1], nameIn, nameOut, GRAY);

    //-- read image
    In = img_get(nameIn, &nr, &nc, &ml, GRAY);

    Out = img_alloc(nrr, ncr);      // Alocando memória com a proporção da nova imagem

    // -- atribuição das novas colunas e linhas da imagem resultado
    ncr = atoi(argv[2]);        // Conversão de char para int necessária
    nrr = atoi(argv[3]);

    // -- amostragem
    amostragem(In, Out, nr, nc, ncr, nrr);

    //-- quantização
    quantizacao(Out, ncr, nrr, argv[4], ml);

    img_info2(nr, nc, nrr, ncr, ml/strlen(argv[4]));

    //-- save image
    img_put(Out, nameOut, nrr, ncr, ml, GRAY);  // Mudança dos parâmetros nc e nr para nrr e ncr para que a imagem resultado tivesse a proporção desejada
    // sprintf(cmd, "%s %s &", VIEW, nameOut);  // Comando para exibir a imagem no irfanview
    // system(cmd);

    // -- escrita no arquivo.txt
    copiaArq(nameOut, ncr);

    img_free(In);
    img_free(Out);

    return 0;
}
