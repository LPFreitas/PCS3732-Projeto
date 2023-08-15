/*
 \__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__

  AO PREENCHER ESSE CABE�ALHO COM O MEU NOME E O MEU N�MERO USP,
  DECLARO QUE SOU O �NICO AUTOR E RESPONS�VEL POR ESSE PROGRAMA.
  TODAS AS PARTES ORIGINAIS DESSE EXERC�CIO-PROGRAMA (EP) FORAM
  DESENVOLVIDAS E IMPLEMENTADAS POR MIM SEGUINDO AS INSTRU��ES DESSE EP
  E QUE PORTANTO N�O CONSTITUEM PL�GIO. DECLARO TAMB�M QUE SOU RESPONS�VEL
  POR TODAS AS C�PIAS DESSE PROGRAMA E QUE EU N�O DISTRIBUI OU FACILITEI A
  SUA DISTRIBUI��O. ESTOU CIENTE QUE OS CASOS DE PL�GIO S�O PUNIDOS COM
  REPROVA��O DIRETA NA DISCIPLINA.

  Nome: Lucas Palmiro de Freitas
  Numero USP: 11803396
  MAC0122
  Data: 25/10/2020
  Exerc�cio-Programa 03
  Professora: Cristina Gomes Fernandes

  ep3.c

  Refer�ncias: Com exce��o das rotinas fornecidas no esqueleto e em sala
  de aula, caso voc� tenha utilizado alguma refer�ncia, liste-as abaixo
  para que o seu programa n�o seja considerada pl�gio.

  - fun��o mallocSafe_ep3 copiada de:

       http://www.ime.usp.br/~pf/algoritmos/aulas/aloca.html

  - biblioteca "stackS" utilizada neste programa foi copiada de:

       https://www.ime.usp.br/~cris/mac0122/programas/polonesa/stack4/
       (os arquivos utilizados foram o item.h, o stackS.c e o stackS.h)

 \__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "polinomios.h"
#include "stackS.h"
#include "item.h"
#define MAX 100

void *mallocSafe_ep3(size_t nbytes);

int calcula_indice(char letra);

polinomio calcula_expressao(polinomio var[26], char *infixa);

void faz_operacao(Stack spol, char op);

void descarta_resto_da_linha();

void *mallocSafe_ep3(size_t nbytes)
{
    void *ptr;

    ptr = malloc (nbytes);

    if (ptr == NULL)
    {
        printf ("Socorro! malloc devolveu NULL!\n");
        exit (EXIT_FAILURE);
    }

    return ptr;
}

/*
   a funcao calcula_indice recebe um character entre 'a' e 'z'
   e retorna um inteiro correspodente a "distancia" que esse
   caracter possui em relacao ao caracter 'a' de acordo com a
   tabela ASCII
*/

int calcula_indice(char letra)
{
    return letra - 'a';
}

/*
   a funcao calcula_expressao recebe um vetor de polinomios
   (tipo definido pela biblioteca "polinomios"), os quais
   correspondem aos 26 polinomios que sao armazendos e sobre
   os quais a calculadora realiza operacoes, e recebe uma
   string, a qual representa a expressao em notacao infixa que
   se quer calcular; a funcao devolve um polinomio calculado a
   partir da expressao fornecida, utilizando 2 pilhas (sope -
   pilha de operadores; spol - pilha de polinomios) e a funcao
   faz_operacao

   essa funcao e' inspirada na funcao calcula_expressao
   apresentada na aula 11
*/
polinomio calcula_expressao(polinomio var[26], char *infixa)
{
    int  n = strlen(infixa);
    int  i, ind;
    Stack sope; /* pilha de operadores */
    Stack spol; /* pilha de polinomios */
    char *x;
    polinomio pol, resposta;

    sope = stackInit(n);
    spol = stackInit(n);

    /*
       a precedencia dos operadores e':
       '~' > '*' == '/' == '%' > '+' == '-'
    */

    for (i = 0; i < n; i++)
    {
        switch (infixa[i])
        {
        case '(':
            stackPush(sope, &infixa[i]);
            break;

        case ')':
            while (*(x = stackPop(sope)) != '(')
                faz_operacao(spol, *x);
            break;

        case '+':
        case '-':
            while (!stackEmpty(sope) && *(x = stackTop(sope)) != '(')
            {
                x = stackPop(sope);
                faz_operacao(spol, *x);
            }
            stackPush(sope, &infixa[i]);
            break;

        case '*':
        case '/':
        case '%':
            while (!stackEmpty(sope) && *(x = stackTop(sope)) != '('
                                           && *x != '+' && *x != '-')
            {
                x = stackPop(sope);
                faz_operacao(spol, *x);
            }
            stackPush(sope, &infixa[i]);
            break;

        case '~':
            stackPush(sope, &infixa[i]);
            break;

        /* no default, e' uma letra que representa um polinomio */
        default:
            ind = calcula_indice(infixa[i]);
            pol = copia(var[ind]);
            stackPush(spol, pol); /* empilha na pilha de polinomios */
        }
    }

    /* desempilha todos os operadores que restaram */
    while (!stackEmpty(sope))
    {
        x = stackPop(sope);
        faz_operacao(spol, *x);
    }

    /* o polinomio resposta e' aquele que
       restou na pilha de polinomios */
    resposta = stackPop(spol);
    stackFree(sope);
    stackFree(spol);

    return resposta;
}

