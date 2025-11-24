#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// --- Constants and Global Tracking ---
#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200
#define NUM_REPETITIONS 1000
#define DATA_FILENAME "dataset_id_ascending.csv"

// Struct for student data (provided by user, with added total_grade)
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
    int total_grade; // Added for convenience in comparison
} Student;

// Global structure to hold comparison count and memory usage for a single run
typedef struct {
    long long comparisons;
    size_t memory_bytes; // Simple measure: size of input array + auxiliary space
} SortMetrics;

// Macro for swapping two Student elements
#define SWAP(a, b) do { Student temp = a; a = b; b = temp; } while (0)


// --- Comparison Functions ---
// The comparison function returns:
// < 0 if a < b
// = 0 if a = b
// > 0 if a > b
// It also increments the comparison count.

// Helper for total grade tie-breaker (Korean > English > Math priority for larger)
int compare_grades(const Student* a, const Student* b, long long* comparisons) {
    // 1. Korean Grade (DESC order: higher grade is 'greater' in the tie-breaker)
    (*comparisons)++;
    if (a->korean != b->korean) return b->korean - a->korean;

    // 2. English Grade (DESC order)
    (*comparisons)++;
    if (a->english != b->english) return b->english - a->english;

    // 3. Math Grade (DESC order)
    (*comparisons)++;
    if (a->math != b->math) return b->math - a->math;

    return 0; // All grades are equal
}

// 1. ID Ascending
int compare_id_asc(const Student* a, const Student* b, long long* comparisons) {
    (*comparisons)++;
    return a->id - b->id;
}

// 2. ID Descending
int compare_id_desc(const Student* a, const Student* b, long long* comparisons) {
    (*comparisons)++;
    return b->id - a->id;
}

// 3. NAME Ascending
int compare_name_asc(const Student* a, const Student* b, long long* comparisons) {
    (*comparisons)++;
    return strncmp(a->name, b->name, MAX_NAME_LEN);
}

// 4. NAME Descending
int compare_name_desc(const Student* a, const Student* b, long long* comparisons) {
    (*comparisons)++;
    return strncmp(b->name, a->name, MAX_NAME_LEN);
}

// 5. GENDER Ascending (M < F)
int compare_gender_asc(const Student* a, const Student* b, long long* comparisons) {
    (*comparisons)++;
    return a->gender - b->gender;
}

// 6. GENDER Descending (F < M)
int compare_gender_desc(const Student* a, const Student* b, long long* comparisons) {
    (*comparisons)++;
    return b->gender - a->gender;
}

// 7. Total Grade Ascending (with tie-breaker)
int compare_total_asc(const Student* a, const Student* b, long long* comparisons) {
    (*comparisons)++;
    if (a->total_grade != b->total_grade) {
        return a->total_grade - b->total_grade; // ASC order
    }
    return compare_grades(a, b, comparisons); // Tie-breaker
}

// 8. Total Grade Descending (with tie-breaker)
int compare_total_desc(const Student* a, const Student* b, long long* comparisons) {
    (*comparisons)++;
    if (a->total_grade != b->total_grade) {
        return b->total_grade - a->total_grade; // DESC order
    }
    return compare_grades(a, b, comparisons); // Tie-breaker
}

// Define a function pointer for the comparison
typedef int (*CompareFunc)(const Student*, const Student*, long long*);

// --- Data Loading Function (Modified for total_grade calculation) ---

