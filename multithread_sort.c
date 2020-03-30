#include <stdio.h>
#include <string.h>
#include  <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include "multithread_sort.h"
#include <sys/time.h>

bool StartsWith(const char * a,
  const char * b) {
  if (strncmp(a, b, strlen(b)) == 0) return 1;
  return 0;
}

void WrongSyntaxError() {
  printf("Less arguments than expected.\n");
  printf("The correct synatx is -numbers=X -threads=Y -seed=Z -mode=I\n");
  printf("Terminating...\n");
}

char * substring(char * string, int position, int length) {
  char * pointer;
  int c;
  pointer = malloc(length + 1);
  if (pointer == NULL) {
    printf("Unable to allocate memory.\n");
    exit(1);
  }

  for (c = 0; c < length; c++) {
    *(pointer + c) = * (string + position - 1);
    string++;
  }

  *(pointer + c) = '\0';
  return pointer;
}
void init_array(int arr[], int n) {
  int i;
  for (i = 0; i < n; i++) {
    arr[i] = 0;
  }
}

void merge(int i, int j) {
  int mid = (i + j) / 2;
  int ai = i;
  int bi = mid + 1;

  int newa[j - i + 1], newai = 0;

  while (ai <= mid && bi <= j) {
    if (array[ai] > array[bi])
      newa[newai++] = array[bi++];
    else
      newa[newai++] = array[ai++];
  }

  while (ai <= mid) {
    newa[newai++] = array[ai++];
  }

  while (bi <= j) {
    newa[newai++] = array[bi++];
  }

  for (ai = 0; ai < (j - i + 1); ai++)
    array[i + ai] = newa[ai];
}

void * Sort(void * argp) {
  struct thread_args * args = argp;

  int id = args -> id;
  int start = args -> a;
  int stop = args -> b;
  printf("Sorting %d to %d\n", start, stop);

  int c;
  int d;
  int n = stop - start;

  //bubble sort
  for (c = start; c < stop; c++) {
    if (start > 0) {
      for (d = start; d < stop - c + start; d++) {
        if (array[d] > array[d + 1]) /* For decreasing order use < */ {
          int swap = array[d];
          array[d] = array[d + 1];
          array[d + 1] = swap;
        }
      }
    } else {
      for (d = start; d < stop - c; d++) {
        if (array[d] > array[d + 1]) /* For decreasing order use < */ {
          int swap = array[d];
          array[d] = array[d + 1];
          array[d + 1] = swap;
        }
      }
    }
  }
  pthread_mutex_unlock( & mutex);
  fprintf(fp, "Started thread #%i  for sorting cells %i to %i\n", id, start, stop);
  int k = 1;
  for (int j = start; j <= stop; j++) {
    fprintf(fp, "%i) %i\n", k, array[j]);
    k++;
  }
  //free(args);
  pthread_exit(NULL);
}

int main(int argc, char ** argv) {
  if (argc < 5) {
    WrongSyntaxError();
    return 0;
  } else {
    fp = fopen("results.dat", "ab+");
    struct timeval start, end;
    gettimeofday( & start, NULL);
    //CORRECT SYNTAΧ CONTINUING
    if (StartsWith(argv[1], "-numbers=") && StartsWith(argv[2], "-threads=") &&
      StartsWith(argv[3], "-seed=") && StartsWith(argv[4], "-mode=")) {
      char * numbers = substring(argv[1], 10, strlen("-numbers="));
      char * threads = substring(argv[2], 10, strlen("-threads="));
      char * seed = substring(argv[3], 7, strlen("-seed="));
      char * mode = substring(argv[4], 7, strlen("-mode="));
      int threadsNum = atoi(threads);
      if (threadsNum == 2 || threadsNum == 4 || threadsNum == 8) {
        int numbersNum = atoi(numbers);
        int seedNum = atoi(seed);
        int modeNum = atoi(mode);
        array = malloc(numbersNum * sizeof(int));
        init_array(array, numbersNum);
        srand(seedNum);
        for (int i = 0; i < numbersNum; i++) {
          array[i] = rand();
        }
        if (fp == NULL) {
          printf("error opening file... Terminating.\n");
          exit(0);
        }
        fprintf(fp, "{");
        for (int i = 0; i < numbersNum; i++) {
          if (i == numbersNum - 1) {
            fprintf(fp, "%i", array[i]);
          } else {
            fprintf(fp, "%i, ", array[i]);
          }
        }
        fprintf(fp, "}\n");
        threadsArray = malloc(sizeof(pthread_t) * threadsNum);
        int threadSuccess;
        long i;

        long count = 0;
        for (int j = 0; j < numbersNum; j += floor(numbersNum / threadsNum)) {
          struct thread_args * args = malloc(sizeof * args);
          if (args != NULL) {
            args ->id = count;
            args ->a = j;
            args ->b = j + floor(numbersNum / threadsNum) - 1;
          }
          pthread_mutex_lock( & mutex);
          threadSuccess = pthread_create( & threadsArray[count], NULL, Sort, args);
          if (threadSuccess) {
            printf("ERROR; return code from pthread_create() is %d\n", threadSuccess);
            exit(-1);
          }
          count++;
        }
        for (int k = 0; k < threadsNum; k++) {
          pthread_join(threadsArray[k], NULL);
        }
        //merge step
        merge(0, numbersNum / 2);
        merge(numbersNum / 2, numbersNum - 1);
        merge(0, numbersNum - 1);

        fprintf(fp, "Sorted array:\n");
        fprintf(fp, "{ ");
        for (int i = 0; i < numbersNum; i++) {
          if (i == numbersNum - 1) {
            fprintf(fp, "%i", array[i]);
          } else {
            fprintf(fp, "%i, ", array[i]);
          }
        }
        fprintf(fp, "}\n");
        gettimeofday( & end, NULL);
        fprintf(fp, "Merge sort completed after: %ld nanoseconds\n ", ((end.tv_sec - start.tv_sec) * 1000000L +
          end.tv_usec) - start.tv_usec);
        printf("Merge sort completed after: %ld nanoseconds\n ", ((end.tv_sec - start.tv_sec) * 1000000L +
          end.tv_usec) - start.tv_usec);
        fclose(fp);
      } else {
        printf("Wrong threads number. Threads must ne 2, 4 or 8\n");
        printf("Terminating...\n");
        return 0;
      }
    } else {
      WrongSyntaxError();
      return 0;
    }
  }
  pthread_exit(NULL);
  return 0;
}