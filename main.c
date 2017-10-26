#include <stdio.h>



/// takes in array of pointer to strings to be decoded, terminated by null
// each string represents line in file assuming they do not end with new line character
// TODO vector of lines may be better choice, easier when reading in file and encdoing
void decodeFile(char** encoded, FILE* fp, char*(*decoder) (char*)){
  while(*encoded){
    char* decodedLine = decoder(*encoded);
    fprintf(fp, "%s\n", decodedLine);
    free(decodedLine);
    ++encoded;
  }
}



vector* encodeFile(FILE* fp, char*(*encoder)(char*)){

  size_t bytesRead;
  char* line = 0;
  size_t n = 0;
  while((bytesRead = getline(&line, &n, fp)) != -1){
    if(line[bytesRead - 1] == '\n'){
      line[bytesRead - 1] = 0;
    }
    char* encoded = encoder(line);
    // insert into vector


  }
  free(line);


}








int main(int argc, char** argv){





}
