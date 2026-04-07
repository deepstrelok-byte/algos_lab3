#include "ast.h"
#include "parser.c"
#include "simplifier.c"

int tests_passed = 0;
int tests_failed = 0;

void run_test(const char* test_name, const char* input, const char* expected) {
    Node* tree = parse(input);
    simplify(&tree);

    char result_buffer[256];
    node_to_string(tree, result_buffer, sizeof(result_buffer));
    
    if (strcmp(result_buffer, expected) == 0) {
        printf("[PASS] %s\n", test_name);
        tests_passed++;
    } else {
        printf("[FAIL] %s: Expected '%s', but got '%s'\n", test_name, expected, result_buffer);
        tests_failed++;
    }
    
    free_tree(tree);
}

int main() {
    printf("Running tests...\n");

    // 1. Простая свертка констант
    run_test("Constant Folding", "3 - 5", "-2");

    // 2. Пример из задания (перегруппировка)
    run_test("Task Example Reordering", "5 + b - 3", "(2 + b)");
    
    // 3. Отсутствие упрощения, если не применимо
    run_test("No Simplification", "a - b", "(a - b)");
    
    // 4. Упрощение 
    run_test("Subtraction with Zero", "a - 0", "a"); 
    
    // 5. Более сложная цепочка
    run_test("Complex Chain", "(x + 10) - 2", "(x + 8)");

    printf("\nTests finished: %d passed, %d failed.\n", tests_passed, tests_failed);
    
    return tests_failed > 0; // Вернуть 1 если были ошибки
}