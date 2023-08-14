#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_SIZE 100

FILE *outputFile;

struct Stack {
    char data[MAX_SIZE];
    int top;
};

void initialize(struct Stack *stack) {
    stack->top = -1;
}

int isEmpty(struct Stack *stack) {
    return stack->top == -1;
}

void push(struct Stack *stack, char value) {
    if (stack->top == MAX_SIZE - 1) {
        printf("Erro: pilha cheia\n");
        exit(1);
    }
    stack->top++;
    stack->data[stack->top] = value;
}

char pop(struct Stack *stack) {
    if (isEmpty(stack)) {
        printf("Erro: pilha vazia\n");
        exit(1);
    }
    return stack->data[stack->top--];
}

char peek(struct Stack *stack) {
    if (isEmpty(stack)) {
        printf("Erro: pilha vazia\n");
        exit(1);
    }
    return stack->data[stack->top];
}

int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int precedence(char op) {
    if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/') {
        return 2;
    }
    return 0;
}

void infixToPostfix(char *infix, char *postfix) {
    struct Stack operatorStack;
    initialize(&operatorStack);

    int length = strlen(infix);
    int postfixIndex = 0;

    for (int i = 0; i < length; i++) {
        char c = infix[i];

        if (isdigit(c)) {
            postfix[postfixIndex++] = c;
        } else if (c == '(') {
            push(&operatorStack, c);
        } else if (c == ')') {
            while (!isEmpty(&operatorStack) && peek(&operatorStack) != '(') {
                postfix[postfixIndex++] = pop(&operatorStack);
            }
            pop(&operatorStack);
        } else if (isOperator(c)) {
            while (!isEmpty(&operatorStack) && precedence(peek(&operatorStack)) >= precedence(c)) {
                postfix[postfixIndex++] = pop(&operatorStack);
            }
            push(&operatorStack, c);
        }
    }

    while (!isEmpty(&operatorStack)) {
        postfix[postfixIndex++] = pop(&operatorStack);
    }

    postfix[postfixIndex] = '\0';
}

void generateARMCode(char *postfix) {
    // Gerar código Assembly para armazenar o resultado em um registrador e imprimir
    outputFile = fopen("output.s", "w");
    if (!outputFile) {
        printf("Erro ao criar o arquivo de saída\n");
        exit(1);
    }

    fprintf(outputFile, ".text\n.global main\nmain:\n");

    struct Stack operandStack;
    initialize(&operandStack);

    int length = strlen(postfix);
    int ARMStackLength = 0;

    for (int i = 0; i < length; i++) {
        char c = postfix[i];

        if (isdigit(c)) {
            // Converte o caractere numérico em número inteiro
            int num = c - '0';
            // Empilha o número na pilha
            push(&operandStack, num);
            ARMStackLength += 1;
            fprintf(outputFile, "\tmov r0, #%d\n", num);
            fprintf(outputFile, "\tstr r0, [sp, #-%d]!\n", ARMStackLength * 4);
        } else if (isOperator(c)) {
            // Realiza a operação usando os dois operandos do topo da pilha
            int operand2 = pop(&operandStack);
            fprintf(outputFile, "\tldr r1, [sp], #%d\n", ARMStackLength * 4);
            ARMStackLength -= 1;
            int operand1 = pop(&operandStack);
            fprintf(outputFile, "\tldr r0, [sp], #%d\n", ARMStackLength * 4);
            ARMStackLength -= 1;
            int result;

            if (c == '+') {
                result = operand1 + operand2;
                fprintf(outputFile, "\tadd r0, r0, r1\n");
            } else if (c == '-') {
                result = operand1 - operand2;
                fprintf(outputFile, "\tsub r0, r0, r1\n");
            } else if (c == '*') {
                result = operand1 * operand2;
                fprintf(outputFile, "\tmul r0, r0, r1\n");
            } else if (c == '/') {
                result = operand1 / operand2;
                fprintf(outputFile, "\tdiv r0, r0, r1\n");
            }

            // Empilha o resultado de volta na pilha
            push(&operandStack, result);
            ARMStackLength += 1;
            fprintf(outputFile, "\tstr r0, [sp, #-%d]!\n", ARMStackLength * 4);
        }
    }

    // O resultado final estará no topo da pilha
    pop(&operandStack);
    
    fprintf(outputFile, "\tldr r0, [sp], #4\n");
    fprintf(outputFile, "fim:\n");
    fprintf(outputFile, "\tb fim\n");

    fclose(outputFile);
}

int main() {
    FILE *inputFile = fopen("input.txt", "r");
    if (!inputFile) {
        printf("Erro ao abrir o arquivo de entrada\n");
        return 1;
    }

    char infixExpression[MAX_SIZE];
    fscanf(inputFile, "%s", infixExpression);

    fclose(inputFile);

    char postfixExpression[MAX_SIZE];
    infixToPostfix(infixExpression, postfixExpression);
    printf("Expressão em notação pós-fixa: %s\n", postfixExpression);

    generateARMCode(postfixExpression);

    return 0;
}
