#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASK_LEN 256
#define TASK_FILE "tasks.txt"

typedef struct {
    char title[MAX_TASK_LEN];
    int done;
} Task;

typedef struct {
    Task* tasks;
    int size;
    int capacity;
} TaskList;

TaskList* createTaskList() {
    TaskList* list = (TaskList*)malloc(sizeof(TaskList));
    list->tasks = (Task*)malloc(sizeof(Task) * 10);
    list->size = 0;
    list->capacity = 10;
    return list;
}

void freeTaskList(TaskList* list) {
    free(list->tasks);
    free(list);
}

void addTask(TaskList* list, const char* title) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->tasks = (Task*)realloc(list->tasks, sizeof(Task) * list->capacity);
    }
    strncpy(list->tasks[list->size].title, title, MAX_TASK_LEN);
    list->tasks[list->size].done = 0;
    list->size++;
    printf("Task added: %s\n", title);
}

void listTasks(TaskList* list) {
    printf("\n-- Task List --\n");
    for (int i = 0; i < list->size; i++) {
        printf("%d. [%c] %s\n", i + 1, list->tasks[i].done ? 'x' : ' ', list->tasks[i].title);
    }
    printf("----------------\n");
}

void markTaskDone(TaskList* list, int index) {
    if (index < 0 || index >= list->size) {
        printf("Invalid task number\n");
        return;
    }
    list->tasks[index].done = 1;
    printf("Task marked as done: %s\n", list->tasks[index].title);
}

void deleteTask(TaskList* list, int index) {
    if (index < 0 || index >= list->size) {
        printf("Invalid task number\n");
        return;
    }
    for (int i = index; i < list->size - 1; i++) {
        list->tasks[i] = list->tasks[i + 1];
    }
    list->size--;
    printf("Task deleted.\n");
}

void saveTasks(TaskList* list) {
    FILE* file = fopen(TASK_FILE, "w");
    if (!file) { perror("Failed to open file"); return; }
    for (int i = 0; i < list->size; i++) {
        fprintf(file, "%d|%s\n", list->tasks[i].done, list->tasks[i].title);
    }
    fclose(file);
}

void loadTasks(TaskList* list) {
    FILE* file = fopen(TASK_FILE, "r");
    if (!file) return; // No file yet
    char line[MAX_TASK_LEN + 10];
    while (fgets(line, sizeof(line), file)) {
        char* sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        int done = atoi(line);
        char* title = sep + 1;
        title[strcspn(title, "\n")] = '\0'; // remove newline
        addTask(list, title);
        list->tasks[list->size - 1].done = done;
    }
    fclose(file);
}

void searchTasks(TaskList* list, const char* keyword) {
    printf("\n-- Search Results --\n");
    for (int i = 0; i < list->size; i++) {
        if (strstr(list->tasks[i].title, keyword)) {
            printf("%d. [%c] %s\n", i + 1, list->tasks[i].done ? 'x' : ' ', list->tasks[i].title);
        }
    }
    printf("--------------------\n");
}

int main() {
    TaskList* list = createTaskList();
    loadTasks(list);

    int choice;
    char input[MAX_TASK_LEN];

    do {
        printf("\n1. Add Task\n2. List Tasks\n3. Mark Task Done\n4. Delete Task\n5. Search Task\n6. Exit\nChoice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch(choice) {
            case 1:
                printf("Enter task title: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                addTask(list, input);
                saveTasks(list);
                break;
            case 2:
                listTasks(list);
                break;
            case 3:
                printf("Enter task number to mark done: ");
                int doneIndex;
                scanf("%d", &doneIndex);
                markTaskDone(list, doneIndex - 1);
                saveTasks(list);
                break;
            case 4:
                printf("Enter task number to delete: ");
                int delIndex;
                scanf("%d", &delIndex);
                deleteTask(list, delIndex - 1);
                saveTasks(list);
                break;
            case 5:
                printf("Enter keyword to search: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                searchTasks(list, input);
                break;
            case 6:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while(choice != 6);

    freeTaskList(list);
    return 0;
}
