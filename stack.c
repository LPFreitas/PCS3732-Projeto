
/* STACK.c: IMPLEMENTACAO DA PILHA */

#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

/*
 * PILHA: uma implementacao com lista encadeada sem cabeca
 */
typedef struct stacknode *Link;
struct stacknode
{
    Item conteudo;
    Link prox;
};

struct stack
{              /* aqui esta especificado o que e' */
    Link topo; /* uma pilha: um apontador para stacknode */
};
typedef struct stack *TStack;

static void *mallocSafe(unsigned int n);

Stack stackInit(int n)
{
    TStack s = (TStack)mallocSafe(sizeof *s);
    s->topo = NULL;
    return (Stack)s;
}

int stackEmpty(Stack s)
{
    return ((TStack)s)->topo == NULL;
}

void stackPush(Stack s, Item conteudo)
{
    Link p = (Link)mallocSafe(sizeof *p);
    p->conteudo = conteudo;
    p->prox = ((TStack)s)->topo;
    ((TStack)s)->topo = p;
}

Item stackPop(Stack s)
{
    Link p = ((TStack)s)->topo;
    Item conteudo;

    if (p == NULL)
    { /* stackempty() */
        fprintf(stderr, "Putz, voce nao sabe o que esta fazendo!\n");
        exit(-1);
    }
    /* tudo bem, a pilha nao esta vazia... */
    conteudo = p->conteudo;
    ((TStack)s)->topo = p->prox;
    free(p);
    return conteudo;
}

Item stackTop(Stack s)
{
    if (((TStack)s)->topo == NULL)
    { /* stackempty() */
        fprintf(stderr, "Putz, voce nao sabe o que esta fazendo!\n");
        exit(-1);
    }

    /* tudo bem, a pilha nao esta vazia... */
    return ((TStack)s)->topo->conteudo;
}

void stackFree(Stack s)
{
    while (((TStack)s)->topo != NULL)
    {
        Link p = ((TStack)s)->topo;
        ((TStack)s)->topo = ((TStack)s)->topo->prox;
        free(p);
    }
    free(s);
}

void stackDump(Stack s)
{
    Link p = ((TStack)s)->topo;
    fprintf(stdout, "pilha:");
    if (p == NULL)
        fprintf(stdout, "vazia.");
    while (p != NULL)
    {
        fprintf(stdout, " %c", *(char *)(p->conteudo));
        p = p->prox;
    }
    fprintf(stdout, "\n");
}

static void *mallocSafe(unsigned int n)
{
    void *p;

    p = malloc(n);
    if (p == NULL)
    {
        fprintf(stderr, "stack: malloc de %u bytes falhou.\n", n);
        exit(-1);
    }
    return p;
}