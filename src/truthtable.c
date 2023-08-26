#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int numInputs;
typedef enum
{
    AND,
    OR,
    NAND,
    NOR,
    XOR,
    NOT,
    PASS,
    DECODER,
    MULTIPLEXER
} gate_t;

struct gate {
    gate_t type;
    int size;
    int *inputs;
    int *outputs;
    struct gate *next;
};
typedef struct gate Gate;

struct node
{
    int val;     // Hash
    int truth;   // Truth Value
    int isInput; // 1 if is an input 0 else
    struct node *next;
};
typedef struct node IO;

struct LL {
    int val;
    struct LL *next;
};
typedef struct LL Node;

struct queue {
    Gate *g;
    struct queue *next;
    Node *missing;
    int executed;
};
typedef struct queue QueueNode;

Gate *toBeExecuted = NULL; 
IO *unsetNodes = NULL, *globalInputs = NULL, *globalOutputs = NULL;
QueueNode *queue = NULL;

void freeCircuit(Gate *head) {
    Gate *prev ;
    while (head != NULL) {
        prev = head;
        free(head->inputs);
        free(head->outputs);
        head = head->next;
        free(prev);
    }
}

void freeList(IO *head) {
    IO *prev;
    while (head != NULL) {
        prev = head;
        head = head->next;
        free(prev);
        prev = NULL;
    }
}

void freeNodes(Node *head) {
    Node *prev;
    while (head != NULL) {
        prev = head;
        head = head->next;
        free(prev);
        prev = NULL;
    }
}

void freeQueue(QueueNode *head) {
    QueueNode *prev;
    while (head != NULL) {
        prev = head;
        freeNodes(head->missing);
        head = head->next;
        free(prev);
        prev = NULL;
    }
}

int hash(char *str)
{
    if (str == NULL)
        printf("FOUND A NULL");
    int hash = 0;
    char *ptr = str;
    while (*ptr)
    {
        hash = 11 * hash + *ptr;
        ptr++;
    }
    return hash;
}

void insert(IO **head, int val, int isInput)
{
    if (*head == NULL)
    {
        *head = malloc(sizeof(IO));
        (*head)->val = val;
        (*head)->next = NULL;
        (*head)->truth = 0;
        (*head)->isInput = isInput;
    }
    else
    {
        IO *ptr = *head, *prev;
        while (ptr != NULL) {
            if (ptr->val == val)
                return;
            prev = ptr;
            ptr = ptr->next;
        }

        prev->next = malloc(sizeof(IO));
        prev->next->val = val;
        prev->next->truth = -1;
        prev->next->next = NULL;
        prev->next->isInput = isInput;
    }
}

void insertGate(Gate **head, Gate *next) {
    if (*head == NULL) {
        *head = next;
    }
    else {
        Gate *ptr = *head;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = next;
    }
}

void enqueue(QueueNode **head, Gate *g, Node *m) {
    if (*head == NULL) {
        *head = malloc(sizeof(QueueNode));
        (*head)->g = g;
        (*head)->next = NULL;
        (*head)->missing = m;
        (*head)->executed = 0;
    }
    else {
        QueueNode *ptr = *head, *prev;
        while (ptr != NULL) {
            prev = ptr;
            ptr = ptr->next;
        }
        prev->next = malloc(sizeof(QueueNode));
        prev->next->g = g;
        prev->next->missing = m;
        prev->next->executed = 0;
        prev->next->next = NULL;
    }
}

void dequeue(QueueNode **head, Gate *g) {
    if (*head == NULL)
        return;
    QueueNode *ptr = *head, *temp;
    if ((*head)->g == g) {
        *head = (*head)->next;
        freeNodes(ptr->missing);
        ptr = NULL;
        free(ptr);
        return;
    }
    while (ptr->next != NULL) {
        if (ptr->next->g == g) {
            temp = ptr->next;
            ptr->next = ptr->next->next;
            freeNodes(temp->missing);
            temp = NULL;
            free(temp);
            return;
        }
        temp = ptr;
        ptr = ptr->next;
    }
    if (ptr->g == g) {
        freeNodes(ptr->missing);
        temp->next = NULL;
        free(ptr);
    }

}

