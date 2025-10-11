#include <stdio.h>
#include <stdlib.h>

struct Node {
    char ch;
    int freq;
    struct Node* left;
    struct Node* right;
};

struct Queue {
    struct Node* data[100];
    int front, rear;
};

void initQueue(struct Queue* q) {
    q->front = q->rear = 0;
}

int isEmpty(struct Queue* q) {
    return q->front == q->rear;
}

void enqueue(struct Queue* q, struct Node* node) {
    q->data[q->rear++] = node;
}

struct Node* dequeue(struct Queue* q) {
    return q->data[q->front++];
}

struct Node* createNode(char ch) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->ch = ch;
    newNode->freq = 1;
    newNode->left = newNode->right = NULL;
    return newNode;
}

void insertCharacter(struct Node** root, char ch) {
    if (*root == NULL) {
        *root = createNode(ch);
        return;
    }

    struct Queue q;
    initQueue(&q);
    enqueue(&q, *root);

    while (!isEmpty(&q)) {
        struct Node* temp = dequeue(&q);

        if (temp->ch == ch) {
            temp->freq++;
            return;
        }

        if (temp->left)
            enqueue(&q, temp->left);
        else {
            temp->left = createNode(ch);
            return;
        }

        if (temp->right)
            enqueue(&q, temp->right);
        else {
            temp->right = createNode(ch);
            return;
        }
    }
}

void levelOrder(struct Node* root) {
    if (root == NULL) return;

    struct Queue q;
    initQueue(&q);
    enqueue(&q, root);

    while (!isEmpty(&q)) {
        struct Node* temp = dequeue(&q);
        printf("(%c, %d) ", temp->ch, temp->freq);

        if (temp->left) enqueue(&q, temp->left);
        if (temp->right) enqueue(&q, temp->right);
    }
}


int main() {
    char str[100];
    printf("Enter string: ");
    scanf("%s", str);

    struct Node* root = NULL;

    for (int i = 0; str[i] != '\0'; i++)
        insertCharacter(&root, str[i]);

    printf("\nLevel Order Traversal with frequencies:\n");
    levelOrder(root);
    return 0;
}