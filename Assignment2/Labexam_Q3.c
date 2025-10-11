#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100


struct Stack {
    char operations[MAX][50];
    int top;
};


void init(struct Stack *s) {
    s->top = -1;
}

int isEmpty(struct Stack *s) {
    return s->top == -1;
}

int isFull(struct Stack *s) {
    return s->top == MAX - 1;
}


void push(struct Stack *s, char *op) {
    if (isFull(s)) {
        printf("Stack Overflow!\n");
        return;
    }
    strcpy(s->operations[++s->top], op);
}

char* pop(struct Stack *s) {
    if (isEmpty(s)) {
        return NULL;
    }
    return s->operations[s->top--];
}


void clear(struct Stack *s) {
    s->top = -1;
}

void performOperation(struct Stack *undo, struct Stack *redo, char *op) {
    push(undo, op);
    clear(redo); 
    printf("Performed operation: \"%s\"\n", op);
}


void undoOperation(struct Stack *undo, struct Stack *redo) {
    if (isEmpty(undo)) {
        printf("Nothing to undo!\n");
        return;
    }
    char *op = pop(undo);
    push(redo, op);
    printf("Undo: Operation that can be undone is \"%s\"\n", op);
}


void redoOperation(struct Stack *undo, struct Stack *redo) {
    if (isEmpty(redo)) {
        printf("Nothing to redo!\n");
        return;
    }
    char *op = pop(redo);
    push(undo, op);
    printf("Redo: Operation that can be redone is \"%s\"\n", op);
}


void displayStacks(struct Stack *undo, struct Stack *redo) {
    printf("\nCurrent Undo Stack: ");
    for (int i = 0; i <= undo->top; i++)
        printf("%s ", undo->operations[i]);

    printf("\nCurrent Redo Stack: ");
    for (int i = 0; i <= redo->top; i++)
        printf("%s ", redo->operations[i]);
    printf("\n");
}

int main() {
    struct Stack undo, redo;
    init(&undo);
    init(&redo);

    int choice;
    char op[50];

    printf("\n--- Text Editor Undo-Redo Simulation ---\n");

    do {
        printf("\n1. Perform Operation\n2. Undo\n3. Redo\n4. Display Stacks\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter operation name: ");
                scanf("%s", op);
                performOperation(&undo, &redo, op);
                break;

            case 2:
                undoOperation(&undo, &redo);
                break;

            case 3:
                redoOperation(&undo, &redo);
                break;

            case 4:
                displayStacks(&undo, &redo);
                break;

            case 5:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 5);

    return 0;
}