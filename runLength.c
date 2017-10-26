#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

  size_t i;
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
  size_t i;
  size_t totalLength = 0;
  char* temp = str;
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
    *endOfNum = saved;
    totalLength += repeat;
    tmp = endOfNum;
  }

  char* result = malloc(totalLength + 1);
  // fill in result string






}




int main(){
  char* test = strdup("hello");
  printf("%s\n",encode(test));
}