void insertMissing(Node **head, int val) {
    if (*head == NULL) {
        *head = malloc(sizeof(Node));
        (*head)->val = val;
        (*head)->next = NULL;
    }
    else {
        Node *ptr = *head, *prev;
        while (ptr != NULL) {
            prev = ptr;
            ptr = ptr->next;
        }
        prev->next = malloc(sizeof(Node));
        prev->next->val = val;
        prev->next->next = NULL;

    }
}

void setTruthValues(IO *head, int num)
{
    IO *ptr = head;
    int bit = numInputs - 1;
    while (ptr != NULL)
    {
        if (ptr->isInput != 0) {
            ptr->truth = (num >> bit--) & 1; 
        }
        ptr = ptr->next;
    }
}

void printRow(IO *inputs, IO *outputs)
{
    IO *ptr = inputs;
    while (ptr != NULL)
    {
        if (ptr->isInput == 1)
            printf("%d ", ptr->truth);
        ptr = ptr->next;
    }
    printf("| ");
    ptr = outputs;
    while (ptr != NULL)
    {
        printf("%d", ptr->truth);
        if (ptr->next != NULL)
            printf(" ");
        ptr = ptr->next;
    }
    printf("\n");
}

void printList(IO *head) {
    IO *ptr = head;
    while (ptr != NULL) {
        printf("%d", ptr->truth);
        ptr = ptr->next;
    }
    printf("\n");
}


IO *getNode(IO *first, IO *second, int val) {
    IO *ptr = first;
    while (ptr != NULL) {
        if (ptr->val == val)
            return ptr;
        ptr = ptr->next;
    }
    ptr = second;
    while (ptr != NULL) {
        if (ptr->val == val)
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}


void executeGate(Gate *ptr) {
    gate_t type;
    IO *out, *temp;
    int left, right, hsh, count;
    int size;
    int *localInputs;
    int *localOutputs;
    Node *missing = NULL;

    type = ptr->type;
    localInputs = ptr->inputs;
    localOutputs = ptr->outputs;
    size = ptr->size;
    count = type == MULTIPLEXER ? size + (1 << size) : size;
    

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (localInputs[i] == 0 || localInputs[i]== 1)
            continue;
        temp = getNode(globalInputs, NULL, localInputs[i]);
        if (temp == NULL)
            break;
        if (temp->truth == -1) {
            insertMissing(&missing, localInputs[i]);
            found = 1;
        }
    }
    if (found == 1) {
        enqueue(&queue, ptr, missing);
        return;
    }

    if (type >=0 && type <= 4) {
        left = localInputs[0];
        right = localInputs[1];
        hsh = localOutputs[0];
        out = getNode(globalOutputs, globalInputs, hsh);
        left = left > 1 ? getNode(globalInputs, NULL, left)->truth : left;
        right = right > 1 ? getNode(globalInputs, NULL, right)->truth : right;
        switch (type) {
            case 0:
                out->truth = left && right;
                break;
            case 1:
                out->truth = left || right;
                break;
            case 2:
                out->truth = !(left && right);
                break;
            case 3:
                out->truth = !(left || right);
                break;
            default:
                out->truth = (left && !right) || (!left && right);
                break;
        }
    }
    else if (type >= 5 && type <= 6) {
        left = localInputs[0];
        left = left > 1 ? getNode(globalInputs, NULL, left)->truth : left;
        hsh = localOutputs[0];
        out = getNode(globalOutputs, globalInputs, hsh);
        switch (type) {
            case 5:
                out->truth = !left;
                break;

            default:
                out->truth = left;
                break;
        }
    }
    else if (type == 7) {
        int *decoders = malloc(size * sizeof(int));
        for (int j = 0; j < size; j++) {
            left = localInputs[j];
            decoders[j] = left > 1 ? getNode(globalInputs, NULL, left)->truth : left;
        }
        for (int j = 0; j < (1 << size); j++) {
            left = localOutputs[j];
            if (left == -1)
                continue;
            out = getNode(globalOutputs, globalInputs, left);
            int temp = 1, bitStatus;
            for (int k = 0; k < size; k++) {
                bitStatus = (j >> (size - k - 1)) & 1;
                temp = temp && bitStatus == decoders[k];
            }
            out->truth = temp;
        }
        free(decoders);
    }
    else {
        int *multiplexers = malloc((1 << size) * sizeof(int));
        int j = 0;
        for (; j < (1 << size); j++) {
            left = localInputs[j]; // 0x1b59b251b40 "temp1"
            multiplexers[j] = left > 1 ? getNode(globalInputs, NULL, left)->truth : left; //116 't'
        }
        int bit = 0, temp = size-1;
        for (; j <(1 << size) + size; j++) {
            left = localInputs[j];
            left = left > 1 ? getNode(globalInputs, NULL, left)->truth : left;
            if (left == 1)
                bit |= 1 << temp;
            temp--;   
        }
        left = localOutputs[0];
        out = getNode(globalOutputs, globalInputs, left);
        out->truth = multiplexers[bit];
        free(multiplexers);
    }    
}

