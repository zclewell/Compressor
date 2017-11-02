#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>




int numDigits(int n){
  int count = 0;
  while(n){
    n/= 10;
    ++count;
  }
  return count;
}

char* encode(char* line){
  char* front = line;
  // initially assume worst case for how much memory is needed, realloc in the end
  char* result = malloc(2 * strlen(line) + 1);
  size_t currentSize = 2 * strlen(line) + 1;
  result[strlen(line)] = 0;
  size_t resultIndex = 0;

  while(*line){
    char c = *line;
    char* temp = line;
    size_t count = 0;
    while(*temp){
      if(*temp != c){
        break;
      }
      ++temp;
      ++count;
    }

    result[resultIndex++] = c;
    //convertIntToChar(result + resultIndex, count);
    sprintf(result + resultIndex, "%lu", count);
    resultIndex += numDigits(count);
    line = temp;
  }

  result = realloc(result, resultIndex + 1);
  result[resultIndex] = 0;
  return result;
}

char* decode(char* str){
  size_t totalLength = 0;
  char* tmp = str;
  // this int array stores how many time each character is repeated
  // is in same order as encded string, memory allocated is upper bound for number of characters that are repeated
  int* charRepeatCount = malloc(sizeof(int) * strlen(str) / 2);
  int indexRepeatCount = -1;
  // figure out length of string
  while(*tmp){
    char* num = tmp + 1;
    char* endOfNum = num;
    while(isdigit(*endOfNum)){
      ++endOfNum;
    }
    char saved = *endOfNum;
    *endOfNum = 0;
    int repeat = atoi(num);
    charRepeatCount[++indexRepeatCount] = repeat;
    *endOfNum = saved;
    totalLength += repeat;
    tmp = endOfNum;
  }


  char* result = malloc(totalLength + 1);
  // fill in result string
  size_t i;
  size_t index = 0;
  for(i = 0; i <= indexRepeatCount; ++i){
    int count = charRepeatCount[i];
    char charToRepeat = *str;
    size_t j = 0;
    for(j; j < count; ++j){
      result[index++] = charToRepeat;
    }
    str += numDigits(count) + 1;
  }

  free(charRepeatCount);
  return result;
}

// basic tests, checks if encoded string is correct, and decoded ( encoeded) = original string
int main(){
  char* test1 = strdup("hello");
  char* encoded = encode(test1);
  assert(!strcmp(encoded, "h1e1l2o1"));
  assert(!strcmp(test1, decode(encoded)));

  char* test2 = strdup("hhhhaaa");
  char* encoded2 = encode(test2);
  assert(!strcmp(encoded2, "h4a3"));
  assert(!strcmp(test2, decode(encoded2)));

  // check if endcoing/ decoding wokrs with character repeated more than 9 times
  // also has spaces
  char* bigStr = strdup("mmmmmmmmmm  ");
  char* encoded3 = encode(bigStr);
  assert(!strcmp(encoded3, "m10 2"));
  assert(!strcmp(bigStr, decode(encoded3)));

}
