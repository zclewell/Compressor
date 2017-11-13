#include <glib.h>
#include <stdlib.h>


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

}
