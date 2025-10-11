#include <stdio.h>
#include <stdlib.h>


struct Node {
    int songID;
    struct Node* next;
};


struct Node* createNode(int songID) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->songID = songID;
    newNode->next = NULL;
    return newNode;
}

void printPlaylist(struct Node* head) {
    struct Node* temp = head;
    while (temp) {
        printf("%d", temp->songID);
        if (temp->next) printf(" -> ");
        temp = temp->next;
    }
    printf("\n");
}

struct Node* reversePlaylistSegment(struct Node* head, int m, int n) {
    if (head == NULL || m == n) return head;

    struct Node dummy;
    dummy.next = head;
    struct Node* prev = &dummy;

 
    for (int i = 1; i < m; i++) {
        if (prev == NULL) return head;
        prev = prev->next;
    }

    struct Node* start = prev->next;
    struct Node* then = start->next;


    for (int i = 0; i < n - m; i++) {
        start->next = then->next;
        then->next = prev->next;
        prev->next = then;
        then = start->next;
    }

    return dummy.next;
}


int main() {
    int n, m, k;
    struct Node* head = NULL;
    struct Node* tail = NULL;

    printf("Enter number of songs in the playlist: ");
    scanf("%d", &n);

    printf("Enter the song IDs:\n");
    for (int i = 0; i < n; i++) {
        int id;
        scanf("%d", &id);
        struct Node* newNode = createNode(id);
        if (head == NULL)
            head = newNode;
        else
            tail->next = newNode;
        tail = newNode;
    }

    printf("\nOriginal Playlist:\n");
    printPlaylist(head);

    printf("\nEnter m and n positions to reverse (1-based index): ");
    scanf("%d %d", &m, &k);

    head = reversePlaylistSegment(head, m, k);

    printf("\nModified Playlist:\n");
    printPlaylist(head);

    return 0;
}