#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define BUF_EXP_SIZE 512
#define BUF_INIT_SIZE 1024
#define LINE_INIT_SIZE 256
#define LINE_EXP_SIZE 128

struct Edit {
  int type; // 0 - add, 1 - delete 1, 2 - delete 2,  3 - change
  int idx1, idx2;
};

int diff_std_print(const char *file1_path, const char *file2_path, int brief, int unified)
{
  FILE *f1 = fopen(file1_path, "rt");
  FILE *f2 = fopen(file2_path, "rt");
  if (!f1 || !f2)
  {
    perror("open");
    return errno;
  }

  int linesSize = LINE_INIT_SIZE, size = BUF_INIT_SIZE, idx = 0;
  char *line = (char *)malloc(size * sizeof(char));

  char **lineBuf1 = (char **)malloc(linesSize * sizeof(char *));
  int *strSizes1 = (int *)malloc(linesSize * sizeof(int));
  memset(lineBuf1, 0, linesSize);
  int lineCount1 = 0;

  char c = fgetc(f1);
  while (c != EOF)
  {
    if (idx == size)
    {
      size += BUF_EXP_SIZE;
      line = (char *)realloc(line, size);
    }
    line[idx++] = c;
    if (c == '\n')
    {
      lineBuf1[lineCount1] = line;
      strSizes1[lineCount1] = idx;
      lineCount1++;
      if (lineCount1 == linesSize)
      {
        linesSize += LINE_EXP_SIZE;
        lineBuf1 = (char **)realloc(lineBuf1, linesSize);
        strSizes1 = (int *)realloc(strSizes1, linesSize);
      }
      size = BUF_INIT_SIZE, idx = 0;
      line = (char *)malloc(size * sizeof(char));
    }
    c = fgetc(f1);
  }

  linesSize = LINE_INIT_SIZE, size = BUF_INIT_SIZE, idx = 0;
  line = (char *)malloc(size * sizeof(char));

  char **lineBuf2 = (char **)malloc(linesSize * sizeof(char *));
  int *strSizes2 = (int *)malloc(linesSize * sizeof(int));
  memset(lineBuf2, 0, linesSize);
  int lineCount2 = 0;

  c = fgetc(f2);
  while (c != EOF)
  {
    if (idx == size)
    {
      size += BUF_EXP_SIZE;
      line = (char *)realloc(line, size);
    }
    line[idx++] = c;
    if (c == '\n')
    {
      lineBuf2[lineCount2] = line;
      strSizes2[lineCount2] = idx;
      lineCount2++;
      if (lineCount2 == linesSize)
      {
        linesSize += LINE_EXP_SIZE;
        lineBuf2 = (char **)realloc(lineBuf2, linesSize);
        strSizes2 = (int *)realloc(strSizes2, linesSize);
      }
      size = BUF_INIT_SIZE, idx = 0;
      line = (char *)malloc(size * sizeof(char));
    }
    c = fgetc(f2);
  }

  if (brief) {
    if (lineCount1 != lineCount2)
      printf("Files %s and %s differ\n", file1_path, file2_path);
    else{
      for(int i = 0; i < lineCount1; ++i)
        if(strcmp(lineBuf1[i], lineBuf2[i]) != 0){
          printf("Files %s and %s differ\n", file1_path, file2_path);
          break;
        }
    }
    goto Clear;
    return 0; 
  }

  int **minedit = (int **)malloc((lineCount1 + 1) * sizeof(int *));

  for (int i = 0; i < lineCount1 + 1; i++)
  {
    minedit[i] = (int *)malloc((lineCount2 + 1) * sizeof(int));

    for (int j = 0; j < lineCount2 + 1; j++)
    {
      if (i == 0 || j == 0)
      {
        minedit[i][j] = i + j;
      }
      else
      {
        if (strcmp(lineBuf1[i - 1], lineBuf2[j - 1]) == 0){
          minedit[i][j] = minedit[i - 1][j - 1];
        }
        else
          minedit[i][j] = MIN(minedit[i][j - 1]+1, MIN(minedit[i - 1][j]+1, minedit[i - 1][j - 1] + 2));
      }
      // printf("%d ", minedit[i][j]);
    }
    // printf("\n");
  }

  int i = lineCount1, j = lineCount2;
  struct Edit *edits = (struct Edit*)malloc(1024 * sizeof(struct Edit));
  int editCount = 0;
  while(i!=0 || j!=0){
    if (i == 0){
      struct Edit edit;
      edit.type = 0; edit.idx1 = 0; edit.idx2 = j;
      edits[editCount++] = edit;
      j-=1;
      continue;
    }
    if (j == 0) {
      struct Edit edit;
      edit.type = 1; edit.idx1 = i; edit.idx2 = 0;
      edits[editCount++] = edit;
      i-=1;
      continue;
    }
    if (minedit[i-1][j-1] == minedit[i][j] && strcmp(lineBuf1[i - 1], lineBuf2[j - 1]) == 0){
      struct Edit edit;
      edit.type = 4; edit.idx1 = i; edit.idx2 = j;
      edits[editCount++] = edit;
      i-=1;j-=1;
      continue;
    }
    if (minedit[i-1][j-1] == minedit[i][j]-2){
      struct Edit edit;
      edit.type = 3; edit.idx1 = i; edit.idx2 = j;
      edits[editCount++] = edit;
      i-=1;j-=1;
      continue;
    }
    if (minedit[i][j-1] == minedit[i][j] - 1){
      struct Edit edit;
      edit.type = 1; edit.idx1 = i; edit.idx2 = j;
      edits[editCount++] = edit;
      j-=1;
      continue;
    }
    if (minedit[i-1][j] == minedit[i][j] - 1){
      struct Edit edit;
      edit.type = 2; edit.idx1 = i; edit.idx2 = j;
      edits[editCount++] = edit;
      i-=1;
      continue;
    }
  }

  if (unified){
    for(int i = editCount-1; i>=0; --i){
      if(edits[i].type == 4){
        printf(" %s", lineBuf2[edits[i].idx2-1]);  
        continue;
      }
      if(edits[i].idx1 == 0) {
        printf("+%s", lineBuf2[edits[i].idx2-1]);  
        continue;
      }
      if(edits[i].idx2 == 0) { 
        printf("-%s", lineBuf1[edits[i].idx1-1]);  
        continue;
      }
      if(edits[i].type == 3) {
        printf("-%s", lineBuf1[edits[i].idx1-1]);
        printf("+%s", lineBuf2[edits[i].idx2-1]);
        continue;
      }
      if(edits[i].type == 1) {
        printf("+%s", lineBuf2[edits[i].idx2-1]);  
        continue;
      }
      if(edits[i].type == 2) {
        printf("-%s", lineBuf1[edits[i].idx1-1]);  
        continue;
      }
    }
  }
  else {
    for(int i = editCount-1; i>=0; --i){
      if(edits[i].type == 4)
        continue;
      if(edits[i].idx1 == 0) {
        printf("%da%d\n", edits[i].idx1, edits[i].idx2);
        printf("> %s", lineBuf2[edits[i].idx2-1]);  
        continue;
      }
      if(edits[i].idx2 == 0) { 
        printf("%dd%d\n", edits[i].idx1, edits[i].idx2);
        printf("< %s", lineBuf1[edits[i].idx1-1]);  
        continue;
      }
      if(edits[i].type == 3) {
        printf("%dc%d\n", edits[i].idx1, edits[i].idx2);
        printf("< %s", lineBuf1[edits[i].idx1-1]);
        printf("---\n");
        printf("> %s", lineBuf2[edits[i].idx2-1]);
        continue;
      }
      if(edits[i].type == 1) {
        printf("%dd%d\n", edits[i].idx1, edits[i].idx2);
        printf("> %s", lineBuf2[edits[i].idx2-1]);  
        continue;
      }
      if(edits[i].type == 2) {
        printf("%dd%d\n", edits[i].idx1, edits[i].idx2);
        printf("< %s", lineBuf1[edits[i].idx1-1]);  
        continue;
      }
    }
  }
  Clear:
  for (int i = 0; i < lineCount1; i++)
      free(lineBuf1[i]);
  for (int i = 0; i < lineCount2; i++)
      free(lineBuf2[i]);
  free(lineBuf1);free(lineBuf2);
  free(strSizes1);
  
  fclose(f1);
  fclose(f2);
  return 0;
}