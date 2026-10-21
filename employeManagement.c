
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMPLOYEES 100

struct Employee {
    int id;
    char name[50];
    char designation[50];

    struct Employee *manager;
    struct Employee *reports[MAX_EMPLOYEES];
    int reportCount;
};

struct Employee *employees[MAX_EMPLOYEES];

int employeeCount = 0;

struct Employee *findEmployeeById(int id) {
    for (int i = 0; i < employeeCount; i++) {
        if (employees[i]->id == id) {
            return employees[i];
        }
    }
    return NULL;
}

void addReport(struct Employee *manager, struct Employee *employee) {
    if (manager == NULL) {
        return;
    }

    if (manager->reportCount >= MAX_EMPLOYEES) {
        return;
    }

    manager->reports[manager->reportCount] = employee;
    manager->reportCount++;
}

void removeReport(struct Employee *manager, struct Employee *employee) {
    if (manager == NULL) {
        return;
    }

    for (int i = 0; i < manager->reportCount; i++) {
        if (manager->reports[i] == employee) {
            for (int j = i; j < manager->reportCount - 1; j++) {
                manager->reports[j] = manager->reports[j + 1];
            }

            manager->reportCount--;
            return;
        }
    }
}

void addEmployee(void) {
    int id, managerID;
    char name[50], designation[50];

    if (employeeCount >= MAX_EMPLOYEES) {
        printf("Organization is full.\n");
        return;
    }

    printf("Enter Employee ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return;
    }

    if (findEmployeeById(id) != NULL) {
        printf("Employee id exists.\n");
        return;
    }

    printf("Enter Name: ");
    if (scanf(" %49[^\n]", name) != 1) {
        printf("Invalid name.\n");
        return;
    }

    printf("Enter Designation: ");
    if (scanf(" %49[^\n]", designation) != 1) {
        printf("Invalid designation.\n");
        return;
    }

    printf("Enter Manager ID (0 for none): ");
    if (scanf("%d", &managerID) != 1) {
        printf("Invalid manager ID.\n");

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return;
    }

    struct Employee *manager = NULL;

    if (managerID == id) {
        printf("An Employee cannot manage themselves.\n");
        return;
    }

    if (managerID != 0) {
        manager = findEmployeeById(managerID);

        if (manager == NULL) {
            printf("Manager not found.\n");
            return;
        }

        if (manager->reportCount >= MAX_EMPLOYEES) {
            printf("Manager has too many reporting employees.\n");
            return;
        }
    }

    struct Employee *newEmployee =
        malloc(sizeof(struct Employee));

    if (newEmployee == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    newEmployee->id = id;
    strcpy(newEmployee->name, name);
    strcpy(newEmployee->designation, designation);
    newEmployee->manager = manager;
    newEmployee->reportCount = 0;

    employees[employeeCount] = newEmployee;
    employeeCount++;

    addReport(manager, newEmployee);

    printf("Employee added successfully.\n");
}

int countUnder(struct Employee *employee) {
    if (employee == NULL) {
        return 0;
    }

    int total = 0;

    for (int i = 0; i < employee->reportCount; i++) {
        total += 1 + countUnder(employee->reports[i]);
    }

    return total;
}

void displaySubtree(struct Employee *employee, int level) {
    if (employee == NULL) {
        return;
    }

    for (int i = 0; i < level; i++) {
        printf("   ");
    }

    printf("%d - %s - %s\n",
           employee->id,
           employee->name,
           employee->designation);

    for (int i = 0; i < employee->reportCount; i++) {
        displaySubtree(employee->reports[i], level + 1);
    }
}

void displayEmployeeHierarchy(void) {
    int id;

    printf("Enter Employee ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid id.\n");

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return;
    }

    struct Employee *employee = findEmployeeById(id);

    if (employee == NULL) {
        printf("Employee not found.\n");
        return;
    }

    printf("Employee Hierarchy:\n");
    displaySubtree(employee, 0);

    printf("Total employees under %d: %d\n",
           id, countUnder(employee));
}

void displayCompleteHierarchy(void) {
    if (employeeCount == 0) {
        printf("No employees in the organization.\n");
        return;
    }

    printf("\n===== Organization Hierarchy =====\n");

    for (int i = 0; i < employeeCount; i++) {
        if (employees[i]->manager == NULL) {
            displaySubtree(employees[i], 0);
        }
    }

    printf("Total Employees: %d\n", employeeCount);
}

void removeEmployee(void) {
    int id;

    printf("Enter Employee ID to remove: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid id.\n");

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return;
    }

    struct Employee *target = findEmployeeById(id);

    if (target == NULL) {
        printf("Employee not found.\n");
        return;
    }

    // The CEO cannot be removed.
    if (target->manager == NULL) {
        printf("CEO cannot be removed.\n");
        return;
    }

    struct Employee *manager = target->manager;
    struct Employee *sibling = NULL;

    // Find the sibling with the fewest direct reports.
    for (int i = 0; i < manager->reportCount; i++) {
        struct Employee *current = manager->reports[i];

        if (current != target) {
            if (sibling == NULL ||
                current->reportCount < sibling->reportCount) {
                sibling = current;
            }
        }
    }

    // Transfer all direct reports before deleting the employee.
    while (target->reportCount > 0) {
        struct Employee *child = target->reports[0];

        if (sibling != NULL) {
            // Transfer the child to the selected sibling.
            child->manager = sibling;
            addReport(sibling, child);
        } else {
            // No siblings: transfer the child to the superior.
            child->manager = manager;
            addReport(manager, child);
        }

        removeReport(target, child);
    }

    // Remove the target from the manager's report list.
    removeReport(manager, target);

    // Remove the target from the main employee array.
    for (int i = 0; i < employeeCount; i++) {
        if (employees[i] == target) {
            for (int j = i; j < employeeCount - 1; j++) {
                employees[j] = employees[j + 1];
            }

            employeeCount--;
            break;
        }
    }

    free(target);

    printf("Employee removed successfully.\n");
}

int main(void) {
    int choice = 0;

    do {
        printf("\n===== Employee Hierarchy Management =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}

            if (feof(stdin)) {
                break;
            }

            continue;
        }

        switch (choice) {
            case 1:
                addEmployee();
                break;

            case 2:
                removeEmployee();
                break;

            case 3:
                displayEmployeeHierarchy();
                break;

            case 4:
                displayCompleteHierarchy();
                break;

            case 5:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice. Please enter a number between 1 and 5.\n");
        }

    } while (choice != 5);

    for (int i = 0; i < employeeCount; i++) {
        free(employees[i]);
    }

    return 0;
}
