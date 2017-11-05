#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>


/*
* Returns the number of digits in a number
* @param n number for which number of digits is calculated
* @return number of digits in n
*/
int numDigits(int n){
  int count = 0;
  while(n){
    n/= 10;
    ++count;
  }
  return count;
}

/*
* Encodes a string using run length encoding
* @parm line the string that is encoded
* @return encoded string, allocated on heap
*/
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

/*
* Decodes a string encoded with run length encoding
* @param str string that is decoded
* @return decoded string, is allocated on heap
*/
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