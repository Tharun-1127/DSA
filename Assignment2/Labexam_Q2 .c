#include <stdio.h>
#define MAX_SIZE 5

struct CallQueue {
    int items[MAX_SIZE];
    int front;
    int rear;
    int count;
};

// Initialize queue
void initQueue(struct CallQueue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

// Check if queue is full
int isFull(struct CallQueue *q) {
    return q->count == MAX_SIZE;
}

// Check if queue is empty
int isEmpty(struct CallQueue *q) {
    return q->count == 0;
}

// Add a call to the queue
void addCall(struct CallQueue *q, int callID) {
    if (isFull(q)) {
        printf("Queue is full please wait!\n");
        return;
    }
    q->rear = (q->rear + 1) % MAX_SIZE;
    q->items[q->rear] = callID;
    q->count++;
    printf("Call added: %d\n", callID);
}

// Remove a call when agent is free
void removeCall(struct CallQueue *q) {
    if (isEmpty(q)) {
        printf("No calls to remove (Queue is empty)\n");
        return;
    }
    int removed = q->items[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    q->count--;
    printf("Removed customer: %d\n", removed);
}

// Display the queue
void displayQueue(struct CallQueue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }

    printf("Customers in queue: ");
    int i;
    for (i = 0; i < q->count; i++) {
        int index = (q->front + i) % MAX_SIZE;
        printf("%d ", q->items[index]);
    }
    printf("\n");
}

// Main function
int main() {
    struct CallQueue q;
    initQueue(&q);

    int choice, id;

    while (1) {
        printf("\n--- Customer Service Call Queue ---\n");
        printf("1. Add new call\n");
        printf("2. Remove call (agent free)\n");
        printf("3. Display queue\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter customer ID to add: ");
                scanf("%d", &id);
                addCall(&q, id);
                break;
            case 2:
                removeCall(&q);
                break;
            case 3:
                displayQueue(&q);
                break;
            case 4:
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}