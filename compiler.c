#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_SIZE 100

FILE *outputFile;

struct OperatorStack
{
    char data[MAX_SIZE];
    int top;
};

struct OperandStack
{
    int data[MAX_SIZE];
    int top;
};

void initializeOperatorStack(struct OperatorStack *stack)
{
    stack->top = -1;
}

int isEmptyOperatorStack(struct OperatorStack *stack)
{
    return stack->top == -1;
}

void pushOperatorStack(struct OperatorStack *stack, char value)
{
    if (stack->top == MAX_SIZE - 1)
    {
        printf("Erro: pilha cheia\n");
        exit(1);
    }
    stack->top++;
    stack->data[stack->top] = value;
}

char popOperatorStack(struct OperatorStack *stack)
{
    if (isEmptyOperatorStack(stack))
    {
        printf("Erro: pilha vazia\n");
        exit(1);
    }
    return stack->data[stack->top--];
}

char peekOperatorStack(struct OperatorStack *stack)
{
    if (isEmptyOperatorStack(stack))
    {
        printf("Erro: pilha vazia\n");
        exit(1);
    }
    return stack->data[stack->top];
}

void initializeOperandStack(struct OperandStack *stack)
{
    stack->top = -1;
}

int isEmptyOperandStack(struct OperandStack *stack)
{
    return stack->top == -1;
}

void pushOperandStack(struct OperandStack *stack, int value)
{
    if (stack->top == MAX_SIZE - 1)
    {
        printf("Erro: pilha cheia\n");
        exit(1);
    }
    stack->top++;
    stack->data[stack->top] = value;
}

int popOperandStack(struct OperandStack *stack)
{
    if (isEmptyOperandStack(stack))
    {
        printf("Erro: pilha vazia\n");
        exit(1);
    }
    return stack->data[stack->top--];
}

void printOperandStack(struct OperandStack *stack)
{
    int i = 0;
    printf("PILHA:\n");
    while (i <= stack->top)
        printf("%d", stack->data[i++]);
    printf("\n\n");
}

int isOperator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int precedence(char op)
{
    if (op == '+' || op == '-')
        return 1;
    else if (op == '*' || op == '/')
        return 2;

    return 0;
}

void infixToPostfix(char *infix, char *postfix)
{
    struct OperatorStack operatorStack;
    initializeOperatorStack(&operatorStack);

    int length = strlen(infix);
    int postfixIndex = 0;

    for (int i = 0; i < length; i++)
    {
        char c = infix[i];

        if (isspace(c))
            continue; // Ignorar espaços em branco
        else if (isdigit(c))
        {
            // Para números com mais de um algarismo
            while (i < length && (isdigit(infix[i])))
                postfix[postfixIndex++] = infix[i++];

            postfix[postfixIndex++] = ' ';
            i--; // Decrementar i pois o loop principal também incrementa
        }
        else if (c == '-' && isdigit(infix[i + 1]))
        {
            postfix[postfixIndex++] = infix[i++];
            while (i < length && (isdigit(infix[i])))
                postfix[postfixIndex++] = infix[i++];

            postfix[postfixIndex++] = ' ';
            i--; // Decrementar i pois o loop principal também incrementa
        }
        else if (c == '(')
            pushOperatorStack(&operatorStack, c);
        else if (c == ')')
        {
            while (!isEmptyOperatorStack(&operatorStack) && peekOperatorStack(&operatorStack) != '(')
            {
                postfix[postfixIndex++] = popOperatorStack(&operatorStack);
                postfix[postfixIndex++] = ' ';
            }

            popOperatorStack(&operatorStack);
        }
        else if (isOperator(c))
        {
            while (!isEmptyOperatorStack(&operatorStack) && precedence(peekOperatorStack(&operatorStack)) >= precedence(c))
            {
                postfix[postfixIndex++] = popOperatorStack(&operatorStack);
                postfix[postfixIndex++] = ' ';
            }

            pushOperatorStack(&operatorStack, c);
        }
    }

    while (!isEmptyOperatorStack(&operatorStack))
    {
        postfix[postfixIndex++] = popOperatorStack(&operatorStack);
        postfix[postfixIndex++] = ' ';
    }

    postfixIndex--; // Decrementar postfixIndex para sobrescrever espaço a mais
    postfix[postfixIndex] = '\0';
}

