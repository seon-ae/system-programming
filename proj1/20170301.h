#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#define col 16
#define row 65536

typedef struct node* nodePointer;
typedef struct node{
	int key;
	int number;
	char* str;
	nodePointer link;
} node; 

typedef struct bucket* bucketPtr;
typedef struct bucket{
	char* key;
	char* format;
	int opcode;
	bucketPtr link;
} bucket;


// 전역 변수 선언
nodePointer first=NULL; // 가장 먼저 호출된 명령어 가리킴
nodePointer worker=NULL; // 움직이면서 링크드리스트를 이어줌. 
char memory[row][col]; // 가상 메모리
int address;
int insert_idx=1; //insert할때마다 증가할 인덱스
bucket* ht[20];

// 함수 정의 
int help(void);
int insert(char* string);
int history(void);
int dir(void);
int dump(void);
int dump_start(int start);
int dump_startend(int start, int end);
int edit(int address, int value);
int fill(int start, int end, int value);
int h(char* mnemonic);
int makeTable(void);
int insert_hash(int hb,bucket* newNode);
int findOpcode(char* mnemonic);
int showHt(void);

