#include "ast.h"

void simplify(Node** node_ptr) {
    Node* node = *node_ptr;
    if (!node || node->type != OPERATOR) return;

    // Рекурсивно упрощаем дочерние узлы
    simplify(&node->left);
    simplify(&node->right);
    
    // Правило 1: Свертка констант (3 - 5 -> -2)
    if (node->data.op == '-' && node->left->type == NUMBER && node->right->type == NUMBER) {
        double result = node->left->data.value - node->right->data.value;
        free_tree(node->left); // Освобождаем старые узлы
        free_tree(node->right);
        
        node->type = NUMBER;
        node->data.value = result;
        node->left = NULL;
        node->right = NULL;
        return;
    }
    
    // Правило 2: X - 0 -> X
    if (node->data.op == '-' && node->right->type == NUMBER && node->right->data.value == 0) {
        Node* left_child = node->left;
        free(node->right); // Освобождаем узел с 0
        free(node);        // Освобождаем узел '-'
        *node_ptr = left_child; // Заменяем узел '-' на его левого потомка
        simplify(node_ptr); // Продолжаем упрощение, если возможно
        return;
    }

    // Правило 3: Перегруппировка
    Node* left_child = node->left;
    Node* right_child = node->right;
    
    if (node->data.op == '-' && left_child && left_child->type == OPERATOR && left_child->data.op == '+' && right_child && right_child->type == NUMBER) {
        
        // Случай: (C1 + X) - C2  ->  (C1-C2) + X
        if (left_child->left && left_child->left->type == NUMBER) {
            double c1 = left_child->left->data.value;
            double c2 = right_child->data.value;
            
            // Заменяем узел C1 на результат (C1-C2)
            left_child->left->data.value = c1 - c2;
            
            // "Поднимаем" узел '+' на место '-'
            Node* new_root = left_child;
            *node_ptr = new_root;
            
            // Освобождаем старый узел '-' и его правого ребенка (C2)
            free(node);
            free(right_child);
            
            simplify(node_ptr); // Продолжаем упрощение с нового корня
            return;
        }

        // НОВЫЙ БЛОК: Обработка случая (X + C1) - C2  ->  X + (C1-C2)
        if (left_child->right && left_child->right->type == NUMBER) {
            double c1 = left_child->right->data.value;
            double c2 = right_child->data.value;
            
            // Вычисляем новую константу
            double new_const = c1 - c2;

            // Перестраиваем дерево:
            // 1. Освобождаем старый правый узел (C2)
            free(right_child);
            // 2. Освобождаем старый узел с константой C1
            free(node->left->right);

            // 3. Создаем новый узел с результатом (C1-C2) и ставим его на место C1
            node->left->right = create_number_node(new_const);
            
            // 4. Заменяем узел '-' на узел '+' (поднимаем его)
            Node* new_root = node->left;
            *node_ptr = new_root;
            
            // 5. Освобождаем старый узел '-'
            free(node);

            simplify(node_ptr); // Продолжаем упрощение
            return;
        }
    }
}