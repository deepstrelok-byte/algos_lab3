#pragma once
#include <stdlib.h> // для malloc, free, strtod
#include <string.h> // для strcpy, strcmp
#include <stdio.h>  // для printf, snprintf
#include <ctype.h>  // для isdigit, isalpha, isspace

// Типы узлов в нашем дереве
typedef enum {
    NUMBER,
    VARIABLE,
    OPERATOR
} NodeType;

// Структура узла дерева
typedef struct Node {
    NodeType type;
    struct Node *left;
    struct Node *right;
    union {
        double value;   // для NUMBER
        char* name;     // для VARIABLE
        char op;        // для OPERATOR
    } data;
} Node;

// Функции для создания узлов (безопаснее, чем делать это вручную)
Node* create_number_node(double value);
Node* create_variable_node(const char* name);
Node* create_operator_node(char op, Node* left, Node* right);

// Функция для рекурсивного освобождения памяти дерева
void free_tree(Node* node);

// Функция для преобразования дерева обратно в строку
void node_to_string(Node* node, char* buffer, int buffer_size);

// Функция для преобразования дерева обратно в строку
void node_to_string(Node* node, char* buffer, int buffer_size);

// НОВАЯ ФУНКЦИЯ: для псевдографической печати дерева
void print_tree(Node* node, const char* prefix, int is_left);