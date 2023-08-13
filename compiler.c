#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "item.h"
#include "stack.h"

FILE *outputFile;
int mostrePilha = 1;

void *mallocSafe(unsigned int);
void fazOperacao(Stack, char);
float calculaExpressao(char *);

/* Função para leitura de dígitos e construção de um número */
int getNum(FILE *file)
{
    int num = 0;
    char c = fgetc(file);
    while (isdigit(c))
    {
        num = num * 10 + (c - '0');
        c = fgetc(file);
    }
    ungetc(c, file); /* Coloca o último caractere lido de volta no buffer */
    return num;
}

/* Função para gerar código Assembly ARMv7 para uma operação binária */
void genBinaryOp(const char *op, const char *destReg, const char *srcReg1, const char *srcReg2)
{
    fprintf(outputFile, "\t%s %s, %s, %s\n", op, destReg, srcReg1, srcReg2);
}

/* Função para gerar código Assembly ARMv7 para carregar um valor imediato em um registrador */
void genLoadImmediate(const char *reg, int value)
{
    fprintf(outputFile, "\tmov %s, #%d\n", reg, value);
}

/* Função para gerar código Assembly ARMv7 para expressões aritméticas */
void genARMCode(FILE *file)
{
    int num = getNum(file);
    fprintf(outputFile, "\tmov r0, #%d\n", num);

    char c = fgetc(file);
    while (c != EOF)
    {
        char op = c;
        int num = getNum(file);

        /* Carregar o próximo número para o registrador r1 */
        genLoadImmediate("r1", num);

        /* Gerar código Assembly para a operação */
        if (op == '+')
        {
            genBinaryOp("add", "r0", "r0", "r1");
        }
        else if (op == '-')
        {
            genBinaryOp("sub", "r0", "r0", "r1");
        }
        else if (op == '*')
        {
            genBinaryOp("mul", "r0", "r0", "r1");
        }
        else if (op == '/')
        {
            genBinaryOp("sdiv", "r0", "r0", "r1");
        }

        c = fgetc(file);
    }
}

float Valor(char ch)
{ /* o valor da letra maiuscula e' a posição da letra no alfabeto */
    return ch - 'A' + 1;
}

void fazOperacao(Stack s_val, char op)
{
    float *v, *v1, *v2;

    v2 = stackPop(s_val);
    v1 = stackPop(s_val);
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

    /* Carregar o próximo número para o registrador r1 */
    /* genLoadImmediate("r1", v1);
    genLoadImmediate("r1", v2); */

    /* Gerar código Assembly para a operação */
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

    stackPush(s_val, v);
    free(v1);
    free(v2);
}

float calculaExpressao(char *infixa)
{
    int n = strlen(infixa); /* comprimento da expressao infixa */
    int i;                  /* percorre a expressao infixa */
    Stack s_op;             /* pilha de operadores */
    Stack s_val;            /* pilha de valores */
    char *item;
    float *v, res;

    s_op = stackInit(n);  /* inicializa a pilha de operadores */
    s_val = stackInit(n); /* inicializa a pilha de valores    */

    /* examina cada item da expressao infixa */
    for (i = 0; i < n; i++)
    {
        /*
        if (mostreItem)
            fprintf(stdout, "item: '%c'\n", infixa[i]);
        */

        switch (infixa[i])
        {
        case '(':
            stackPush(s_op, &infixa[i]); /* empilha o '(' */
            break;

        case ')': /* desempilha até o '(' */
            while (*(item = stackPop(s_op)) != '(')
                fazOperacao(s_val, *item);
            break;

        case '+':
        case '-': /* '+' e '-' tem precedencia menor que '*' e '/' */
            while (!stackEmpty(s_op) && *(item = stackTop(s_op)) != '(')
            {
                item = stackPop(s_op);
                fazOperacao(s_val, *item);
            }
            stackPush(s_op, &infixa[i]);
            break;

        case '*':
        case '/': /* '*' e '/' tem precedencia maior que '+' e '-' */
            while (!stackEmpty(s_op) && *(item = stackTop(s_op)) != '(' && *item != '+' && *item != '-')
            {
                item = stackPop(s_op);
                fazOperacao(s_val, *item);
            }
            stackPush(s_op, &infixa[i]);
            break;

        default:
            if (infixa[i] != ' ')
            {
                v = mallocSafe(sizeof(float));
                *v = Valor(infixa[i]);
                stackPush(s_val, v); /* empilha na pilha de valores */
            }
        }

        if (mostrePilha)
            stackDump(s_op);
    }

    /* desempilha todos os operadores que restaram */
    while (!stackEmpty(s_op))
    {
        item = stackPop(s_op);
        fazOperacao(s_val, *item);
    }

    v = stackPop(s_val);
    res = *v;
    free(v);
    stackFree(s_op);
    stackFree(s_val);
    return res;
}

void *mallocSafe(unsigned int n)
{
    void *p;

    p = malloc(n);
    if (p == NULL)
    {
        /* fprintf(stderr, "%s, malloc de %u bytes falhou.\n", nomeProg, n); */
        exit(-1);
    }
    return p;
}

int main()
{
    FILE *inputFile = fopen("input.txt", "r");
    if (!inputFile)
    {
        printf("Erro ao abrir o arquivo de entrada\n");
        return 1;
    }

    /*
    outputFile = fopen("output.s", "w");
    if (!outputFile)
    {
        printf("Erro ao criar o arquivo de saída\n");
        return 1;
    }

    fprintf(outputFile, ".text\n.global main\nmain:\n");

    genARMCode(inputFile);

    fprintf(outputFile, "end:\n\tb end\n");
    */

    char *infixa;
    int i = 0;

    int num = getNum(inputFile);
    infixa[i] = num;
    i += 1;

    char c = fgetc(inputFile);
    while (c != EOF)
    {
        int num = getNum(inputFile);
        infixa[i] = c;
        infixa[i + 1] = num;
        i += 2;
        c = fgetc(inputFile);
    }

    float res = calculaExpressao(infixa);
    printf("Resposta = %f\n", res);

    fclose(inputFile);
    /* fclose(outputFile); */

    return 0;
}

/*
Função para empilhar um caractere na pilha
void push(Stack *stack, char c)
{
    if (stack->top < MAX_EXPRESSION_SIZE - 1)
    {
        stack->top++;
        stack->data[stack->top] = c;
    }
}

Função para desempilhar um caractere da pilha
char pop(Stack *stack)
{
    if (stack->top >= 0)
    {
        char c = stack->data[stack->top];
        stack->top--;
        return c;
    }
    return '\0';
}

Função que verifica se um caractere é um operador
int is_operator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}
*/