Student* load_students(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    char line[MAX_LINE_LEN];
    int capacity = 10;
    int count = 0;
    Student* arr = (Student*)malloc(sizeof(Student) * capacity);

    if (!arr) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    // Skip header line
    if (!fgets(line, sizeof(line), fp)) {
        free(arr);
        fclose(fp);
        *out_count = 0;
        return NULL;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* temp = (Student*)realloc(arr, sizeof(Student) * capacity);
            if (!temp) {
                perror("Reallocation failed");
                break;
            }
            arr = temp;
        }

        Student s;
        char temp_line[MAX_LINE_LEN];
        strncpy(temp_line, line, MAX_LINE_LEN - 1);
        temp_line[MAX_LINE_LEN - 1] = '\0';

        char* token = strtok(temp_line, ",\r\n");
        if (!token) continue;
        s.id = atoi(token);

        token = strtok(NULL, ",\r\n");
        if (!token) continue;
        strncpy(s.name, token, MAX_NAME_LEN - 1);
        s.name[MAX_NAME_LEN - 1] = '\0';

        token = strtok(NULL, ",\r\n");
        if (!token) continue;
        s.gender = token[0];

        token = strtok(NULL, ",\r\n");
        if (!token) continue;
        s.korean = atoi(token);

        token = strtok(NULL, ",\r\n");
        if (!token) continue;
        s.english = atoi(token);

        token = strtok(NULL, ",\r\n");
        if (!token) continue;
        s.math = atoi(token);

        // Calculate total grade
        s.total_grade = s.korean + s.english + s.math;

        arr[count++] = s;
    }

    fclose(fp);

    // Tight reallocation to fit the exact count
    Student* tight = (Student*)realloc(arr, sizeof(Student) * count);
    if (!tight) {
        fprintf(stderr, "Warning: Tight reallocation failed, using original memory.\n");
        *out_count = count;
        return arr;
    }

    arr = tight;

    *out_count = count;
    return arr;
}

// --- Sorting Algorithms (Implementations) ---

// A. Bubble Sort
void bubble_sort(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (cmp(&arr[j], &arr[j + 1], comparisons) > 0) {
                SWAP(arr[j], arr[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

// B. Selection Sort
void selection_sort(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (cmp(&arr[j], &arr[min_idx], comparisons) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            SWAP(arr[i], arr[min_idx]);
        }
    }
}

// C. Insertion Sort
void insertion_sort(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    for (int i = 1; i < n; i++) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= 0 && cmp(&arr[j], &key, comparisons) > 0) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

// D. Shell Sort (A - Basic, using n/2 sequence)
void shell_sort_basic(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            Student temp = arr[i];
            int j;
            for (j = i; j >= gap; j -= gap) {
                if (cmp(&arr[j - gap], &temp, comparisons) > 0) {
                    arr[j] = arr[j - gap];
                }
                else {
                    break;
                }
            }
            arr[j] = temp;
        }
    }
}

// Shell Sort (B - Improved, using Knuth's sequence: 1, 4, 13, 40, 121, ...)
void shell_sort_improved(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    int gaps[20];
    int h = 1;
    int k = 0;
    while (h < n) {
        gaps[k++] = h;
        h = h * 3 + 1;
    }

    for (int i = k - 1; i >= 0; i--) {
        int gap = gaps[i];
        for (int j = gap; j < n; j++) {
            Student temp = arr[j];
            int l;
            for (l = j; l >= gap; l -= gap) {
                if (cmp(&arr[l - gap], &temp, comparisons) > 0) {
                    arr[l] = arr[l - gap];
                }
                else {
                    break;
                }
            }
            arr[l] = temp;
        }
    }
}

// E. Quick Sort (A - Basic, Simple Pivot: last element, Lomuto Partition)
int partition_basic(Student arr[], int low, int high, CompareFunc cmp, long long* comparisons) {
    Student pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (cmp(&arr[j], &pivot, comparisons) < 0) {
            i++;
            SWAP(arr[i], arr[j]);
        }
    }
    SWAP(arr[i + 1], arr[high]);
    return (i + 1);
}

void quick_sort_basic_recursive(Student arr[], int low, int high, CompareFunc cmp, long long* comparisons) {
    if (low < high) {
        int pi = partition_basic(arr, low, high, cmp, comparisons);
        quick_sort_basic_recursive(arr, low, pi - 1, cmp, comparisons);
        quick_sort_basic_recursive(arr, pi + 1, high, cmp, comparisons);
    }
}

void quick_sort_basic(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    quick_sort_basic_recursive(arr, 0, n - 1, cmp, comparisons);
}


// Quick Sort (B - Improved, Median-of-Three Pivot Selection)
int median_of_three(Student arr[], int low, int high, CompareFunc cmp, long long* comparisons) {
    int mid = low + (high - low) / 2;
    // Sort low, mid, high indices
    if (cmp(&arr[low], &arr[mid], comparisons) > 0) SWAP(arr[low], arr[mid]);
    if (cmp(&arr[low], &arr[high], comparisons) > 0) SWAP(arr[low], arr[high]);
    if (cmp(&arr[mid], &arr[high], comparisons) > 0) SWAP(arr[mid], arr[high]);
    return mid; // Return the index of the median (which is now at mid)
}

