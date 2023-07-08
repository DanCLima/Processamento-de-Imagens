/*---------------------------------------------------------------+
|            UNIFAL - Universidade Federal de Alfenas            |
|              BACHARELADO EM CIÊNCIA DA COMPUTAÇÃO              |
|       Trabalho....:   Esteganografia                           |
|       Disciplina..:   Processamento de Imagens                 |
|       Professor...:   Luiz Eduardo da Silva                    |
|       Aluno.......:   Daniel da Costa Lima                     |
|       Data........:   01/07/2023                               |
+----------------------------------------------------------------*/

/*---------------------------------------------------------
 * Image Processing
 * 
 * Steganography - decode
 *---------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "imagelib.h"
#define DEBUG(x) x

#define R 0
#define G 1
#define B 2

/*---------------------------------------------------------------------------------------------------------
    Recebe uma banda com valores RGB somados bit a bit. Realiza a divisão dessa soma, de acordo
    com a cor, e retorna o último bit dessa cor 
---------------------------------------------------------------------------------------------------------*/
int ultimoBit(int pixel, int *banda) {
    int px;

    if (*banda == R) {
        px = (pixel >> 16) & 0x01;        //Pega o último bit do R  
        (*banda)++;                       // Anda do R para o G
    } else if(*banda == G) {             
        px = (pixel >> 8) & 0x01;         //Pega o último bit do G 
        (*banda)++;                       //Anda do G para o B
    } else if (*banda == B) {
        px = pixel & 0x01;              //Pega o último bit do B  
        *banda = R;                       //Anda do B para o R
    }
    return px;
}

/*--------------------------------------------------------------------------------------------------------- 
    Faz um deslocamento bit a bit de 1 casa para a esquerda do primeiro parâmetro. Em seguida, 
    reliza um "ou" lógico bit a bit com a máscara 0x01 para concatenar o segundo parâmetro 
    (um bit) ao primeiro (um valor inteiro).
---------------------------------------------------------------------------------------------------------*/
int ouBin(int byte, int bit) {
    return ((byte << 1) | bit);
}

void decode(image In)
{
    FILE *file;
    char name[100] = "";
    int fsize = 0;
    unsigned char byte = 0;
    int banda = R;
    int px = 0;
    int nc, nr, pos;
    int contaBit;
    
    contaBit = 0;
    // decode the name
    for (int i = 0; fsize >= 0; i++, contaBit++)  {         //A variável "fsize" foi usado como condição de parada apenas para não criar outra variável
        if (contaBit < 8) {
            px = ouBin(px, (ultimoBit(In->px[i], &banda)));     //Concatenando os bits até formar 1 byte
        } else {
            byte = (unsigned char)px;           //Quando o byte é formado, a variável "byte" recebe, convertido em char, o valor binário representado na variável "px" do tipo int 
            name[strlen(name)] = byte;          //Atribui à última posição da string "name" o caracter corresponde ao byte  
            if (byte == 0) {
                fsize = -1;                     //Caso o byte seja igual a menor que 0 o loop deve ser interrompido, pois chegamos ao final de "name"
            } else {
                px = 0;
                byte = 0;
                contaBit = 0;
                px = ouBin(px, (ultimoBit(In->px[i], &banda)));                
            }
        }
        pos = i;
    }
    name[strlen(name)] = '\0';          //Fechamento da string "name"
    printf("File name: %s\n", name);
    
    // decode file size
    fsize = 0;
    contaBit = 0;
    for (;contaBit < 32; pos++, contaBit++) {       
        fsize = ouBin(fsize, ultimoBit(In->px[pos], &banda));       //Concatenando os 32 bits decodificados em fsize
    } 
    printf("File size: %d bytes\n", fsize);
    
    // decode file
    file = fopen(name, "wb");
    if (!file)
    {
        printf("Cannot create file %s\n", name);
        exit(10);
    }

    byte = 0;
    contaBit = 0;
    px = 0;
    // contador = 0;
    while (fsize) {
        for (int i = 0; i < 8; i++, pos++) {            //Construindo 1 byte de informação
            px = ouBin(px, (ultimoBit(In->px[pos], &banda)));
        }

        byte = (unsigned char)px;           //Convertendo os 8 bits que formam um valor inteiro e atribuindo à variável byte         
        fprintf(file, "%c", byte);          //Escrevendo 1 byte de informação no arquivo

        px = 0;
        byte = 0;
        contaBit = 0;
  
        fsize--;
    }
    fclose(file);
}

void msg(char *s)
{
    printf("\nSteganography - decode");
    printf("\n-------------------------------");
    printf("\nUSE.:  %s  <IMG.PPM>", s);
    printf("\nWhere:\n");
    printf("    <IMG.PPM>  Image file in PPM ASCII format\n\n");
    exit(20);
}

/*-----------------------------------------
  Main program
------------------------------------------*/
int main(int argc, char *argv[])
{
    char name[100];
    image In;
    char *p;
    if (argc < 2)
        msg(argv[0]);
    strcpy(name, argv[1]);
    p = strstr(name, ".ppm");
    if (p)
        *p = 0;
    strcat(name, ".ppm");
    In = img_get(name, COLOR);
    decode(In);
    img_free(In);
    return 0;
}