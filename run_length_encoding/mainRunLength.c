#include "runLength.c"
#include <glib.h>
#include <stdlib.h>
#include <pthread.h>


//used for pushing onto queue
typedef struct{
  char* line;
  size_t lineNo;
} fileLine;


void fileLineDestructor(fileLine* fileLine){
  free(fileLine->line);
  free(fileLine);
}


typedef char* (*coder)(char*);

pthread_mutex_t lock;
GAsyncQueue* codingData;
GPtrArray* results;

// dummy variable pushed onto queue to tell threads to exit
// GAsyncQueue doesnt let you push NULL
fileLine finished[1];


void* encodeLine(void* arg){
  coder code = (coder)arg;
  fileLine* data;
  while((data = g_async_queue_pop(codingData)) != finished){
    size_t lineNum = data->lineNo;
    if (!strlen(data->line)){
      g_ptr_array_insert(results, lineNum, strdup("\n"));
      continue;
    }
    char* codedLine = code(data->line);
    pthread_mutex_lock(&lock);
    if(lineNum + 1 > results->len){
      g_ptr_array_set_size(results, lineNum + 1);
    }
    g_ptr_array_insert(results, lineNum, codedLine);
    pthread_mutex_unlock(&lock);
  }
  g_async_queue_push(codingData, finished);
  return NULL;
}

#define USAGE "usage: %s mode(1 for compress 2 for decomress) input_file outout_file [num_threads]\n"
#define MODE  "Mode is 1 to compress input file 2 to decomress inut file\n"
#define FILE_ERROR "Could not open file %s\n"
#define THREAD_ERROR "Could not create thread\n"


int main(int argc, char**argv){
  if(argc < 4){
    fprintf(stderr, USAGE, argv[0]);
    exit(1);
  }

  int mode = atoi(argv[1]);
  if(mode != 1 && mode != 2){
    fprintf(stderr, MODE);
    exit(1);
  }

  coder coder_func = (coder)encode;
  if(mode == 2){
    coder_func = (coder)decode;
  }

  pthread_mutex_init(&lock, 0);
  codingData = g_async_queue_new_full((GDestroyNotify)fileLineDestructor);
  results = g_ptr_array_new_with_free_func((GDestroyNotify)free);
  size_t num_threads = 1;
  if(argc >= 5){
    num_threads = atoi(argv[4]);
  }

  FILE* input = fopen(argv[2], "r");
  if(!input){
    fprintf(stderr, FILE_ERROR, argv[2]);
    exit(1);
  }

  size_t i;
  pthread_t threads[num_threads];
  for(i = 0; i < num_threads; ++i){
    int t = pthread_create(threads + i, 0, encodeLine, coder_func);
    if(t){
      fprintf(stderr, THREAD_ERROR);
      exit(1);
    }
  }


  size_t bytesRead;
  char* line = NULL;
  size_t n = 0;
  size_t lineNum = 0;
  while((bytesRead = getline(&line, &n, input)) != -1){
    fileLine* temp = malloc(sizeof(fileLine));
    temp->lineNo = lineNum++;
    if(line[bytesRead - 1] == '\n'){
      if(strlen(line) >= 2 && line[bytesRead - 2] == '\r') {
        line[bytesRead - 2] = 0;
      }
      line[bytesRead - 1] = 0;
    }
    temp->line = strdup(line);
    g_async_queue_push(codingData, temp);
  }
  free(line);
  g_async_queue_push(codingData, finished);

  for(i = 0;i < num_threads; ++i){
    pthread_join(threads[i], 0);
  }

  FILE* out = fopen(argv[3],"w");
  for(i = 0; i < lineNum; ++i){
    char* resLine = g_ptr_array_index(results, i);
    if(!strcmp(resLine, "\n")){
      fprintf(out, "\n");
    } else{
      fprintf(out, "%s\n", resLine);
    }
  }

  //clean up
  pthread_mutex_destroy(&lock);
  g_ptr_array_free(results, TRUE);
  return 0;
}
