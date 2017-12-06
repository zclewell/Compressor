#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bitfile.h"

int BITSIZE = 12;
int extendedAscii = 256;


void decode(int* input, size_t count, int fd, GHashTable* table){
  // assuming 12 bit reads for now
  //int fd; // set output file descriptor
  //size_t nextIndex;
  for(size_t i = 0; i < count; ++i){
    int curr = input[i];
    if(curr > 255){
      char* out  = g_hash_table_lookup(table, &curr);
      write(fd, out, 2);
      continue;
    } else{
      //out = &curr;
      write(fd, &curr, 1);
    }
    //TODO write out to result string
    int next = input[i + 1];
    if(next > 255){
      continue;
    }
    // convert to str
    char* val = malloc(2);
    val[0] = (char)curr;
    val[1] = (char)next;
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
  GHashTable* table = g_hash_table_new(g_int_hash, g_int_equal);
  while((res = read_bits_to_buff(inputFile, buff, 1024)) > 0){
    decode(buff, res, outfd, table);
  }
}


void encode(char* input, bit_file_t* file){
  int length = strlen(input);
  //char* result = malloc(length);
  GHashTable* table = g_hash_table_new(g_str_hash, g_str_equal);
  int currentIndex  = 0;
  int nextIndex = 1;

  while(nextIndex <= length){
    char saved = 0;
    if(nextIndex != length){
      saved = input[nextIndex + 1];
      input[nextIndex + 1] = 0;
    }

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
      printf("outing %d for %s\n", extendedAscii, input + currentIndex);
      int* code = g_hash_table_lookup(table, input + currentIndex);
      //BitFilePutBits(file, code, BITSIZE);
      BitFilePutBitsNum(file, code, BITSIZE, sizeof(int));
    } else{
      printf("outing %c\n",*(input + currentIndex));
      BitFilePutBitsNum(file, input + currentIndex, BITSIZE, 1);
      //memcpy(output, input + currentIndex, 2);
    }

    input[nextIndex] = temp;
    input[nextIndex + 1] = saved;
    currentIndex = nextIndex;//printf("%s\n", );
    nextIndex = currentIndex + 1;
  }

  g_hash_table_destroy(table);
}

int main (int argc, char** argv){

  //bit_file_t* outFile = BitFileOpen("out.txt", BF_WRITE);
  //encode(strdup("thisisthe"), outFile);
  //BitFileByteAlign(outFile);

  bit_file_t* inputFile = BitFileOpen("out.txt", BF_READ);
  run_decode_file(inputFile, 1);
  return 0;
}
