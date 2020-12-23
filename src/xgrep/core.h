#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define BUF_EXP_SIZE 512
#define BUF_INIT_SIZE 1024

#define LINE_INIT_SIZE 256
#define LINE_EXP_SIZE 128

int grep_std_print(const char *pattern, const char *path)
{
  FILE *f = fopen(path, "rt");
  if (!f)
  {
    perror("open");
    return errno;
  }

  int linesSize = LINE_INIT_SIZE, size = BUF_INIT_SIZE, idx = 0;
  char *line = (char *)malloc(size * sizeof(char));
  char **lineBuf = (char **)malloc(linesSize * sizeof(char *));
  int *strSizes = (int *)malloc(linesSize * sizeof(int));
  int lineCount = 0;
  memset(lineBuf, 0, linesSize);

  char c = fgetc(f);

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
      lineBuf[lineCount] = line;
      strSizes[lineCount] = idx;
      lineCount++;
      if(lineCount == linesSize)
      {
        linesSize += LINE_EXP_SIZE;
        lineBuf = (char **)realloc(lineBuf, linesSize);
        strSizes = (int *)realloc(strSizes, linesSize);
      }
      size = BUF_INIT_SIZE, idx = 0;
      line = (char *)malloc(size * sizeof(char));
    }
    c = fgetc(f);
  }

  for (int i = 0; i < lineCount; i++)
  {
    if (lineBuf[i] && strstr(lineBuf[i], pattern) != NULL)
    {
      write(STDOUT_FILENO, lineBuf[i], strSizes[i]);
      free(lineBuf[i]);
    }
  }

  free(lineBuf);
  free(strSizes);
  return lineCount;
}