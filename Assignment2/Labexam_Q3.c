#include <stdio.h>
#define MAX_SIZE 5

struct CallQueue {
    int items[MAX_SIZE];
    int front, rear;
    int count;
};

void initQueue(struct CallQueue* q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int isFull(struct CallQueue* q) {
    return q->count == MAX_SIZE;
}

int isEmpty(struct CallQueue* q) {
    return q->count == 0;
}

void enqueue(struct CallQueue* q, int customerId) {
    if (isFull(q)) {
        printf("Queue is full. Cannot add customer %d\n", customerId);
        return;
    }
    q->rear = (q->rear + 1) % MAX_SIZE;
    q->items[q->rear] = customerId;
    q->count++;
    printf("Customer %d added to queue\n", customerId);
}

int dequeue(struct CallQueue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty. No customer to remove\n");
        return -1;
    }
    int customerId = q->items[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    q->count--;
    printf("Customer %d removed from queue\n", customerId);
    return customerId;
}

void display(struct CallQueue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    printf("Queue: ");
    int i = q->front;
    for (int count = 0; count < q->count; count++) {
        printf("%d ", q->items[i]);
        i = (i + 1) % MAX_SIZE;
    }
    printf("\n");
}

int main() {
    struct CallQueue q;
    initQueue(&q);

    enqueue(&q, 101);
    enqueue(&q, 102);
    enqueue(&q, 103);
    display(&q);
    dequeue(&q);
    display(&q);
    enqueue(&q, 104);
    display(&q);

    return 0;
}