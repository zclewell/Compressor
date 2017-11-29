#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitfile.h"

int BITSIZE = 12;
int extendedAscii = 256;


void encode(char* input, bitfile_t* file){
  int length = strlen(input);
  //char* result = malloc(length);
  GHashTable* table = g_hash_table_new(g_str_hash, g_str_equal);
  int currentIndex  = 0;
  int nextIndex = 1;
  //char* output = result;

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
      g_hash_table_add(table, strdup(input + currentIndex));
    }
    char temp = input[nextIndex];
    input[nextIndex] = 0;
    if(strlen(input + currentIndex) > 1){
      BitFilePutBitsNum(file, &extendedAscii, BITSIZE, sizeof(int));
      //memcpy(output, &extendedAscii, 2);
      ++extendedAscii;
    } else{
      BitFilePutBitsNum(file, input + currentIndex, BITSIZE, 1);
      //memcpy(output, input + currentIndex, 2);
    }
    //output += 2;

    // restore string
    input[nextIndex] = temp;
    input[nextIndex + 1] = saved;
    currentIndex = nextIndex;printf("%s\n", );
    nextIndex = currentIndex + 1;
  }

  g_hash_table_destroy(table);
  //return result;
}

int main (int argc, char** argv){
  bit_file_t* inputFile = BitFileOpen(argv[1], BF_READ);
  bitfile_t* outFile = BitFileOpen(argv[2], BF_WRITE);
  encode(strdup("this is the"), outFile);
  return 0;
}