void moveNode(Gate **circuit, Gate *start, Gate *end) {
    Gate *ptr = *circuit, *prev;
    if (*circuit == start) {
        *circuit = (*circuit)->next;
    }
    else {
        while (ptr != NULL) {
            if (ptr == start) {
                prev->next = ptr->next;
                break;
            }
            prev = ptr;
            ptr = ptr->next;
        }
    }
    while (ptr != NULL) {
        if (ptr == end) {
            start->next = end;
            prev->next = start;
            return;
        }
        prev = ptr;
        ptr = ptr->next;
    }
    prev->next = start;
    prev->next->next = NULL;
    
}

int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
        return EXIT_FAILURE;
    
    char *word = malloc(17 * sizeof(char));
    int count;
    fscanf(file, "%16s %d", word, &count);
    globalInputs = NULL;
    globalOutputs = NULL;
    numInputs = count;

     // Create LL of Input Variable
    for (int i = 0; i < count; i++)
    {
        fscanf(file, "%16s", word);
        insert(&globalInputs, hash(word), 1);
    }

    // Create LL of Output Variable
    fscanf(file, "%16s %d", word, &count);
    for (int i = 0; i < count; i++)
    {
        fscanf(file, "%16s", word);
        insert(&globalOutputs, hash(word), 0);
    }
    char **line = malloc(3 * sizeof(char *));
    for (int i = 0; i < 3; i++)
    {
        line[i] = malloc(17 * sizeof(char));
    }

    Gate *circuit = NULL;
    int size, res;
    int *localInputs;
    int *localOutputs;
    gate_t type;

    // Read in circuits
    while (1) {
        res = fscanf(file, "%16s", word);
        if (res != 1)
            break;
        
        Gate *curr = malloc(sizeof(Gate));
        if (strcmp(word, "DECODER") == 0) {
            fscanf(file, "%d", &count);
            type = DECODER;
            localInputs = malloc(count * sizeof(int));
            localOutputs = malloc((1 << count) * sizeof(int));
            for (int i = 0; i < count; i++) {
                fscanf(file, "%16s", line[0]);
                localInputs[i] = !strcmp(line[0], "0") || !strcmp(line[0], "1") ? atoi(line[0]) : hash(line[0]);
                if (localInputs[i] > 1) {
                    insert(&globalInputs, localInputs[i], 0);
                }
                
            }
            for (int i = 0; i < (1 << count); i++) {
                fscanf(file, "%16s", line[0]);
                if (!strcmp(line[0], "_")) {
                    localOutputs[i] = -1;
                    continue;
                }
                localOutputs[i] = !strcmp(line[0], "0") || !strcmp(line[0], "1") ? atoi(line[0]) : hash(line[0]);
                if (localOutputs[i] > 1 ) {
                    if (getNode(globalOutputs, NULL, localOutputs[i]) == NULL)
                        insert(&globalInputs, localOutputs[i], 0);
                }
            }
            size = count;
        }
        else if (strcmp(word, "MULTIPLEXER") == 0) {
            fscanf(file, "%d", &count);
            type = MULTIPLEXER;
            size = count;
            localInputs = malloc(((1 << count) + count) * sizeof(int));
            localOutputs = malloc(sizeof(int));
            for (int i = 0; i < (1 << count) + count; i++) {
                fscanf(file, "%16s", line[0]);
                localInputs[i] = !strcmp(line[0], "0") || !strcmp(line[0], "1") ? atoi(line[0]) : hash(line[0]);
                if (localInputs[i] > 1) {
                    insert(&globalInputs, localInputs[i], 0);
                }
            }
            fscanf(file, "%16s", line[0]);
            localOutputs[0] = !strcmp(line[0], "0") || !strcmp(line[0], "1") ? atoi(line[0]) : hash(line[0]);
            if (localOutputs[0] > 1) {
                if (getNode(globalOutputs, NULL, localOutputs[0]) == NULL)
                    insert(&globalInputs, localOutputs[0], 0);
            }
        }

        else if (strcmp(word, "NOT") == 0 || strcmp(word, "PASS") == 0) {
            // NOT, PASS
            fscanf(file, "%16s %16s", line[0], line[1]);
            if (strcmp(word, "NOT") == 0) type = NOT;
            else type = PASS;

            localInputs = malloc(sizeof(int));
            localOutputs = malloc(sizeof(int));
            localInputs[0] = !strcmp(line[0], "0") || !strcmp(line[0], "1") ? atoi(line[0]) : hash(line[0]);
            if (localInputs[0] > 1) {
                insert(&globalInputs, localInputs[0], 0);
            }
            localOutputs[0] = !strcmp(line[1], "0") || !strcmp(line[1], "1") ? atoi(line[0]) : hash(line[1]);
            if (localOutputs[0] > 1) {
                if (getNode(globalOutputs, NULL, localOutputs[0]) == NULL)
                    insert(&globalInputs, localOutputs[0], 0);
            }
            size = 1;        
        }

        else {
            // AND, OR, NAND, NOR, XOR
            fscanf(file, "%16s %16s %16s", line[0], line[1], line[2]);
            if (strcmp(word, "AND") == 0) type = AND;
            else if (strcmp(word, "OR") == 0) type = OR;
            else if (strcmp(word, "NAND") == 0) type = NAND;
            else if (strcmp(word, "NOR") == 0) type = NOR;
            else if (strcmp(word, "XOR") == 0) type = XOR;

            localInputs = malloc(2 * sizeof(int));
            localOutputs = malloc(sizeof(int));
            localInputs[0] = !strcmp(line[0], "0") || !strcmp(line[0], "1") ? atoi(line[0]) : hash(line[0]);
            localInputs[1] = !strcmp(line[1], "0") || !strcmp(line[1], "1") ? atoi(line[1]) : hash(line[1]);
            for (int i =0 ; i < 2; i++) {
                if (localInputs[i] > 1) {
                    insert(&globalInputs, localInputs[i], 0);
                }
            }
            localOutputs[0] = !strcmp(line[2], "0") || !strcmp(line[2], "1") ? atoi(line[2]) : hash(line[2]);
            if (localOutputs[0] > 1) {
                if (getNode(globalOutputs, NULL, localOutputs[0]) == NULL)
                    insert(&globalInputs, localOutputs[0], 0);
            }
            size = 2;
        }
        curr->inputs = localInputs;
        curr->outputs = localOutputs;
        curr->type = type;
        curr->size = size;
        curr->next = NULL;
        insertGate(&circuit, curr);
    }
    Gate *ptr;
    QueueNode *queue_ptr;
    Node *temp;
    for (int i = 0; i < (1 << numInputs); i++) {
        setTruthValues(globalInputs, i);
        ptr = circuit;
        while (ptr != NULL) {
            if (queue != NULL) {
                queue_ptr = queue;
                while (queue_ptr != NULL) {
                    if (queue_ptr->executed == 1) {
                        queue_ptr = queue_ptr->next;
                        continue;
                    }
                    int ready = 1;
                    temp = queue_ptr->missing;
                    while (temp != NULL) {
                        if (getNode(globalInputs, NULL, temp->val)->truth == -1) {
                            ready = 0;
                            break;
                        }
                        temp = temp->next;
                    }
                    if (ready == 1) {
                        executeGate(queue_ptr->g);
                        queue_ptr->executed = 1;
                        moveNode(&circuit, queue_ptr->g, ptr);                 
                        break;
                    }
                    queue_ptr = queue_ptr->next;
                }
            }
            executeGate(ptr);
            ptr = ptr->next;
        }
        queue_ptr = queue;
        while (queue_ptr != NULL) {
            if (queue_ptr->executed == 1) {
                queue_ptr = queue_ptr->next;
                continue;
            }
            executeGate(queue_ptr->g);
            queue_ptr->executed = 1;
            moveNode(&circuit, queue_ptr->g, ptr);
            queue_ptr = queue_ptr->next;
        }
        printRow(globalInputs, globalOutputs); 
    }
    freeCircuit(circuit);
    freeList(globalInputs);
    freeList(globalOutputs);
    freeQueue(queue);
    for (int i = 0; i < 3; i++) {
        free(line[i]);
    }
    free(line);
    free(word);
    return EXIT_SUCCESS;  
}