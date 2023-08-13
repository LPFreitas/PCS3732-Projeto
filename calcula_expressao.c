/*
 *  polonesa.c: converte e calcula
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "item.h"
#include "stackS.h"

typedef enum
{
    FALSE,
    TRUE
} Boolean;

int mostreItem; /* indica se o item sendo examinado deve ser exibido */

int mostrePilha; /* indica se o conteudo da pilha deve ser exibido
                  * depois de um item ser examinado
                  */

char *nomeProg; /* nome do programa */

void mostreUso(char *);
void *mallocSafe(unsigned int);
void fazOperacao(Stack, char);
float calculaExpressao(char *);

float Valor(char ch)
{ /* o valor da letra maiuscula e' a posição da letra no alfabeto */
    return ch - 'A' + 1;
}

/*
 * M A I N
 */
int main(int argc, char *argv[])
{
    nomeProg = argv[0];

    if (argc == 1)
    {
        mostreUso(nomeProg);
        return 0;
    }

    /*
     * Valores "default"
     */
    mostreItem = FALSE;
    mostrePilha = FALSE;

    /* examina os argumentos na linha de comando */
    while (--argc > 1)
    {
        if (strncmp(argv[argc], "-i", 2) == 0)
            mostreItem = TRUE;
        else if (strncmp(argv[argc], "-s", 2) == 0)
            mostrePilha = TRUE;
        else
        {
            mostreUso(nomeProg);
            return EXIT_FAILURE;
        }
    }

    printf("valor: %.2f\n", calculaExpressao(argv[1]));

    return 0;
}

/*
 * para_polonesa: recebe uma expressão infixa apontada por `infixa' e
 *         devolve o valor da expressão, considerando que as letras
 *         maiusculas valem sua posição no alfabeto.
 *
 * ATENCAO: a funcao supoe que a expressao infixa esta sintaticamente correta.
 *          Se a expressao for, digamos, "A+B)", ocorrera' "segmentation fault".
 */
float calculaExpressao(char *infixa)
{
    int n = strlen(infixa); /* comprimento da expressao infixa */
    int i;                  /* percorre a expressao infixa */
    Stack sope;             /* pilha de operadores */
    Stack sval;             /* pilha de valores */
    char *item;
    float *v, res;

    sope = stackInit(n); /* inicializa a pilha de operadores */
    sval = stackInit(n); /* inicializa a pilha de valores    */

    /* examina cada item da expressao infixa */
    for (i = 0; i < n; i++)
    {
        if (mostreItem)
            fprintf(stdout, "item: '%c'\n", infixa[i]);

        switch (infixa[i])
        {
        case '(':
            stackPush(sope, &infixa[i]); /* empilha o '(' */
            break;

        case ')': /* desempilha até o '(' */
            while (*(item = stackPop(sope)) != '(')
                fazOperacao(sval, *item);
            break;

        case '+':
        case '-': /* '+' e '-' tem precedencia menor que '*' e '/' */
            while (!stackEmpty(sope) && *(item = stackTop(sope)) != '(')
            {
                item = stackPop(sope);
                fazOperacao(sval, *item);
            }
            stackPush(sope, &infixa[i]);
            break;

        case '*':
        case '/': /* '*' e '/' tem precedencia maior que '+' e '-' */
            while (!stackEmpty(sope) && *(item = stackTop(sope)) != '(' && *item != '+' && *item != '-')
            {
                item = stackPop(sope);
                fazOperacao(sval, *item);
            }
            stackPush(sope, &infixa[i]);
            break;

        default:
            if (infixa[i] != ' ')
            {
                v = mallocSafe(sizeof(float));
                *v = Valor(infixa[i]);
                stackPush(sval, v); /* empilha na pilha de valores */
            }
        }

        if (mostrePilha)
            stackDump(sope);
    }

    /* desempilha todos os operadores que restaram */
    while (!stackEmpty(sope))
    {
        item = stackPop(sope);
        fazOperacao(sval, *item);
    }

    v = stackPop(sval);
    res = *v;
    free(v);
    stackFree(sope);
    stackFree(sval);
    return res;
}

/*
 * mostra a maneira de usar programa
 */
void mostreUso(char *nomeProg)
{
    fprintf(stdout, "Uso: %s \"<expressao infixa>\" [-i] [-s]\n"
                    " -i mostra o item sendo examinado\n"
                    " -s mostra o conteudo da pilha\n",
            nomeProg);
}

void *mallocSafe(unsigned int n)
{
    void *p;

    p = malloc(n);
    if (p == NULL)
    {
        fprintf(stderr, "%s, malloc de %u bytes falhou.\n", nomeProg, n);
        exit(-1);
    }
    return p;
}

void fazOperacao(Stack sval, char op)
{
    float *v, *v1, *v2;

    v2 = stackPop(sval);
    v1 = stackPop(sval);
    v = mallocSafe(sizeof(float));
    switch (op)
    {
    case '+':
        *v = *v1 + *v2;
        break;
    case '-':
        *v = *v1 - *v2;
        break;
    case '*':
        *v = *v1 * *v2;
        break;
    case '/':
        *v = *v1 / *v2;
        break;
    }
    stackPush(sval, v);
    free(v1);
    free(v2);
}