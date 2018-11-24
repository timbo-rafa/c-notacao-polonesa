#include <stdio.h>
#include <stdlib.h>

typedef enum {FALSE = 0, TRUE = 1} boolean;
typedef enum {FOLHA, NO} tipoNo;
typedef enum {INVALIDO, PILHA_VAZIA, OPERADOR, OPERANDO, FIM} tipoResultado;

struct noArvore
{
    struct
    {
        tipoNo tipo;
        union
        {
            int valor;  // Folha
            struct
            {
                struct noArvore *esquerdo;
                char operador;
                struct noArvore *direito;
            } info;
        };
    } no;
    struct noArvore *prox;
};

void infixo(struct noArvore *p);

tipoResultado obterSimbolo(struct noArvore **p)
{
    char c;
    int valor = 0;
    // remover brancos
    do
    {
        c = getc(stdin);
    }
    while (c == ' ' || c == '\t' || c == '\n');
    // obter simbolo
    if (c >= '0' && c <= '9')
    {
        do
        {
            valor = valor * 10 + (int)c - (int)'0';
            c = getc(stdin);
        } while (c >= '0' && c <= '9');
        ungetc(c, stdin);
        *p = (struct noArvore *)malloc(sizeof(struct noArvore));
        (*p)->no.tipo = FOLHA;
        (*p)->no.valor = valor;
        return OPERANDO;
    }
    else
    {
        if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            *p = (struct noArvore *)malloc(sizeof(struct noArvore));
            (*p)->prox = NULL;
            (*p)->no.tipo = NO;
            (*p)->no.info.esquerdo = NULL;
            (*p)->no.info.operador = c;
            (*p)->no.info.direito = NULL;
            return OPERADOR;
        }
        else
        {
            if (c == '#') return FIM;
            else return INVALIDO;
        }
    }
}

int vazia(struct noArvore *pilha)
{
    return pilha == NULL;
}

void empilhar(struct noArvore **pilha, struct noArvore *p)
{
    p->prox = *pilha;
    *pilha = p;
}

boolean desempilhar(struct noArvore **pilha, struct noArvore **p)
{
    if (vazia(*pilha))
    {
        *p = NULL;
        return FALSE;
    }
    else
    {
        *p = *pilha;
        *pilha = (*pilha)->prox;
        return TRUE;
    }
}

void liberarArvore(struct noArvore **p)
{
    if (*p != NULL)
    {
        if ((*p)->no.tipo == NO)
        {
            liberarArvore(&((*p)->no.info.esquerdo));
            liberarArvore(&((*p)->no.info.direito));
        }
        free(*p);
        *p = NULL;
    }
}

void liberarPilha(struct noArvore **pilha)
{
    struct noArvore *p;
    while (*pilha != NULL)
    {
        p = *pilha;
        *pilha = (*pilha)->prox;
        liberarArvore(&p);
    }
}

tipoResultado processarSufixo(struct noArvore **raiz)
{
    struct noArvore *pilha = NULL, *p;
    tipoResultado tipo = obterSimbolo(&p);
    while (tipo != INVALIDO && tipo != FIM && tipo != PILHA_VAZIA)
    {
        switch (p->no.tipo)
        {
        case NO:
            if (desempilhar(&pilha, &(p->no.info.direito)))
                if (desempilhar(&pilha, &(p->no.info.esquerdo)))
                    empilhar(&pilha, p);
                else tipo = PILHA_VAZIA;
            else tipo = PILHA_VAZIA;
            break;
        case FOLHA:
            empilhar(&pilha, p);
            break;
        }
        if (tipo == PILHA_VAZIA) liberarArvore(&p);  // p "fora" da pilha
        else tipo = obterSimbolo(&p);
    }
    *raiz = p;
    return tipo;
}

void apresentarInfixo(struct noArvore *p)
{
    if (p != NULL)
    {
        if (p->no.tipo == FOLHA) printf("%d", p->no.valor);
        else
        {
            printf("(");
            apresentarInfixo(p->no.info.esquerdo);
            printf(" %c ", p->no.info.operador);
            apresentarInfixo(p->no.info.direito);
            printf(")");
        }
    }
}

int main(void)
{
    tipoResultado tipo;
    struct noArvore *pilha = NULL;
    if ((tipo = processarSufixo(&pilha)) == FIM)
        if (pilha == NULL) printf("Expressao vazia\n");
        else
            if(pilha->prox == NULL)
            {
                apresentarInfixo(pilha);
                putchar('\n');
            }
            else printf("Expressao incompleta\n");
    else
        if (tipo == PILHA_VAZIA) printf("Expressao incompleta\n");
        else printf("Simbolo invalido\n");
    liberarPilha(&pilha); // liberar (sub)arvore(s) na pilha (lista ligada)
    return 0;
}
