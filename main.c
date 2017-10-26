

/// takes in array of pointer to strings to be decoded, terminated by null
// each string represents line in file assuming they do not end with new line character
void decodeFile(char** encoded, FILE* fp, char*(*decoder) (char*)){
  while(*encoded){
    char* decodedLine = decoder(*encoded);
    fprintf(fp, "%s\n", decodedLine);
    free(decodedLine);
    ++encoded;
  }
}

void*encodeFile(char* fileName, char*(*encoder)(char*)){



  
}








int main(int argc, char** argv){





}
