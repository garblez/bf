#include <unistd.h>
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
  int cur; // index of current cell
  int pc; // program counter (position in buffer memory)
};

int allzero(struct tape *t) {
  for (int i = 0; i < TAPELEN; i++) {
    if (t->cells[i] != 0)
      return 1;
  }
  return 0;
}

// Get the index in the buffer of the matching closing bracket ]
int closing(struct tape *t, char *buffer) {
  int depth = 1;
  int i;
  
  for (i = t->pc + 1; buffer[i] != '\0' && depth != 0; i++) {
    if (buffer[i] == '[')
      ++depth;
    else if (buffer[i] == ']')
      --depth;
  }

  return i;
}

// Get the index in the buffer of the token before the matching opening bracket [
int opening(struct tape *t, char *buffer) {
  int depth = -1;
  int i;
  for (i = t->pc - 1; i > 0 && depth != 0; i--) {
    if (buffer[i] == '[') 
      ++depth;
    else if (buffer[i] == ']')
      --depth;
  }
  return i; // The for-loop decrements i before we check the condition to find the depth == 0
}

void execute(struct tape *t, char *buffer) {
  char tok = buffer[t->pc];
  switch (tok) {
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
      putchar(t->cells[t->cur]);
      break;
    case '[':
      if (t->cells[t->cur] == 0) 
        t->pc = closing(t, buffer);
      break; 
    case ']': 
      if (t->cells[t->cur] != 0)
        t->pc = opening(t, buffer);
      break;
  }
  ++t->pc;
}

void run(struct tape *tp, char *buffer) {
  char c;
  while (buffer[tp->pc] != '\0') {
    /*
    printf("\x1B[H");
    for (int i = 0; (c = buffer[i]) != '\0'; i++) {
      if (i == tp->pc) {
        printf("\033[47m\033[30m%c\033[0m", c);
      } else {
        printf("%c", c);
      }

    }
    */
    execute(tp, buffer);
//    sleep(1);
  }
}

int main(int argc, char **argv){

  /*
  for (int i = 0; i < argc; i++)
    printf("%s\n", argv[i]);
  printf("%s\n", argv[1]);
  */

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

  //printf("%s\n", buffer);

  if (!isbalanced(buffer)) {
    perror("Syntax error: unbalanced brackets");
    free(buffer);
    return 1;
  }

  struct tape interpreter;
  interpreter.cur = 0;
  interpreter.pc = 0;
  int *p = calloc(TAPELEN, sizeof(int));
  for (int i = 0; i < TAPELEN; i++) {
    interpreter.cells[i] = p[i];
  }

  free(p);

  /*
  if (0 == allzero(&interpreter)){
    printf("All zero!\n");
  } else {
    printf("Something isn't zero!\n");
  }
  */

  run(&interpreter, buffer);

  /*
  printf("\n");

  for (int i = 0; i < 20; i++)
    printf("%d ", interpreter.cells[i]);
  printf("\n");
  */
  free(buffer);

  return 0;
}
