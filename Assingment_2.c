#include <stdio.h>
#include <string.h>

#define MAX 50

int main() {
    int m, n;
    char grid[MAX][MAX];
    char word[MAX];
    int found = 0;

    printf("Enter number of rows: ");
    scanf("%d", &m);

    printf("Enter number of columns: ");
    scanf("%d", &n);
    printf("Enter the grid :\n", m);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            scanf(" %c", &grid[i][j]);
        }
    }


    printf("Enter the target word: ");
    scanf("%s", word);

    int len = strlen(word);

 
    for (int i = 0; i < m; i++) {
        for (int j = 0; j <= n - len; j++) {
            int k;
            for (k = 0; k < len; k++) {
                if (grid[i][j + k] != word[k])
                    break;
            }
            if (k == len) {
                printf("Found horizontally -> Start: (%d, %d) End: (%d, %d)\n",
                       i, j, i, j + len - 1);
                found = 1;
            }
        }
    }
    for (int j = 0; j < n; j++) {
        for (int i = 0; i <= m - len; i++) {
            int k;
            for (k = 0; k < len; k++) {
                if (grid[i + k][j] != word[k])
                    break;
            }
            if (k == len) {
                printf("Found vertically -> Start: (%d, %d) End: (%d, %d)\n",
                       i, j, i + len - 1, j);
                found = 1;
            }
        }
    }

    if (!found)
        printf("Word not found\n");

    return 0;
}
