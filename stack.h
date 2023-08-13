/*
 * stack.h
 * INTERFACE: funcoes para manipular a pilha
 * ATENCAO: Esta interface permite que varias pilhas sejam
 *          utilizadas simultaneamente.
 */

#include "item.h"

typedef void *Stack;

Stack stackInit(int);
int stackEmpty(Stack);
void stackPush(Stack, Item);
Item stackPop(Stack);
Item stackTop(Stack);
void stackFree(Stack);
void stackDump(Stack);
