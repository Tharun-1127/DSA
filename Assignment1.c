#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to store tab information
struct Tab {
    int pageID;
    char URL[100];
    struct Tab* next;
    struct Tab* prev;
};

// Global pointers for navigation
struct Tab* current = NULL;
struct Tab* head = NULL;
int tabCount = 0;

void visitNewPage() {
    struct Tab* newTab = (struct Tab*)malloc(sizeof(struct Tab));
    newTab->pageID = ++tabCount;
    printf("Enter URL for page %d: ", newTab->pageID);
    scanf("%s", newTab->URL);
    
    newTab->next = NULL;
    newTab->prev = current;
    
    if (current == NULL) {
        head = newTab;
    } else {
        current->next = newTab;
    }
    current = newTab;
    printf("Visited page %d with URL: %s\n", newTab->pageID, newTab->URL);
}

void moveToNextTab() {
    if (current && current->next) {
        current = current->next;
        printf("Moved to page %d with URL: %s\n", current->pageID, current->URL);
    } else {
        printf("No next tab exists.\n");
    }
}

void moveToPrevTab() {
    if (current && current->prev) {
        current = current->prev;
        printf("Moved to page %d with URL: %s\n", current->pageID, current->URL);
    } else {
        printf("No previous tab exists.\n");
    }
}

void showCurrentTab() {
    if (current) {
        printf("Current tab - Page ID: %d, URL: %s\n", current->pageID, current->URL);
    } else {
        printf("No tabs open.\n");
    }
}

void closeTab() {
    if (!current) {
        printf("No tabs to close.\n");
        return;
    }
    struct Tab* temp = current;
    if (current->prev) current->prev->next = current->next;
    if (current->next) current->next->prev = current->prev;
    if (current == head) head = current->next;
    current = current->prev ? current->prev : current->next;
    free(temp);
    if (current) {
        printf("Closed current tab. New current tab - Page ID: %d, URL: %s\n", current->pageID, current->URL);
    } else {
        printf("All tabs closed.\n");
    }
}

void showHistory() {
    struct Tab* temp = head;
    if (!temp) {
        printf("No history available.\n");
        return;
    }
    printf("Tab History:\n");
    while (temp) {
        printf("Page ID: %d, URL: %s\n", temp->pageID, temp->URL);
        temp = temp->next;
    }
}

int main() {
    int choice;
    while (1) {
        printf("\nBrowser Menu:\n1. Visit a new page\n2. Go back\n3. Go forward\n4. Show current tab\n5. Close current tab\n6. Show history\n7. Exit\nEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: visitNewPage(); break;
            case 2: moveToPrevTab(); break;
            case 3: moveToNextTab(); break;
            case 4: showCurrentTab(); break;
            case 5: closeTab(); break;
            case 6: showHistory(); break;
            case 7: return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}