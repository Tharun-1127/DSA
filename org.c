/* SIMPLE & FIXED ORG.C
 * Features:
 * - Load employees from CSV
 * - Print full hierarchy
 * - Search by ID
 * - Search by name (CASE-INSENSITIVE + FIXED)
 * - Print reporting path & peers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EMP 1000
#define NAME_LEN 64
#define TITLE_LEN 64
#define LINE_LEN 256
#define MAX_CHILDREN 100

typedef struct Employee {
    int id;
    char name[NAME_LEN];
    char title[TITLE_LEN];
    char email[64];
    char phone[32];
    int manager_id;
    struct Employee *manager;
    struct Employee *children[MAX_CHILDREN];
    int child_count;
} Employee;

Employee *employees[MAX_EMP];
int emp_count = 0;

/* TRIM */
void trim(char *s) {
    if (!s) return;
    int len = strlen(s);
    while (len > 0 && (s[len-1]=='\n'||s[len-1]=='\r'||s[len-1]==' '||s[len-1]=='\t'))
        s[--len] = 0;
    int i = 0;
    while (s[i]==' ' || s[i]=='\t') i++;
    if (i > 0) memmove(s, s+i, strlen(s+i)+1);
}

/* FIND BY ID */
Employee *find_by_id(int id) {
    for (int i = 0; i < emp_count; i++)
        if (employees[i]->id == id) return employees[i];
    return NULL;
}

/* CASE-INSENSITIVE SUBSTRING */
int ci_contains(const char *s, const char *sub) {
    if (!s || !sub) return 0;
    int n = strlen(s), m = strlen(sub);

    for (int i = 0; i <= n - m; i++) {
        int ok = 1;
        for (int j = 0; j < m; j++) {
            if (tolower((unsigned char)s[i+j]) != tolower((unsigned char)sub[j])) {
                ok = 0;
                break;
            }
        }
        if (ok) return 1;
    }
    return 0;
}

/* PRINT TREE */
void print_tree(Employee *e, int depth) {
    if (!e) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%d | %s (%s) [%s, %s]\n", e->id, e->name, e->title, e->email, e->phone);

    for (int i = 0; i < e->child_count; i++)
        print_tree(e->children[i], depth + 1);
}

/* REPORTING PATH */
void print_reporting(Employee *e) {
    printf("Reporting Path:\n");
    Employee *cur = e;
    while (cur) {
        printf("-> %d | %s (%s)\n", cur->id, cur->name, cur->title);
        cur = cur->manager;
    }
}

/* PEERS */
void print_peers(Employee *e) {
    if (!e->manager) {
        printf("No peers. (Top-level)\n");
        return;
    }

    printf("Peers:\n");
    int has = 0;
    for (int i = 0; i < e->manager->child_count; i++) {
        Employee *ch = e->manager->children[i];
        if (ch->id != e->id) {
            printf("- %d | %s (%s)\n", ch->id, ch->name, ch->title);
            has = 1;
        }
    }
    if (!has) printf("(No peers)\n");
}

