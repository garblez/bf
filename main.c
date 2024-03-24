#include <stdio.h>
#include <stdlib.h>

#define TAPELEN 30000

long flen(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  long len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  return len;
}

char *fbuffer(FILE *fp) {
  long len = flen(fp);
  char *buffer = (char *)malloc(len+1);
  if (buffer == NULL)
    return NULL;
  fread(buffer, 1, len, fp);
  buffer[len] = '\0';
  return buffer;
}

int isbalanced(char *b) {
  int openb = 0;
  int closeb = 0;
  char c;
  while ((c = *b++) != '\0') {
    if (c == '[') {
      ++openb;
    } else if (c == ']') {
      ++closeb;
    }
  }

  return openb == closeb;
}

struct tape {
  int cells[TAPELEN];
  int cur;
};

int allzero(struct tape *t) {
  for (int i = 0; i < TAPELEN; i++) {
    if (t->cells[i] != 0)
      return 1;
  }
  return 0;
}

int closing(char *buffer, int start) {
  int depth = 1;
  int i;
  
  for (i = start + 1; buffer[i] != '\0' && depth != 0; i++) {
    if (buffer[i] == '[')
      ++depth;
    else if (buffer[i] == ']')
      --depth;
  }

  return i;
}

int opening(char *buffer, int end) {
  int depth = -1;
  int i;
  for (i = end - 1; i > 0 && depth != 0; i--) {
    if (buffer[i] == '[') 
      ++depth;
    else if (buffer[i] == ']')
      --depth;
  }
  return i + 1; // The for-loop decrements i before we check the condition to find the depth == 0
}

void execute(struct tape *t, char *buffer, int cursor) {
  char c = buffer[cursor];
  switch (c) {
    case '>':
      ++t->cur;
      break;
    case '<':
      --t->cur;
      break;
    case '+':
      t->cells[t->cur]++;
      break;
    case '-':
      t->cells[t->cur]--;
      break;
    case ',':
      t->cells[t->cur] = getchar();
      break;
    case '.':
      putchar(t->cells[t->cur]+65);
      break;
    case '[':
      if (t->cells[t->cur] == 0) 
        t->cur = closing(buffer, cursor);
      break; 
    case ']': 
      if (t->cells[t->cur] != 0)
        t->cur = opening(buffer, cursor);
      break;
  }
}

void run(struct tape *tp, char *buffer) {
  for (int i = 0; buffer[i] != '\0'; i++) {
    execute(tp, buffer, i);
  }
}

int main(int argc, char **argv){

  for (int i = 0; i < argc; i++)
    printf("%s\n", argv[i]);
  printf("%s\n", argv[1]);

  if (argc != 2) {
    perror("Usage: bf [file]\n");
    return 1;
  }

  char *fname = argv[1];

  FILE *f;
  char *buffer;

  f = fopen(fname, "r");

  if (f == NULL) {
    perror("Error opening file");
    return 1;
  }

  buffer = fbuffer(f);

  if (buffer == NULL) {
    perror("Failed to allocate memory");
    fclose(f);
    return 1;
  }

  fclose(f);

  printf("%s\n", buffer);

  if (!isbalanced(buffer)) {
    perror("Syntax error: unbalanced brackets");
    free(buffer);
    return 1;
  }

  struct tape interpreter;
  interpreter.cur = 0;
  int *p = calloc(TAPELEN, sizeof(int));
  for (int i = 0; i < TAPELEN; i++) {
    interpreter.cells[i] = p[i];
  }

  free(p);

  if (0 == allzero(&interpreter)){
    printf("All zero!\n");
  } else {
    printf("Something isn't zero!\n");
  }

  run(&interpreter, buffer);

  printf("\n");

  for (int i = 0; i < 20; i++)
    printf("%d ", interpreter.cells[i]);
  printf("\n");
  free(buffer);

  return 0;
}
