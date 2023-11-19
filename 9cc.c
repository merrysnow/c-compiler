#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED, // 기호
    TK_NUM, // 정수 토큰
    TK_EOF, // 입력 끝을 표시하는 토큰
} TokenKind;

typedef struct Token{
    TokenKind kind; // 토큰형
    struct Token *next; // 다음 입력 토큰
    int val; // kind가 TK_NUM의 경우, 그 수치
    char *str; // 토큰 문자열
}Token;

// 현재 가리키고 있는 토큰
Token *token;

void error_at(char *loc, char *fmt, ...);
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

char * user_input;

void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
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
        error_at(token->str, "'%c'가 아닙니다", op);
    token = token->next;
}

int expect_number(){
    if (token->kind != TK_NUM)
        error_at(token->str,"숫자가 아닙니다");
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

    // 공백 문자 스킵
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
        error_at(token->str,"토큰화할 수 없습니다");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}