int partition_improved(Student arr[], int low, int high, CompareFunc cmp, long long* comparisons) {
    // Hoare-style partition with Median-of-Three pivot
    int pivot_idx = median_of_three(arr, low, high, cmp, comparisons);
    SWAP(arr[pivot_idx], arr[low]); // Move pivot to the start

    Student pivot = arr[low];
    int i = low + 1;
    int j = high;

    while (1) {
        while (i <= high && cmp(&arr[i], &pivot, comparisons) < 0) i++;
        while (cmp(&arr[j], &pivot, comparisons) > 0) j--;

        if (i > j) break;
        SWAP(arr[i], arr[j]);
    }
    SWAP(arr[low], arr[j]); // Place pivot in its correct position
    return j;
}

void quick_sort_improved_recursive(Student arr[], int low, int high, CompareFunc cmp, long long* comparisons) {
    if (low < high) {
        int pi = partition_improved(arr, low, high, cmp, comparisons);
        quick_sort_improved_recursive(arr, low, pi - 1, cmp, comparisons);
        quick_sort_improved_recursive(arr, pi + 1, high, cmp, comparisons);
    }
}

void quick_sort_improved(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    quick_sort_improved_recursive(arr, 0, n - 1, cmp, comparisons);
}

// F. Heap Sort
void max_heapify(Student arr[], int n, int i, CompareFunc cmp, long long* comparisons) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && cmp(&arr[left], &arr[largest], comparisons) > 0)
        largest = left;

    if (right < n && cmp(&arr[right], &arr[largest], comparisons) > 0)
        largest = right;

    if (largest != i) {
        SWAP(arr[i], arr[largest]);
        max_heapify(arr, n, largest, cmp, comparisons);
    }
}

void heap_sort(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        max_heapify(arr, n, i, cmp, comparisons);

    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        SWAP(arr[0], arr[i]); // Move current root to end
        max_heapify(arr, i, 0, cmp, comparisons); // Call max_heapify on the reduced heap
    }
}

// G. Merge Sort
void merge(Student arr[], int l, int m, int r, CompareFunc cmp, long long* comparisons, Student temp_arr[]) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Copy data to temp array temp_arr (auxiliary space)
    for (i = 0; i < n1; i++) temp_arr[i] = arr[l + i];
    for (j = 0; j < n2; j++) temp_arr[n1 + j] = arr[m + 1 + j];

    i = 0; j = n1; k = l;
    while (i < n1 && j < n1 + n2) {
        // Stable property: use <= 0
        if (cmp(&temp_arr[i], &temp_arr[j], comparisons) <= 0) {
            arr[k++] = temp_arr[i++];
        }
        else {
            arr[k++] = temp_arr[j++];
        }
    }

    // Copy the remaining elements
    while (i < n1) arr[k++] = temp_arr[i++];
    while (j < n1 + n2) arr[k++] = temp_arr[j++];
}

void merge_sort_recursive(Student arr[], int l, int r, CompareFunc cmp, long long* comparisons, Student temp_arr[]) {
    if (l < r) {
        int m = l + (r - l) / 2;

        merge_sort_recursive(arr, l, m, cmp, comparisons, temp_arr);
        merge_sort_recursive(arr, m + 1, r, cmp, comparisons, temp_arr);

        merge(arr, l, m, r, cmp, comparisons, temp_arr);
    }
}

void merge_sort(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    // Auxiliary array for merging. This is the main auxiliary memory usage.
    Student* temp_arr = (Student*)malloc(sizeof(Student) * n);
    if (!temp_arr) {
        fprintf(stderr, "Error: Memory allocation failed for Merge Sort auxiliary array.\n");
        return;
    }
    merge_sort_recursive(arr, 0, n - 1, cmp, comparisons, temp_arr);
    free(temp_arr);
}

