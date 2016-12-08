#ifndef _DArray_h
#define _DArray_h

// A pointer to a specific index of a 2D array.
typedef void* Array2DIndexPtr;

// Used to store a 2DArray and its dimensions. Dimensions are stored as inclusive (ie array[rows][columns] is okay) for ease of programming.
typedef struct array_head {
   int rows;
   int columns;
   Array2DIndexPtr head;
} Array2DHead;

// A pointer to an Array2D. Stores a pointer to the first index of the array as well as the array's dimensions.
typedef struct array_head *Array2D;

// A pointer to the payload of an array. Each index in the 2D array is an Array2DPayload_t.
typedef void* Array2DPayload_t;

// A payload free function. Used to free the payload of an array.
typedef void(*Array2DPayloadFreeFnPtr)(Array2DPayload_t payload);

// Prints the dimensions of the given array, as well as its contents
void printArray(Array2D array);

// Returns the payload of a 2D array at (x, y). Returns NULL if array is null or (x,y) is not a valid index for array.
Array2DPayload_t getArray2D(Array2D array, int x, int y);

// Swaps the values at (x1, y1) and (x2, y2) in array. Returns 1 if array is null, 2 if either index is out of bounds, and 0 otherwise.
int swapArray2D(Array2D array, int x1, int y1, int x2, int y2);

// Sets the value at (x, y) to value in array. Returns 1 if array is null, 2 if the index (x,y) is out of bounds, and 0 otherwise.
int setArray2D(Array2D array, void* value, int x, int y);

// Allocates a new Array2D with dimensions x-by-y. Returns NULL if there is not enough memory to allocate the 2D array.
Array2D allocateArray2D(int x, int y);

// Deep-copies all values in an Array2D containing only ints. Returns NULL if other is null. Behavior is undefined if other doesn't contain ints.
Array2D copyArray2D(Array2D other);

Array2D notDeepCopyArray2D(Array2D other);

// Frees the Array2D array, does nothing if array is null. Payload_free_function can be passed to free all the payloads in the array. Not used if NULL.
void freeArray2D(Array2D array, Array2DPayloadFreeFnPtr payload_free_function);

Array2D deseralizeInt2DArray(char* location, int** pointers);

int serializeInt2DArray(Array2D array, const char* location);

#endif /* _DArray_h */

