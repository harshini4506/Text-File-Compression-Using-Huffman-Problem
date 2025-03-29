#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100

struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

struct MinHeap* buildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;

    for (int i = (minHeap->size - 2) / 2; i >= 0; --i)
        minHeapify(minHeap, i);

    return minHeap;
}

struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = buildMinHeap(data, freq, size);

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }

    return extractMin(minHeap);
}

void printCodes(struct MinHeapNode* root, int arr[], int top, FILE* outputFile) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1, outputFile);
    }

    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1, outputFile);
    }

    if (!root->left && !root->right) {
        fprintf(outputFile, "%c:", root->data);
        for (int i = 0; i < top; ++i)
            fprintf(outputFile, "%d", arr[i]);
        fprintf(outputFile, "\n");
    }
}

void HuffmanCodes(char data[], int freq[], int size, FILE* outputFile) {
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
    int arr[MAX_TREE_HT], top = 0;
    printCodes(root, arr, top, outputFile);
}

void compressFile(const char* inputFile) {
    FILE *file = fopen(inputFile, "r");
    if (!file) {
        printf("Error: Could not open file.\n");
        return;
    }

    int freq[256] = {0};
    char ch;
    int originalSize = 0;

    while ((ch = fgetc(file)) != EOF) {
        freq[(unsigned char)ch]++;
        originalSize += 8;
    }
    fclose(file);

    int uniqueChars = 0;
    for (int i = 0; i < 256; i++)
        if (freq[i] > 0)
            uniqueChars++;

    char* data = (char*)malloc(uniqueChars * sizeof(char));
    int* frequencies = (int*)malloc(uniqueChars * sizeof(int));

    int index = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            data[index] = (char)i;
            frequencies[index] = freq[i];
            index++;
        }
    }

    FILE* outputFile = fopen("compressed.txt", "w");
    HuffmanCodes(data, frequencies, uniqueChars, outputFile);
    fclose(outputFile);

    file = fopen("compressed.txt", "r");
    fseek(file, 0, SEEK_END);
    int compressedSize = ftell(file) * 8;
    fclose(file);

    double compressionRatio = ((double)(originalSize - compressedSize) / originalSize) * 100;
    printf("\nCompression Done! Output saved to 'compressed.txt'.\n");
    printf("Original Size: %d bits\n", originalSize);
    printf("Compressed Size: %d bits\n", compressedSize);
    printf("Compression Ratio: %.2f%%\n", compressionRatio);

    free(data);
    free(frequencies);
}

int main() {
    char inputFile[100];
    printf("Enter input file name: ");
    scanf("%s", inputFile);
    compressFile(inputFile);
    return 0;
}