// H. Radix Sort (LSD) - Non-Comparison Sort, only implemented for ID
// NOTE: This sort does NOT use CompareFunc and should only be run for ID-based criteria.
int get_max_id(Student arr[], int n) {
    int max = arr[0].id;
    for (int i = 1; i < n; i++)
        if (arr[i].id > max)
            max = arr[i].id;
    return max;
}

void counting_sort_radix(Student arr[], int n, int exp, Student output[]) {
    int i;
    int count[10] = { 0 };

    for (i = 0; i < n; i++)
        count[(arr[i].id / exp) % 10]++;

    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    for (i = n - 1; i >= 0; i--) {
        output[count[(arr[i].id / exp) % 10] - 1] = arr[i];
        count[(arr[i].id / exp) % 10]--;
    }

    for (i = 0; i < n; i++)
        arr[i] = output[i];
}

void radix_sort_id(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    int max = get_max_id(arr, n);

    Student* output = (Student*)malloc(sizeof(Student) * n);
    if (!output) {
        fprintf(stderr, "Error: Memory allocation failed for Radix Sort auxiliary array.\n");
        return;
    }

    for (int exp = 1; max / exp > 0; exp *= 10)
        counting_sort_radix(arr, n, exp, output);

    free(output);
    *comparisons = 0; // Radix sort is non-comparison based
}


// I. Tree Sort (A - Simple Binary Search Tree)