/* LOAD CSV */
int load_csv(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror("fopen"); return 0; }

    emp_count = 0;
    char line[LINE_LEN];

    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (line[0] == 0) continue;

                /* Skip header if present (line contains 'id' and 'name') */
        {
            char lowline[LINE_LEN];
            strncpy(lowline, line, LINE_LEN - 1);
            lowline[LINE_LEN - 1] = '\0';
            for (int _i = 0; lowline[_i]; _i++) lowline[_i] = tolower((unsigned char)lowline[_i]);
            if (strstr(lowline, "id") && strstr(lowline, "name")) {
                continue; /* skip header row */
            }
        }

        char *tok = strtok(line, ",");
        if (!tok) continue;
        int id = atoi(tok);

                /* --- NAME --- */
        tok = strtok(NULL, ",");
        if (!tok) continue;
        char name[NAME_LEN];
        memset(name, 0, NAME_LEN);
        strncpy(name, tok, NAME_LEN - 1);
        name[NAME_LEN - 1] = '\0';
        trim(name);

        /* --- TITLE --- */
        tok = strtok(NULL, ",");
        if (!tok) continue;
        char title[TITLE_LEN];
        memset(title, 0, TITLE_LEN);
        strncpy(title, tok, TITLE_LEN - 1);
        title[TITLE_LEN - 1] = '\0';
        trim(title);

        /* --- MANAGER ID --- */
        tok = strtok(NULL, ",");
        int mid = tok ? atoi(tok) : -1;

        /* --- EMAIL --- */
        tok = strtok(NULL, ",");
        char email[64];
        memset(email, 0, sizeof(email));
        if (tok) {
            strncpy(email, tok, sizeof(email) - 1);
            email[sizeof(email) - 1] = '\0';
            trim(email);
        } else {
            email[0] = '\0';
        }

        /* --- PHONE (last field may contain newline) --- */
        tok = strtok(NULL, "\n");
        char phone[32];
        memset(phone, 0, sizeof(phone));
        if (tok) {
            trim(tok);
            strncpy(phone, tok, sizeof(phone) - 1);
            phone[sizeof(phone) - 1] = '\0';
            trim(phone);
        } else {
            phone[0] = '\0';
        }
      

        Employee *e = malloc(sizeof(Employee));
        e->id = id;
        strcpy(e->name, name);
        strcpy(e->title, title);
        strcpy(e->email, email);
        strcpy(e->phone, phone);
        e->manager_id = mid;
        e->manager = NULL;
        e->child_count = 0;

        employees[emp_count++] = e;
    }
    fclose(f);

    /* LINK MANAGERS */
    for (int i = 0; i < emp_count; i++) {
        Employee *e = employees[i];
        if (e->manager_id > 0) {
            Employee *m = find_by_id(e->manager_id);
            if (m) {
                e->manager = m;
                m->children[m->child_count++] = e;
            }
        }
    }
    return 1;
}

/* MENU */
void menu(const char *csv) {
    char input[128];

    while (1) {
        printf("\n===== MENU =====\n");
        printf("1) Display hierarchy\n");
        printf("2) Search by ID\n");
        printf("3) Search by name (case-insensitive)\n");
        printf("4) Reload CSV\n");
        printf("5) Exit\n");
        printf("Choose: ");

        fgets(input, 128, stdin);
        int opt = atoi(input);

        if (opt == 1) {
            for (int i = 0; i < emp_count; i++)
                if (employees[i]->manager == NULL)
                    print_tree(employees[i], 0);

        } else if (opt == 2) {
            printf("Enter ID: ");
            fgets(input, 128, stdin);
            int id = atoi(input);

            Employee *e = find_by_id(id);
            if (!e) printf("Not found.\n");
            else {
                printf("%d | %s (%s)\n", e->id, e->name, e->title);

                printf("Email: %s | Phone: %s\n", e->email, e->phone);
                print_reporting(e);
                print_peers(e);
            }

        } else if (opt == 3) {
            printf("Enter substring: ");
            fgets(input, 128, stdin);
            trim(input);

            int found = 0;
            for (int i = 0; i < emp_count; i++) {
                if (ci_contains(employees[i]->name, input)) {
                    found = 1;

                    printf("\n%d | %s (%s)\n", employees[i]->id, employees[i]->name, employees[i]->title);
                    printf("Email: %s | Phone: %s\n", employees[i]->email, employees[i]->phone);
                    print_reporting(employees[i]);
                }
            }
            if (!found) printf("No matches.\n");

        } else if (opt == 4) {
            load_csv(csv);
            printf("Reloaded.\n");

        } else if (opt == 5) {
            printf("Goodbye!\n");
            break;
        } else {
            printf("Invalid option.\n");
        }
    }
}

/* MAIN */
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s employees.csv\n", argv[0]);
        return 1;
    }

    if (!load_csv(argv[1])) {
        printf("Failed loading file.\n");
        return 1;
    }

    printf("Loaded %d employees.\n", emp_count);
    menu(argv[1]);
    return 0;
}
