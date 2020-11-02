#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

static double read_timer() {

    static bool initialized = false;

    static struct timeval start;

    struct timeval end;

    if( !initialized ){

        gettimeofday( &start, NULL );

        initialized = true;
    }

    gettimeofday( &end, NULL );

    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

#define ARRAY_MAX_SIZE 10000

int arr[ARRAY_MAX_SIZE];

static void swap(int* a, int* b){

  int t = *a;

  *a = *b;

  *b = t;
}

static int partition (int *a, int l, int h){

  int pivot = a[h];

  int i = (l - 1);

  for (int j = l; j <= h - 1; j++) if (a[j] <= pivot) swap(&a[i++], &a[j]);

	swap(&a[i+1], &a[h]);

  return (i + 1);
}

static void quickSort(int *a, int l, int h){

  if (l < h){

    int pi = partition(a,l,h);

    //define a task that can run in parallel with the code
    //outside the task region so the values passed wont
    //be garbage and every task claims its own copy
		#pragma omp task firstprivate(a,l,pi)
    {
			quickSort(a,l, pi - 1);
		}
		{
			quickSort(a, pi + 1,h);
		}
	}
}

int main(){

	double sTime, rTime;

	for( int i = 0; i < ARRAY_MAX_SIZE-1; i++ ) arr[i] = rand() % 50 +1;

  int n = sizeof(arr)/sizeof(arr[0]);

  //omp_set_num_threads(8);

  sTime = read_timer();

  #pragma omp parallel
  {

    printf("i am parallelised\n");

  //int id = omp_get_thread_num();

  //int nthrds = omp_get_num_threads();

  //execute quicksort once by a single thread, nowait instructs the compiler
  //to make the rest of the threads execute the code after the block,
  //so essentially it avoids the implicit barrier
  //(the rest of the threads will go at qsort and execute the code in parallel)
  #pragma omp single nowait

  quickSort(arr, 0, n-1);

  }

	rTime = read_timer() - sTime;

  printf("%lf\n",rTime );

	return 0;
}