/*
   a funcao faz_operacao recebe uma pilha de polinomios
   e um caracter que indica qual operacao sera realizada
   (as operacoes sao aquelas implementadas na biblioteca
   polinomios); desempilha os operandos, faz o calculo e
   empilha o resultado do calculo na pilha de polinomios
*/

void faz_operacao(Stack spol, char op)
{
    polinomio p, q, r;

    /* caso a operacao seja o ~ (menos unario),
       apenas o polinomio do topo da pilha sera
       desempilhado */
    if (op == '~')
        p = stackPop(spol);
    else
    {
        q = stackPop(spol);
        p = stackPop(spol);
    }

    switch (op)
    {
    case '+':
        r = soma(p, q);
        break;
    case '-':
        r = subt(p, q);
        break;
    case '*':
        r = mult(p, q);
        break;
    case '/':
        r = quoc(p, q);
        break;
    case '%':
        r = rest(p, q);
        break;
    case '~':
        r = nega(p);
        break;
    }

    /* libera os polinomios operandos */
    libera(p);
    if (op != '~') libera(q);

    /* empilha o polinomio resultado da operacao */
    stackPush(spol, r);

    return;
}

/*
   a funcao descarta_resto_da_linha e' aquela
   definida no enunciado do ep3 e e' chamada
   apos qualquer leitura feita pelo programa
*/

void descarta_resto_da_linha()
{
    char c;

    do
    {
        scanf("%c", &c);
    }
    while (c != '\n' && c != EOF);

    return;
}

int main()
{
    int i, quit = 0;
    char pol, operacao, *expressao;
    /* O tipo polinomio e' o definido pela biblioteca polinomios */
    polinomio var[26], aux;

    expressao = (char *) mallocSafe_ep3(MAX * sizeof(char));

    /* inicializa os polinomios 'a' a 'z' */
    for (i = 0; i < 26; i++)
        var[i] = cria();

    printf("*************************\n");
    printf("Calculadora de polinomios\n");
    printf("*************************\n");

    do
    {
        printf("\n");
        printf("Digite uma expressao ou quit para sair do programa:\n");
        printf("> ");

        scanf("%c%c", &pol, &operacao);

        /* indice do polinomio que sera impresso, lido ou calculado */
        i = calcula_indice(pol);

        switch(operacao)
        {
        case '?':
            descarta_resto_da_linha();
            impr(pol, var[i]);
            break;

        case ':':
            libera(var[i]);
            var[i] = leia();
            descarta_resto_da_linha();
            impr(pol, var[i]);
            break;

        case '=':
            /* a expressao lida nao pode ter espacos */
            scanf("%s", expressao);
            descarta_resto_da_linha();
            aux = calcula_expressao(var, expressao);
            libera(var[i]);
            var[i] = aux;
            impr(pol, var[i]);
            break;

        /* caso a operacao nao for uma das 3 listadas
           acima, o programa se encerra*/
        default:
            descarta_resto_da_linha();
            quit = 1;
        }
    }
    while (!quit);

    /* libera os polinomios 'a' a 'z' */
    for (i = 0; i < 26; i++)
        libera(var[i]);

    free(expressao);

    printf("Tchau!\n");

    return 0;
}
