#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED, // 기호
    TK_NUM, // 정수 토큰
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

Token *token;

void error(char *fmt, ...);
bool consume(char op);
void expect(char op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);

int main(int argc, char **argv) {
    if (argc !=2 ){
        fprintf(stderr, "인수 개수가 올바르지 않습니다\n");
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')){
            printf("  add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("  sub rax, %d \n", expect_number());
    }

    printf("  ret\n");
    return 0;
}

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt ,ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

void expect(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error("'%c'가 아닙니다", op);
    token = token->next;
}

int expect_number(){
    if (token->kind != TK_NUM)
        error("숫자가 아닙니다");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)){
            p++;
            continue;
        }

        if (*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        error("토큰화할 수 없습니다");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}