void generateARMCode(char *postfix)
{
    // Gerar código Assembly para armazenar o resultado em um registrador e imprimir
    outputFile = fopen("output.s", "w");
    if (!outputFile)
    {
        printf("Erro ao criar o arquivo de saída\n");
        exit(1);
    }

    fprintf(outputFile, ".text\n.global main\nmain:\n");
    fprintf(outputFile, "\tldr sp, =inicio_stack\n");

    struct OperandStack operandStack;
    initializeOperandStack(&operandStack);

    int length = strlen(postfix);

    for (int i = 0; i < length; i++)
    {
        char c = postfix[i];

        if (isdigit(c))
        {
            int num = 0;
            while (i < length && (isdigit(postfix[i])))
            {
                num = num * 10 + (postfix[i] - '0');
                i++;
            }
            i--; // Decrementar i pois o loop principal também incrementa

            pushOperandStack(&operandStack, num);
            fprintf(outputFile, "\tmov r0, #%d\n", num);
            fprintf(outputFile, "\tpush {r0}\n");
        }
        else if (c == '-' && isdigit(postfix[i + 1]))
        {
            int num = 0;
            i++;
            while (i < length && (isdigit(postfix[i])))
            {
                num = num * 10 + (postfix[i] - '0');
                i++;
            }
            num = -num;
            i--; // Decrementar i pois o loop principal também incrementa

            pushOperandStack(&operandStack, num);
            fprintf(outputFile, "\tmov r0, #%d\n", num);
            fprintf(outputFile, "\tpush {r0}\n");
        }
        else if (isOperator(c))
        {
            // Realiza a operação usando os dois operandos do topo da pilha
            int operand2 = popOperandStack(&operandStack);
            fprintf(outputFile, "\tpop {r1}\n");
            int operand1 = popOperandStack(&operandStack);
            fprintf(outputFile, "\tpop {r0}\n");
            int result;

            if (c == '+')
            {
                result = operand1 + operand2;
                fprintf(outputFile, "\tadd r0, r0, r1\n");
            }
            else if (c == '-')
            {
                result = operand1 - operand2;
                fprintf(outputFile, "\tsub r0, r0, r1\n");
            }
            else if (c == '*')
            {
                result = operand1 * operand2;
                fprintf(outputFile, "\tmul r0, r1, r0\n");
            }
            else if (c == '/')
            {
                result = operand1 / operand2;
                fprintf(outputFile, "\tsdiv r0, r0, r1\n");
            }

            printf("> Operação %d %c %d = %d\n", operand1, c, operand2, result);

            // Empilha o resultado de volta na pilha
            pushOperandStack(&operandStack, result);
            fprintf(outputFile, "\tpush {r0}\n");
        }
    }

    // O resultado final estará no topo da pilha
    printf("Resultado: %d\n", popOperandStack(&operandStack));

    fprintf(outputFile, "\tpop {r0}\n");
    fprintf(outputFile, "end:\n");
    fprintf(outputFile, "\tb end\n");

    fclose(outputFile);
}

int main()
{
    FILE *inputFile = fopen("input.txt", "r");
    if (!inputFile)
    {
        printf("Erro ao abrir o arquivo de entrada\n");
        return 1;
    }

    char infixExpression[MAX_SIZE];
    fgets(infixExpression, MAX_SIZE, inputFile);

    fclose(inputFile);

    char postfixExpression[MAX_SIZE];
    infixToPostfix(infixExpression, postfixExpression);
    printf("Expressão em notação pós-fixa: %s\n", postfixExpression);

    generateARMCode(postfixExpression);

    return 0;
}
