#include <glib.h>
#include <stdlib.h>

/*
void populateDictionary(GHashTable* table, char* str){
  size_t i =0;
  while(i < strlen(str)){
    size_t j;
    char saved;
    for(j = i + 1; j < streln(str); ++j){
      saved = str[j];
      str[j] = 0;
      if(!g_hash_table_contains(table, str + i)){
        g_hash_table_add(strdup(str + i), table);
        break;
      }
    }
    str[j] = saved;
    i = j;
  }
}*/

int asciiSum(char* str){
  int sum = 0;
  while(*str){
    sum += (int)*str;
    ++str;
  }
  return sum;
}


char* encode(char* input, int windowsSize, int searchSize){

  int nextSymIncrement = 2;
  int currentIndex  = 0;
  int nextIndex = 1;
  //char current = *input;
  char saved = input[nextIndex + 1];
  input[nextIndex + 1] = 0;
  if(g_hash_table_contains(input + currentIndex)){
    ++nextIndex;
    continue;
  } else{
    g_hash_table_add(strdup(input + currentIndex), table);
  }
  char temp = input[nextIndex];
  input[nextIndex] = 0;
  sprintf(STR HERE, "%d",asciiSum(input + currentIndex));
  // restore string
  input[nextIndex] = temp;
  input[nextIndex + 1] = saved;
  currentIndex = nextIndex;
  nextIndex = currentIndex + 1;

}
