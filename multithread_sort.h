#include <pthread.h>
int *array;
pthread_t *threadsArray;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
FILE *fp;
struct thread_args {
	int id;
    int a;
    int b;
};