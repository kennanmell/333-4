#include "Array2D.h"
#include <stdlib.h>
#include <stdio.h>
#include <jansson.h>

#define PtrSize sizeof(Array2DIndexPtr)

// Prints a 2D array, including its dimensions.
void printArray(Array2D array) {
   printf("Array is %d x %d\n", array->rows + 1, array->columns + 1);
   for (int i = 0; i <= array->rows; i++) {
      for(int j = 0; j <= array->columns; j++) {
         int* p = getArray2D(array, j, i);
         printf("%d ", *p);
      }
   }
   printf("\n");
}

// Serializes a 2DArray. Array is the array to serialize.
// Location is the file path of the file to write the serialized array to.
int serializeInt2DArray(Array2D array, const char* location) {
   json_t* out = json_object();
   json_t* jArr = json_array();
   for (int i = 0; i <= array->rows; i++) {
      for (int j = 0; j <= array->columns; j++) {
         int* curr = (int*) getArray2D(array,j, i);
         json_array_append_new(jArr, json_integer(*curr));
      }
   }

   json_object_set_new(out, "rows", json_integer(array->rows + 1));
   json_object_set_new(out, "columns", json_integer(array->columns + 1));
   json_object_set_new(out, "data", jArr);
  
   int result = json_dump_file(out, location, 0);
   json_decref(out);
   json_decref(jArr);
   return result;
}

// Deseralizes and returns a 2D int array using jansson.
// Location is the file path of the array to deseralize.
// Pointers is set by this function. It contains the pointers stored in the array and much be freed when the array is freed.
Array2D deseralizeInt2DArray(char* location, int** pointers) {
    json_error_t error;
    json_t* json = json_load_file(location, 0, &error);

    if(!json) {
        return NULL;
    }

    json_t* jRows = json_object_get(json, "rows");

   if (!json_is_integer(jRows)) {
      return NULL;
   }
   int rows = json_integer_value(jRows);

   json_t* jColumns = json_object_get(json, "columns");
   if (!json_is_integer(jColumns)) {
      return NULL;
   }

   int columns = json_integer_value(jColumns);

   Array2D array = allocateArray2D(rows, columns);   

   json_t* jData = json_object_get(json, "data");
   if (!json_is_array(jData)) {
      return NULL;
   }

   int arraySize = json_array_size(jData);
   int* arr = malloc(arraySize);
   *pointers = arr;

   for (int i = 0; i < arraySize; i++) {
      arr[i] = json_integer_value(json_array_get(jData, i));
      setArray2D(array, &arr[i], i % columns, i / columns);
   }
   
   json_decref(json);
   return array;
}

Array2DPayload_t getArray2D(Array2D array, int x, int y) {
    if (array == NULL || x > array->rows + 1 || y > array->columns + 1 || x < 0 || y < 0) {
        return (Array2DPayload_t) NULL;
    }
    long* returnVal = (long*) array->head + y * (array->columns + 1) + x; //might not work
    //long* returnVal = array->head + y * (array->columns + 1) * PtrSize + x * PtrSize; //works but has warnings
    return (Array2DPayload_t)(*returnVal);

}

int swapArray2D(Array2D array, int x1, int y1, int x2, int y2) {
    if (array == NULL) {
        return 1;
    }
    
    if (x1 < 0 || y1 < 0 || x1 > array->rows + 1 || y1 > array->columns + 1 || x2 < 0 || y2 < 0 || x2 > array->rows + 1 || y2 >= array->columns + 1) {
        return 2;
    }

    Array2DPayload_t p = getArray2D(array, x1, y1);
    setArray2D(array, getArray2D(array, x2, y2), x1, y1);
    setArray2D(array, p, x2, y2);
    return 0;
}

int setArray2D(Array2D array, Array2DPayload_t value, int x, int y) { 
   if (array == NULL) {
        return 1;
    }
    
    if (x < 0 || y < 0 || x > array->rows + 1 || y > array->columns + 1) {
        return 2;
    }

    long* index = (long*) array->head + y * (array->columns + 1) + x; //cast to ptr so no * ptr
    *index = (long) value;
       
    return 0;
}

Array2D allocateArray2D(int x, int y) {
    if (x < 1 || y < 1) {
        return (Array2D) NULL;
    }
    
    Array2D array = (Array2D) malloc(sizeof(Array2DHead));
    if (array == NULL) {
        return (Array2D) NULL;
    }
    
    array->rows = x - 1;
    array->columns = y - 1;
    
    Array2DIndexPtr head = (Array2DIndexPtr) malloc(PtrSize * x * y);
    if (head == NULL) {
        return (Array2D) NULL;
    }
    array->head = head;
    
    return array;
}

void freeArray2D(Array2D array, Array2DPayloadFreeFnPtr payload_free_function) {
    if (array == NULL) {
        return;
    }
    
    //might not work
    for (Array2DIndexPtr x = array->head; payload_free_function != NULL && (long*) x < (long*) array->head + array->rows * array->columns; x = (long*) x + 1) {
        payload_free_function(x);
    }

    /*
    for (Array2DIndexPtr x = array->head; payload_free_function != NULL && x < array->head + array->rows * array->columns * PtrSize; x += PtrSize) {
        payload_free_function(x);
    } */ //definitely works but has warnings
    
    free(array->head);
    free(array);
}

int main(int argc, char **argv) {
   if (argc != 2) {
	return 1;
   } 
   
   int* pointers;
   Array2D array = deseralizeInt2DArray(argv[1], &pointers);

   if (array == NULL) {
      return 2;
   }
   
   printf("[1] Deserialize %s\n", argv[1]);
   for (int i = 0; i <= array->rows; i++) {
      for(int j = 0; j <= array->columns; j++) {
         int* p = getArray2D(array, j, i);
         printf("    		  deserialized %d\n", *p);
      }
   }
   
   printArray(array);

   printf("\n\n");
   printf("[2] Set [1][1] to 100\n");
   int hundred = 100;
   setArray2D(array, &hundred, 1, 1);
   printArray(array);
   
   printf("\n\n");
   printf("[3] swap [1][1] and [0][0]\n");
   swapArray2D(array, 1, 1, 0, 0);
   printArray(array);   

   printf("\n\n");
   printf("[4] swap [1][1] and [10][10]\n");
   swapArray2D(array, 1, 1, 10, 10);
   printArray(array);

   printf("\n\n");
   printf("[5] serialize array to file json.out\n");
   serializeInt2DArray(array, "json.out");
   for (int i = 0; i <= array->rows; i++) {
      for(int j = 0; j <= array->columns; j++) {
         int* p = getArray2D(array, j, i);
         printf("	      serialized %d\n", *p);
      }
   }

   printf("\n");
   printf("[6] destroy array\n");
   for (int i = 0; i <= array->rows; i++) {
      for(int j = 0; j <= array->columns; j++) {
         int* p = getArray2D(array, j, i);
         printf("	    destroyed %d\n", *p);
      }
   }

   freeArray2D(array, NULL);
   free(pointers);

   return 0;
}
