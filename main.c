#include "ast.h"
#include "parser.c"   // Просто подключаем .c файлы для простоты сборки
#include "simplifier.c"

int main() {
    char buffer[256];
    char result_str[256];

    printf("Enter expression (or 'exit' to quit):\n> ");
    while (fgets(buffer, sizeof(buffer), stdin) && strncmp(buffer, "exit", 4) != 0) {
        // Убираем символ новой строки
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strlen(buffer) == 0) {
            printf("> ");
            continue;
        }

        printf("\n--- Original ---\n");
        printf("Expression: %s\n", buffer);
        
        Node* tree = parse(buffer);
        
        printf("Tree view:\n");
        print_tree(tree, "", 1); // Выводим исходное дерево

        printf("\n--- Simplified ---\n");
        simplify(&tree);
        
        node_to_string(tree, result_str, sizeof(result_str));
        printf("Expression: %s\n", result_str);
        
        printf("Tree view:\n");
        print_tree(tree, "", 1); // Выводим упрощенное дерево
        
        free_tree(tree);
        printf("\n> ");
    }

    return 0;
}   