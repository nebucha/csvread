#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFSIZE 102400


static char SEPARATOR = ',';
static char UNSCO = '_';

char *getval(char *p, char *val, int size)
{
  char *sval;
  sval = val;

  // 要素がダブルクォートで囲まれている部分
  if (*p == '"') {
    while(*(++p) && (*p != '\n')) {
      if(*p == '\\' && *(p+1) == '"') {
        // エスケープされたダブルクォートは、
        // エスケープ文字を取り除く
        ++p;
      } else if (*p == '"') {
        // エスケープされていないダブルクォートで終了
        ++p;
        break;
      }

      // 要素をセット 半角スペースは指定文字（デフォルトはアンダースコア）に置換
      if (--size > 0) {
        *(val++) = (*p!=' ')?*p:UNSCO;
      }
    }
  }

  // ダブルクォートで囲まれていない部分
  while (*p && (*p != SEPARATOR) && (*p != '\n')) {
    // 要素をセット 半角スペースは指定文字（デフォルトはアンダースコア）に置換
    if (--size > 0) {
      *(val++) = (*p!=' ')?*p:UNSCO;
    }
    p++;
  }

  // 要素が空の場合はアンダースコアをセット  
  if (sval == val) {
    *(val++) = '_';
  }

  *val = '\0';

  // 次の要素の先頭ポインタを返す
  return *p ? (p+1) : p;
} 

int main(int argc, char *argv[]) 
{
  char buf[BUFFSIZE];
  char val[BUFFSIZE];
  char *p;
  char *filename = NULL; 
  FILE *fp = NULL;

  // オプション解析
  int opt = 0;
  while ((opt = getopt(argc, argv, "hd:s:")) != -1) {
    switch (opt) {
      case 'd':
        if (strlen(optarg) == 1) {
          UNSCO = optarg[0];
        } else {
          fprintf(stderr, "%s: Parameter error -- option d must be single character.\n", argv[0]);
          fprintf(stderr, "Usage: %s [-d character] [-s character] [filename]\n", argv[0]);
          exit(1);
        }
        break;
      case 's':
        if (strlen(optarg) == 1) {
          SEPARATOR = optarg[0];
        } else {
          fprintf(stderr, "%s: Parameter error -- option s must be single character.\n", argv[0]);
          fprintf(stderr, "Usage: %s [-d character] [-s character] [filename]\n", argv[0]);
          exit(1);
        }
        break;
      case 'h':
        fprintf(stderr, "Usage: %s [-d character] [-s character] [filename]\n", argv[0]);
        exit(1);
      default:
        fprintf(stderr, "Usage: %s [-d character] [-s character] [filename]\n", argv[0]);
        exit(1);
        break;
    }
  }

  // 引数にファイル名が指定されていれば、ファイルから読み込む
  // ファイル名が指定されていなければ、標準入力から読み込む
  if (optind < argc) {
    filename = argv[optind];
    if ((fp = fopen(filename, "r")) == NULL) {
      fprintf(stderr, "File open error: %s\n", filename);
      exit(1);
    }
  } else {
    fp = stdin;
  }

  // 読み込み
  while (fgets(buf, sizeof(buf), fp)) {
    p = buf;
    while (*p) {
      p = getval(p, val, sizeof(val));
      printf("%s ", val);
    }
    puts("");  	
  }

  fclose(fp);
  return 0;
}


