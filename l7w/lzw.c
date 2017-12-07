#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "bitfile.h"

int BITSIZE = 12;
int extendedAscii = 256;
int maxAscii = 4095;



void decode(bit_file_t* encodedFile, int fd){
  GHashTable* table = g_hash_table_new_full(g_int_hash, g_int_equal, free, free);

  int curr = 0;
  BitFileGetBitsNum(encodedFile, &curr, BITSIZE, sizeof(int));
  while(1){
    int currentLength = 1;
    char* out = NULL;
    if(curr > 255){
      //if(!g_hash_table_contains())
      out  = g_hash_table_lookup(table, &curr);
      //if(!out){
      //  printf("error\n");
      //}
      currentLength = strlen(out);
      write(fd, out, currentLength);
    } else{
      write(fd, &curr, 1);
    }

    int nextLength = 1;
    int nextInt = 0;
    int res = BitFileGetBitsNum(encodedFile, &nextInt, BITSIZE, sizeof(int));
    if(res == EOF){
      break;
    }
    //if(nextInt == 2560){
    //  printf("error 2\n" );
    //}
    char* next = NULL;
    if(nextInt > 255){
      if(!g_hash_table_contains(table, &nextInt)){
        char* newEntry = malloc(currentLength + 2);
        newEntry[currentLength + 1] = 0;
        if(out){
          strncpy(newEntry, out, currentLength);
          newEntry[currentLength] = *out;
        } else{
          newEntry[0] = curr;
          newEntry[1] = curr;
        }
        int* key = malloc(sizeof(int));
        //*key = nextInt;
        *key = extendedAscii;
        ++extendedAscii;
        g_hash_table_insert(table, key, newEntry);
        curr = nextInt;
        continue;
      }
      next = g_hash_table_lookup(table, &nextInt);
    }

    char* val = malloc(currentLength + nextLength + 1);
    val[currentLength + nextLength] = 0;
    if(currentLength > 1){
      strncpy(val, out, currentLength);
    } else{
      *val = curr;
    }
    if(next){
      strncpy(val + currentLength, next, 1);
    } else{
      val[currentLength] = nextInt;
    }
    int* key = malloc(sizeof(int));
    //if(g_hash_table_lookup(table, &extendedAscii)){
    //  ++extendedAscii;
    //}
    *key = extendedAscii;
    g_hash_table_insert(table, key, val);
    ++extendedAscii;
    curr = nextInt;
  }
  g_hash_table_destroy(table);

}

int read_bits_to_buff(bit_file_t* inputFile, int* buff, int count){
  for(int i = 0; i < count; ++i){
    int temp = 0;
    int result = BitFileGetBitsNum(inputFile, &temp, BITSIZE, sizeof(int));
    buff[i] = temp;
    if(result == EOF){
      if(!i){
        return -1;
      }
      return i + 1;
    }
  }
  return count;
}



void run_decode_file(char* input, char* out){
  bit_file_t* inputFile = BitFileOpen(input, BF_READ);
  int outfd = fileno(fopen(out, "w"));
  decode(inputFile, outfd);
  close(outfd);
  BitFileClose(inputFile);
}

void encode(char* input, size_t length, bit_file_t* file){
  size_t currentIndex  = 0;
  size_t nextIndex = 1;
  GHashTable* table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

  while(nextIndex <= length){
    if(nextIndex == length){
      if(strlen(input + currentIndex) > 1){
        int* code = g_hash_table_lookup(table, input + currentIndex);
        BitFilePutBitsNum(file, code, BITSIZE, sizeof(int));
      } else{
        BitFilePutBitsNum(file, input + currentIndex, BITSIZE, 1);
      }
      break;
    }

    char saved = input[nextIndex + 1];
    input[nextIndex + 1] = 0;
    if(g_hash_table_contains(table, input + currentIndex)){
      input[++nextIndex] = saved;
      continue;
    } else if(extendedAscii <= maxAscii){
      int* temp = malloc(sizeof(int));
      *temp = extendedAscii;
      ++extendedAscii;
      g_hash_table_insert(table, strdup(input + currentIndex), temp);
    }

    char temp = input[nextIndex];
    input[nextIndex] = 0;
    if(strlen(input + currentIndex) > 1){
      int* code = g_hash_table_lookup(table, input + currentIndex);
      BitFilePutBitsNum(file, code, BITSIZE, sizeof(int));
    } else{
      BitFilePutBitsNum(file, input + currentIndex, BITSIZE, 1);
    }

    input[nextIndex] = temp;
    input[nextIndex + 1] = saved;
    currentIndex = nextIndex;
    nextIndex = currentIndex + 1;
  }
  g_hash_table_destroy(table);

}

void run_encode_file(char* input, char* out){
  int fd = fileno(fopen(input, "r"));
  size_t fileSize= lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  char* mappedFile = (char*)mmap(NULL, fileSize, PROT_WRITE, MAP_PRIVATE, fd, 0);
  bit_file_t* encodedFile = BitFileOpen(out, BF_WRITE);
  encode(mappedFile, fileSize, encodedFile);

  BitFileClose(encodedFile);
  munmap(mappedFile, fileSize);
}

#define USAGE "usage: %s mode(1 for compress 2 for decomress) input_file outout_file\n"
#define MODE  "Mode is 1 to compress input file 2 to decomress inut file\n"

int main(int argc, char**argv){
  if(argc != 4){
    fprintf(stderr, USAGE, argv[0]);
    exit(1);
  }

  int mode = atoi(argv[1]);

  char* input = argv[2];
  char* output = argv[3];

  if(mode == 1){
    run_encode_file(input, output);
  } else if(mode == 2){
    run_decode_file(input, output);
  } else{
    fprintf(stderr, MODE);
  }

  return 0;

}

//TODO write read size to file
