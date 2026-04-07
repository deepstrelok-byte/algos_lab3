#include "ast.h"

// --- Реализация функций из ast.h ---

Node* create_number_node(double value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = NUMBER;
    node->data.value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* create_variable_node(const char* name) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = VARIABLE;
    node->data.name = (char*)malloc(strlen(name) + 1);
    strcpy(node->data.name, name);
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* create_operator_node(char op, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = OPERATOR;
    node->data.op = op;
    node->left = left;
    node->right = right;
    return node;
}

void free_tree(Node* node) {
    if (!node) return;
    if (node->type == VARIABLE) {
        free(node->data.name);
    }
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

void node_to_string(Node* node, char* buffer, int buffer_size) {
    if (!node) return;
    char left_str[128] = {0};
    char right_str[128] = {0};

    switch (node->type) {
        case NUMBER:
            snprintf(buffer, buffer_size, "%.2f", node->data.value);
            // Убираем лишние ".00"
            char* p = strstr(buffer, ".00");
            if (p && strlen(p) == 3) *p = '\0';
            break;
        case VARIABLE:
            snprintf(buffer, buffer_size, "%s", node->data.name);
            break;
        case OPERATOR:
            node_to_string(node->left, left_str, sizeof(left_str));
            node_to_string(node->right, right_str, sizeof(right_str));
            // Красивый вывод для "a + -2" -> "a - 2"
            if (node->data.op == '+' && right_str[0] == '-') {
                snprintf(buffer, buffer_size, "(%s - %s)", left_str, right_str + 1);
            } else {
                snprintf(buffer, buffer_size, "(%s %c %s)", left_str, node->data.op, right_str);
            }
            break;
    }
}

// --- Парсер (Shunting-yard) ---

// Вспомогательные стеки
#define STACK_SIZE 100
Node* value_stack[STACK_SIZE];
int value_top = -1;
char op_stack[STACK_SIZE];
int op_top = -1;

void push_value(Node* node) { value_stack[++value_top] = node; }
Node* pop_value() { return value_stack[value_top--]; }
void push_op(char op) { op_stack[++op_top] = op; }
char pop_op() { return op_stack[op_top--]; }
char top_op() { return op_stack[op_top]; }

int get_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

void apply_op() {
    char op = pop_op();
    Node* right = pop_value();
    Node* left = pop_value();
    push_value(create_operator_node(op, left, right));
}

// Основная функция парсинга
Node* parse(const char* expression) {
    value_top = -1;
    op_top = -1;
    int i = 0;
    while (expression[i] != '\0') {
        if (isspace(expression[i])) {
            i++;
            continue;
        }

        if (isdigit(expression[i]) || (expression[i] == '-' && (i == 0 || expression[i-1] == '('))) {
            char* end;
            double val = strtod(&expression[i], &end);
            push_value(create_number_node(val));
            i = end - expression;
        } else if (isalpha(expression[i])) {
            char buffer[32];
            int j = 0;
            while (isalnum(expression[i])) {
                buffer[j++] = expression[i++];
            }
            buffer[j] = '\0';
            push_value(create_variable_node(buffer));
        } else if (expression[i] == '(') {
            push_op('(');
            i++;
        } else if (expression[i] == ')') {
            while (op_top != -1 && top_op() != '(') {
                apply_op();
            }
            pop_op(); // Убираем '('
            i++;
        } else { // Оператор
            // ---------- ИСПРАВЛЕННАЯ СТРОКА ----------
            while (op_top != -1 && top_op() != '(' && get_precedence(top_op()) >= get_precedence(expression[i])) {
                apply_op();
            }
            push_op(expression[i]);
            i++;
        }
    }

    while (op_top != -1) {
        apply_op();
    }
    
    return pop_value();
}


// Рекурсивно печатает дерево в виде псевдографики
void print_tree(Node* node, const char* prefix, int is_left) {
    if (node == NULL) {
        return;
    }

    char new_prefix[256];
    
    // Печатаем правое поддерево
    strcpy(new_prefix, prefix);
    strcat(new_prefix, is_left ? "│   " : "    ");
    print_tree(node->right, new_prefix, 0);

    // Печатаем текущий узел
    printf("%s", prefix);
    printf(is_left ? "└──L:" : "┌──R:");

    switch (node->type) {
        case NUMBER:
            // Убираем .00 для целых
            if (node->data.value == (int)node->data.value) {
                printf("Num(%d)\n", (int)node->data.value);
            } else {
                printf("Num(%.2f)\n", node->data.value);
            }
            break;
        case VARIABLE:
            printf("Var(%s)\n", node->data.name);
            break;
        case OPERATOR:
            printf("Op(%c)\n", node->data.op);
            break;
    }

    // Печатаем левое поддерево
    strcpy(new_prefix, prefix);
    strcat(new_prefix, is_left ? "    " : "│   ");
    print_tree(node->left, new_prefix, 1);
}