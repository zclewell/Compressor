#include "queue.h"
#include <runLength.c>
#include <pthread.h>




queue* queue;

void* encodeLine(void* line){




}




int main(int argc, char**argv){
  if(argc < 2){
    fprintf(stderr, "usage: mainRunLength input_file outout_file [num_threads]\n", );
    return 0;
  }


  queue = queue_create(-1);
  size_t num_threads = 0;
  if(argc >= 3){
    num_threads = atoi(argv[2]);
  }

  FILE* file = fopen(argv[1], "r");
  if(!file){
    printf("file does not exist\n");
    return 0;
  }

  size_t i;
  pthrad_t threads[num_threads];
  for(i = 0; i < num_threads; ++i){
    int* threadNo = malloc(sizeof(int));
    *threadNo = i;
    int t = pthrad_create(threads + i, 0, encodeLine, 0);
    if(!t){
      fprintf(stderr, "Count not create thread\n");
      exit(1);
    }
  }









}