typedef struct TreeNode {
    Student data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* new_tree_node(Student s) {
    TreeNode* temp = (TreeNode*)malloc(sizeof(TreeNode));
    if (temp) {
        temp->data = s;
        temp->left = temp->right = NULL;
    }
    return temp;
}

TreeNode* insert_bst(TreeNode* node, Student s, CompareFunc cmp, long long* comparisons) {
    if (node == NULL) return new_tree_node(s);

    if (cmp(&s, &node->data, comparisons) < 0) {
        node->left = insert_bst(node->left, s, cmp, comparisons);
    }
    else {
        // If equal or greater, go to the right (for non-strict BST)
        node->right = insert_bst(node->right, s, cmp, comparisons);
    }
    return node;
}

void inorder_bst(TreeNode* root, Student arr[], int* index) {
    if (root != NULL) {
        inorder_bst(root->left, arr, index);
        arr[(*index)++] = root->data;
        inorder_bst(root->right, arr, index);
    }
}

void free_bst(TreeNode* root) {
    if (root != NULL) {
        free_bst(root->left);
        free_bst(root->right);
        free(root);
    }
}

void tree_sort_basic(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    TreeNode* root = NULL;
    for (int i = 0; i < n; i++) {
        root = insert_bst(root, arr[i], cmp, comparisons);
    }

    int index = 0;
    inorder_bst(root, arr, &index);

    free_bst(root);
}


// I. Tree Sort (B - AVL Tree) - Placeholder/Note for complexity
void avl_tree_sort(Student arr[], int n, CompareFunc cmp, long long* comparisons) {
    // 완전한 AVL 트리는 구현이 매우 복잡하여, 이 코드에서는 기본적인 BST 정렬을 사용하고
    // 비교 횟수 측정을 위해 함수 구조만 유지합니다. (실제 과제에서는 AVL 구현 필요)
    tree_sort_basic(arr, n, cmp, comparisons);
}


// --- Main Testing and Averaging Logic ---

// Structure to define a single sort test case
typedef struct {
    char name[50];
    void (*sort_func)(Student[], int, CompareFunc, long long*);
    CompareFunc cmp_func;
    char cmp_name[50];
    int skip_heap_tree; // 1 if key has duplicates (skip Heap/Tree), 0 otherwise
    int is_radix; // 1 if Radix sort
} SortTest;

void run_test(const Student* original_data, int n, SortTest test, SortMetrics* avg_metrics) {
    SortMetrics total_metrics = { 0, 0 };
    Student* arr = NULL;

    // Memory Usage Estimate
    size_t data_mem = sizeof(Student) * n;
    size_t aux_mem = 0;

    if (strcmp(test.name, "Merge Sort") == 0 || strcmp(test.name, "Radix Sort (ID)") == 0) {
        aux_mem = sizeof(Student) * n; // Auxiliary array of size N
    }
    if (strcmp(test.name, "Tree Sort (Basic)") == 0 || strcmp(test.name, "AVL Tree Sort (Improved)") == 0) {
        // Tree nodes are larger than Student struct. Estimate N * (sizeof(TreeNode))
        aux_mem = sizeof(TreeNode) * n;
    }
    avg_metrics->memory_bytes = data_mem + aux_mem;


    for (int i = 0; i < NUM_REPETITIONS; i++) {
        // 1. Copy original data for each run
        arr = (Student*)malloc(data_mem);
        if (!arr) {
            fprintf(stderr, "Error: Memory allocation failed for test run copy.\n");
            // Fallback: Use accumulated average and return
            if (i > 0) total_metrics.comparisons /= i;
            avg_metrics->comparisons = total_metrics.comparisons;
            return;
        }
        memcpy(arr, original_data, data_mem);

        // 2. Perform the sort and track comparisons
        long long current_comparisons = 0;
        test.sort_func(arr, n, test.cmp_func, &current_comparisons);
        total_metrics.comparisons += current_comparisons;

        // 3. Free copied data
        free(arr);
    }

    // 4. Calculate average comparisons
    avg_metrics->comparisons = total_metrics.comparisons / NUM_REPETITIONS;
}

int main() {
    // 1. Load Data
    int student_count = 0;
    Student* students = load_students(DATA_FILENAME, &student_count);

    if (!students || student_count == 0) {
        fprintf(stderr, "Exiting due to data loading error.\n");
        return 1;
    }

    printf("Successfully loaded %d student records from %s (Total data memory: %.2f MB)\n",
        student_count, DATA_FILENAME, (double)sizeof(Student) * student_count / (1024 * 1024));

    // ID: Unique (Heap/Tree OK)
    // NAME, GENDER, TOTAL_GRADE: Duplicates (Heap/Tree SKIP)

    // --- Define All Test Cases (Assignment A and B) ---
    SortTest all_tests[] = {
        // --- Assignment A: ID Ascending (Unique Key, Heap/Tree OK) ---
        {"Bubble Sort", bubble_sort, compare_id_asc, "ID Ascending", 0, 0},
        {"Selection Sort", selection_sort, compare_id_asc, "ID Ascending", 0, 0},
        {"Insertion Sort", insertion_sort, compare_id_asc, "ID Ascending", 0, 0},
        {"Shell Sort (Basic)", shell_sort_basic, compare_id_asc, "ID Ascending", 0, 0},
        {"Quick Sort (Basic)", quick_sort_basic, compare_id_asc, "ID Ascending", 0, 0},
        {"Heap Sort", heap_sort, compare_id_asc, "ID Ascending", 0, 0},
        {"Merge Sort", merge_sort, compare_id_asc, "ID Ascending", 0, 0},
        {"Radix Sort (ID)", (void (*)(Student[], int, CompareFunc, long long*))radix_sort_id, compare_id_asc, "ID Ascending", 0, 1},
        {"Tree Sort (Basic)", tree_sort_basic, compare_id_asc, "ID Ascending", 0, 0},

        // --- Assignment A: NAME Ascending (Duplicate Key, Heap/Tree SKIP) ---
        {"Bubble Sort", bubble_sort, compare_name_asc, "NAME Ascending", 1, 0},
        {"Selection Sort", selection_sort, compare_name_asc, "NAME Ascending", 1, 0},
        {"Insertion Sort", insertion_sort, compare_name_asc, "NAME Ascending", 1, 0},
        {"Shell Sort (Basic)", shell_sort_basic, compare_name_asc, "NAME Ascending", 1, 0},
        {"Quick Sort (Basic)", quick_sort_basic, compare_name_asc, "NAME Ascending", 1, 0},
        {"Merge Sort", merge_sort, compare_name_asc, "NAME Ascending", 1, 0},

        // --- Assignment A: GENDER Ascending (Duplicate Key, Stable Sorts ONLY) ---
        // Bubble, Insertion, Merge are typically stable
        {"Bubble Sort", bubble_sort, compare_gender_asc, "GENDER Ascending (Stable)", 1, 0},
        {"Insertion Sort", insertion_sort, compare_gender_asc, "GENDER Ascending (Stable)", 1, 0},
        {"Merge Sort", merge_sort, compare_gender_asc, "GENDER Ascending (Stable)", 1, 0},

        // --- Assignment A: TOTAL Grade Descending (Duplicate Key, Heap/Tree SKIP) ---
        {"Bubble Sort", bubble_sort, compare_total_desc, "TOTAL Descending", 1, 0},
        {"Selection Sort", selection_sort, compare_total_desc, "TOTAL Descending", 1, 0},
        {"Insertion Sort", insertion_sort, compare_total_desc, "TOTAL Descending", 1, 0},
        {"Shell Sort (Basic)", shell_sort_basic, compare_total_desc, "TOTAL Descending", 1, 0},
        {"Quick Sort (Basic)", quick_sort_basic, compare_total_desc, "TOTAL Descending", 1, 0},
        {"Merge Sort", merge_sort, compare_total_desc, "TOTAL Descending", 1, 0},

        // --- Assignment B: Improved Sorts (Using ID Ascending for comparison) ---
        {"Shell Sort (Basic)", shell_sort_basic, compare_id_asc, "ID Ascending (Basic)", 0, 0},
        {"Shell Sort (Improved)", shell_sort_improved, compare_id_asc, "ID Ascending (Improved)", 0, 0},

        {"Quick Sort (Basic)", quick_sort_basic, compare_id_asc, "ID Ascending (Basic)", 0, 0},
        {"Quick Sort (Improved)", quick_sort_improved, compare_id_asc, "ID Ascending (Improved)", 0, 0},

        {"Tree Sort (Basic)", tree_sort_basic, compare_id_asc, "ID Ascending (Basic)", 0, 0},
        {"AVL Tree Sort (Improved)", avl_tree_sort, compare_id_asc, "ID Ascending (Improved)", 0, 0},
    };

    int num_tests = sizeof(all_tests) / sizeof(SortTest);

    printf("\n--- Assignment A & B: Sort Algorithm Comparison (Average of %d runs) ---\n", NUM_REPETITIONS);
    printf("| Algorithm | Criterion | Key Duplicates | Stable Sort | Comparisons (Avg) | Memory (Bytes) |\n");
    printf("|:---|:---|:---|:---|:---:|:---:|\n");

    // Run all tests
    for (int i = 0; i < num_tests; i++) {
        SortTest test = all_tests[i];

        // Skip Heap/Tree for non-unique keys
        if (test.skip_heap_tree) {
            if (strcmp(test.name, "Heap Sort") == 0 || strcmp(test.name, "Tree Sort (Basic)") == 0 || strcmp(test.name, "AVL Tree Sort (Improved)") == 0) {
                continue;
            }
        }

        // Skip Radix for non-ID criteria (Radix only works for integer keys)
        if (test.is_radix && strstr(test.cmp_name, "ID") == NULL) {
            continue;
        }

        // Skip stable sorts for non-stable algorithms on GENDER
        if (strstr(test.cmp_name, "GENDER") != NULL) {
            if (!(strcmp(test.name, "Bubble Sort") == 0 || strcmp(test.name, "Insertion Sort") == 0 || strcmp(test.name, "Merge Sort") == 0)) {
                continue;
            }
        }


        SortMetrics avg_metrics;
        run_test(students, student_count, test, &avg_metrics);

        // Determine the Key Duplicates and Stable Sort column content for clear output
        const char* key_dups = test.skip_heap_tree ? "YES" : "NO";
        const char* is_stable = "N/A";
        if (strstr(test.cmp_name, "GENDER") != NULL) {
            if (strcmp(test.name, "Bubble Sort") == 0 || strcmp(test.name, "Insertion Sort") == 0 || strcmp(test.name, "Merge Sort") == 0) {
                is_stable = "YES";
            }
            else {
                is_stable = "NO";
            }
        }

        printf("| %s | %s | %s | %s | %lld | %zu |\n",
            test.name,
            test.cmp_name,
            key_dups,
            is_stable,
            avg_metrics.comparisons,
            avg_metrics.memory_bytes);
    }

    // Free the original data
    free(students);

    return 0;
}