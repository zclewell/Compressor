#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

int numBytesNeeded(int n){
  int count = 0;
  while(n){
    n/= 10;
    ++count;
  }
  return count;
}

void convertIntToChar(char* str, int n){
  if(n < 10){
    *str = n + '0';
    return;
  }
  size_t i;
  str += numBytesNeeded(n) - 1;
  while(n){
    int digit = n % 10;
    *str = digit + '0';
    --str;
    n/= 10;
  }

}

char* encode(char* line){
  char* result = malloc(strlen(line) + 1);
  size_t currentSize = strlen(line) + 1;
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
    // realloc space if needed
    if(resultIndex + numBytesNeeded(count) >= currentSize - 1){
      int bytesAvailable = currentSize - resultIndex - 2;
      // allocate two extra bytes than needed, for next iteration
      result = realloc(result, currentSize + numBytesNeeded(count) - bytesAvailable + 2);
      currentSize += numBytesNeeded(count) - bytesAvailable + 2;
      result[currentSize - 1] = 0;
    }

    result[resultIndex++] = c;
    convertIntToChar(result + resultIndex, count);
    resultIndex += numBytesNeeded(count);

    line = temp;
  }

  result = realloc(result, resultIndex + 1);
  result[resultIndex] = 0;
  return result;
}

char* decode(char* str){
  size_t totalLength = 0;
  char* tmp = str;
  int* charRepeatCount = malloc(sizeof(int) * strlen(str));
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
    str += numBytesNeeded(count) + 1;
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
