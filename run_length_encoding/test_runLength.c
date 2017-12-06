#include "runLength.c"
#include <assert.h>

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

  // this case is currently failing
  //TODO come up with escaping system to differentiate literal number
  char* mixCase = strdup("xyz = -1");
  //assert(!strcmp("a1=1/1/1/", encode(mixCase)));
  char* decoded = decode(encode(mixCase));
  assert(!strcmp(decoded, mixCase));

  char* slashes = strdup("aaa /44/31/");
  char* decoded2 = decode(encode(slashes));
  assert(!strcmp(decoded2, slashes));

}
