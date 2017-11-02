#include "queue.h"
#include <runLength.c>
#include <pthread.h>





//used for pushing onto queue
typedef struct{
  char* line;
  size_t lineNo;
} fileLine;

pthread_mutex_t lock;


queue* queue;
vector* results;

void* encodeLine(){
  fileLine* data;
  while((data = queue_pull(queue))){
    size_t lineNum = data->lineNo;
    char* encodedLine = encode(data->line);
    vector_set(results, lineNum, encodeLine);
    free(data->line);
    free(data);
  }
  queue_push(NULL);
  return NULL;
}




int main(int argc, char**argv){
  if(argc < 2){
    fprintf(stderr, "usage compress: %s input_file outout_file [num_threads]\n",argv[0]);
    return 0;
  }


  queue = queue_create(-1);
  results = string_vector_create();
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
  pthread_t threads[num_threads];
  for(i = 0; i < num_threads; ++i){
    int t = pthread_create(threads + i, 0, encodeLine, 0);
    if(!t){
      fprintf(stderr, "Count not create thread\n");
      exit(1);
    }
  }

  FILE*in = fopen(argv[1], "r");
  size_t bytesRead;
  char* line = NULL;
  size_t n = 0;
  size_t lineNum = 0;
  while((bytesRead = getline(in, line, n)) != -1){
    fileLine* temp = malloc(sizeof(fileLine));
    temp->lineNo = lineNum++;
    temp->line = strdup(line);
    queue_push(queue, temp);
  }
  free(line);
  queue_push(NULL);



  for(i = 0;i < num_threads; ++i){
    pthread_join(threads[i], 0);
  }

  FILE* out = fopen(argv[2],"w");
  for(i = 0; i < lineNum; ++i){
    char* resLine = vector_get(results, i);
    fprintf(out, "%s\n", resLine);
  }


  //TODO clean up memory












}
