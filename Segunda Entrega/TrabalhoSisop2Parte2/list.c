#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char *string;
    struct node *next;
} Node;

typedef struct list {
    Node *head;
    Node *tail;
} List;

// Função para criar um novo nó
Node *createNode(char *str) {
    Node *newNode = (Node*) malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Erro: Memória insuficiente!\n");
        exit(1);
    }
    newNode->string = (char*) malloc(strlen(str) + 1);
    strcpy(newNode->string, str);
    newNode->next = NULL;
    return newNode;
}

// Função para inserir um novo nó no final da lista
void insert(List *list, char *str) {
    Node *newNode = createNode(str);
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

// Função para deletar um nó da lista
void delete(List *list, char *str) {
    Node *currentNode = list->head;
    Node *previousNode = NULL;
    while (currentNode != NULL) {
        if (strcmp(currentNode->string, str) == 0) {
            if (currentNode == list->head) {
                list->head = currentNode->next;
            } else if (currentNode == list->tail) {
                list->tail = previousNode;
                previousNode->next = NULL;
            } else {
                previousNode->next = currentNode->next;
            }
            free(currentNode->string);
            free(currentNode);
            return;
        }
        previousNode = currentNode;
        currentNode = currentNode->next;
    }
    printf("Erro: Item não encontrado!\n");
}

// Função para liberar a lista
void freeList(List *list) {
    Node *currentNode = list->head;
    Node *nextNode;
    while (currentNode != NULL) {
        nextNode = currentNode->next;
        free(currentNode->string);
        free(currentNode);
        currentNode = nextNode;
    }
    list->head = NULL;
    list->tail = NULL;
}

int main() {
    List list = { NULL, NULL };

    insert(&list, "apple");
    insert(&list, "banana");
    insert(&list, "orange");

    delete(&list, "orange");

    Node *currentNode = list.head;
    while (currentNode != NULL) {
        printf("%s\n", currentNode->string);
        currentNode = currentNode->next;
    }

    freeList(&list);

    return 0;
}
