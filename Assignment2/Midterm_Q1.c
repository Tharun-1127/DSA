#include <stdio.h>
#include <string.h>

#define DAYS 30
#define MAX_MEMBERS 50

struct Member {
    char name[50];
    int steps[DAYS];
};


void analyzeSteps(struct Member members[], int n) {
    for (int i = 0; i < n; i++) {
        int count = 0, max = 0;
        for (int j = 0; j < DAYS; j++) {
            if (members[i].steps[j] > 10000)
                count++;
            if (members[i].steps[j] > max)
                max = members[i].steps[j];
        }
        printf("\nMember: %s\n", members[i].name);
        printf("Days exceeding 10,000 steps: %d\n", count);
        printf("Maximum steps in month: %d\n", max);
    }
}

int main() {
    int n;
    printf("Enter number of members: ");
    scanf("%d", &n);

    struct Member members[MAX_MEMBERS];

    for (int i = 0; i < n; i++) {
        printf("\nEnter member name: ");
        scanf("%s", members[i].name);

        printf("Enter 30 days step counts for %s:\n", members[i].name);
        for (int j = 0; j < DAYS; j++)
            scanf("%d", &members[i].steps[j]);
    }

    analyzeSteps(members, n);
    return 0;
}