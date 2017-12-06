#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bitfile.h"

int BITSIZE = 12;
int extendedAscii = 256;


void decode(int* input, size_t count, int fd, GHashTable* table){
  for(size_t i = 0; i < count; ++i){
    int curr = input[i];
    int currentLength = 1;
    char* out = NULL;
    if(curr > 255){
      out  = g_hash_table_lookup(table, &curr);
      currentLength = strlen(out);
      write(fd, out, currentLength);
    } else{
      write(fd, &curr, 1);
    }

    int nextLength = 1;
    int nextInt = input[i + 1];
    char* next = NULL;
    if(nextInt > 255){
      next = g_hash_table_lookup(table, &nextInt);
      nextLength = strlen(next);
    }

    char* val = malloc(currentLength + nextLength + 1);
    val[currentLength + nextLength] = 0;
    if(currentLength > 1){
      strncpy(val, out, currentLength);
    } else{
      *val = curr;
    }
    if(nextLength > 1){
      strncpy(val + currentLength, next, nextLength);
    } else{
      val[currentLength] = nextInt;
    }
    int* key = malloc(sizeof(int));
    *key = extendedAscii;
    g_hash_table_insert(table, key, val);
    ++extendedAscii;
  }

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



void run_decode_file(bit_file_t* inputFile, int outfd){
  int buff[1024];
  int res;
  GHashTable* table = g_hash_table_new_full(g_int_hash, g_int_equal, free, free);
  while((res = read_bits_to_buff(inputFile, buff, 1024)) > 0){
    decode(buff, res, outfd, table);
  }
  g_hash_table_destroy(table);
}


void encode(char* input, bit_file_t* file){
  int length = strlen(input);
  GHashTable* table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
  int currentIndex  = 0;
  int nextIndex = 1;

  while(nextIndex <= length){
    if(nextIndex == length){
      if(strlen(input + currentIndex) > 1){
        int* code = g_hash_table_lookup(table, input + currentIndex);
        BitFilePutBitsNum(file, code, BITSIZE, sizeof(int));
        //printf("outing %d for %s\n", *code, input + currentIndex);
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
    } else{
      int* temp = malloc(sizeof(int));
      *temp = extendedAscii;
      ++extendedAscii;
      g_hash_table_insert(table, strdup(input + currentIndex), temp);
    }
    char temp = input[nextIndex];
    input[nextIndex] = 0;
    if(strlen(input + currentIndex) > 1){
      int* code = g_hash_table_lookup(table, input + currentIndex);
      //printf("outing %d for %s\n", *code, input + currentIndex);
      BitFilePutBitsNum(file, code, BITSIZE, sizeof(int));
    } else{
      //printf("outing %c\n",*(input + currentIndex));
      BitFilePutBitsNum(file, input + currentIndex, BITSIZE, 1);
    }

    input[nextIndex] = temp;
    input[nextIndex + 1] = saved;
    currentIndex = nextIndex;
    nextIndex = currentIndex + 1;
  }

  g_hash_table_destroy(table);
}

int main (int argc, char** argv){

  //bit_file_t* outFile = BitFileOpen("out.txt", BF_WRITE);
  //encode(strdup("thisisthethe"), outFile);

  bit_file_t* inputFile = BitFileOpen("out.txt", BF_READ);
  run_decode_file(inputFile, 1);
  return 0;
}
