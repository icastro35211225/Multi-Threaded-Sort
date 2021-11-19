#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#define MIN 1.0
#define MAX 100.00

typedef struct struct_t{
  double *arr1;
  double *arr2;
  double *merged;
}struct_t;

double *A, *B, *C, *half1, *half2;
int size;

void swap(double *x, double *y){
  double temp = *x;
  *x = *y;
  *y = temp;
}
double getRandom(double min, double max){
  double range = (MAX - MIN);
  double div = RAND_MAX / range;
  return MIN + (rand() / div);
}

void *selectionSort(void *arg){
  int i, j, minIndx;
  double *arr = (double*)arg;
  for(i = 0; i < size; i++){
    minIndx = i;
    for(j = i + 1; j < size; j++){
      if(arr[j] < arr[minIndx]){
        double temp = arr[minIndx];
        arr[minIndx] = arr[j];
        arr[j] = temp;
      }
    }
  }
  pthread_exit((void *)arr);
}
void *selectionSort1(void *arg){
  int i, j, minIndx;
  double *arr = (double*)arg;
  for(i = 0; i < size/2; i++){
    minIndx = i;
    for(j = i + 1; j < size/2; j++){
      if(arr[j] < arr[minIndx]){
        double temp = arr[minIndx];
        arr[minIndx] = arr[j];
        arr[j] = temp;
      }
    }
  }
  pthread_exit((void *)arr);
}

void *merge(void *arg) {
  double *merged;
  struct_t *p = (struct_t*)arg;
  int i = 0, j = 0, k = 0;
  merged = (double*)malloc(sizeof(double)*size);
    while(i < size/2 && j < size/2){
        if(half1[i] < half2[j]){
            merged[k++] = half1[i++];
        }else{
            merged[k++] = half2[j++];
        }
    }
    while(i < size/2){
        merged[k++] = half1[i++];
    }
    while(j < size/2){
        merged[k++] = half2[j++];
    }
    p->merged = merged;
  pthread_exit((void*)p);
}

int main(int argc, char *argv[]){
  struct timespec ts_begin, ts_end;
  double elapsed;
  srand(time(NULL));
  int n, i;
  pthread_t tid1, tid2, tid3, tid4;

  if(argc <= 1){
    printf("ERROR! Not enough command line args!\n");
    return -1;
  }

  n = atoi(argv[1]);
  size = n;

  A = (double*)malloc(sizeof(double)*n);
  B = (double*)malloc(sizeof(double)*n);
  C = (double*)malloc(sizeof(double)*n);
  half1 = (double*)malloc(sizeof(double)*(n/2));
  half2 = (double*)malloc(sizeof(double)*(n/2));
  
  for(i = 0; i < n; i++){
    A[i] = getRandom(MIN, MAX);
  }
  // ------------------ ONE THREAD CASE ---------------------
  // copy A into B
  for(i = 0; i < n; i++){
    B[i] = A[i];
  }
  clock_gettime(CLOCK_MONOTONIC, &ts_begin);
  // create thB sortThread to sort B use selection/insertion sort
  pthread_create(&tid1, NULL, selectionSort, (void **)B);
  pthread_join(tid1, (void **)&B);

  clock_gettime(CLOCK_MONOTONIC, &ts_end);
  elapsed = ts_end.tv_sec - ts_begin.tv_sec;
  elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;

  printf("Sorting by ONE thread is done in %.2lfms\n", elapsed * 1000);
  // ---------------- TWO THREADS CASE ----------------------
  // copy A into half1 and half2
  for(i = 0; i < n/2; i++){
    half1[i] = A[i];
  }
    int count = 0;
    for(i = n/2; i < n; i++){
        half2[count] = A[i];
        count++;
  }
  clock_gettime(CLOCK_MONOTONIC, &ts_begin);
  // create thA1 sortThread to sort half1
  pthread_create(&tid2, NULL, selectionSort1, (void **)half1);
  // create thA2 sortThread to sort half2
  pthread_create(&tid3, NULL, selectionSort1, (void **)half2);
  // join thA1
  pthread_join(tid2, (void **)&half1);
  // join thA2
  pthread_join(tid3, (void **)&half2);
  // create thM mergeThread to merge half1 and half2 into C
  struct_t *strc = (struct_t*)malloc(sizeof(struct_t));
  strc->arr1 = half1;
  strc->arr2 = half2;
  struct_t *p;
  pthread_create(&tid4, NULL, merge, (void**)strc);
  // join thM
  pthread_join(tid4, (void **)&p);
    C = p->merged;
  // cald thread time
  clock_gettime(CLOCK_MONOTONIC, &ts_end);
  elapsed = ts_end.tv_sec - ts_begin.tv_sec;
  elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;
  printf("Sorting by TWO threads is done by in %lfms\n", elapsed*1000);
  return 0;
}