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
    /*int curr = input[i];
    if(BitFileGetBitsNum(encodedFile, &curr, BITSIZE, sizeof(int)) == EOF){
      break;
    }*/
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
    int nextInt = 0;
    int res = BitFileGetBitsNum(encodedFile, &nextInt, BITSIZE, sizeof(int));
    if(res == EOF){
      break;
    }
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

void encode(char* input, size_t length, bit_file_t* file, GHashTable* table){
  size_t currentIndex  = 0;
  size_t nextIndex = 1;

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


}

void run_encode_file(char* input, char* out){
  int fd = fileno(fopen(input, "r"));
  size_t fileSize= lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  char* mappedFile = (char*)mmap(NULL, fileSize, PROT_WRITE, MAP_PRIVATE, fd, 0);
  GHashTable* table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
  bit_file_t* encodedFile = BitFileOpen(out, BF_WRITE);
  encode(mappedFile, fileSize, encodedFile, table);

  BitFileClose(encodedFile);
  munmap(mappedFile, fileSize);
  g_hash_table_destroy(table);
}


//TODO write read size to file, fix new case w new lines
int main (int argc, char** argv){

  char* n = "hett.txt";
  char* o = "hett-comp.txt";
  if(argc == 1)
    run_encode_file(n, o);
  else
    run_decode_file(o, "hett-res.txt");
  return 0;
}
