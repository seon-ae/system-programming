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


typedef struct symbol* symbolPtr;
typedef struct symbol{
	char* opcode;
	int loc;
	symbolPtr link;
} symbol;

// 전역 변수 선언
nodePointer first=NULL; // 가장 먼저 호출된 명령어 가리킴
nodePointer worker=NULL; // 움직이면서 링크드리스트를 이어줌. 
char memory[row][col]; // 가상 메모리
int address;
int insert_idx=1; //insert할때마다 증가할 인덱스
bucket* ht[20];
char assembly[30];

char* fm=NULL;
int ob; 
int start; // 메모리 시작 주소
int end;
int error_line;
symbol* ht_s[26]; // 알파벳 자모의 개수 26개 
int LOCCTR;
int base; // 베이스 레지스터
int loc; // opr1 심볼의 LOC값 

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

// proj2
int findOpcode_2(char* mnemonic); // findOpcode와 기능은 똑같지만, 두 전역변수에 값을 저장한다.
int typeFilename(char *filename);
int assemble(char *asmfile, int pass);
int insert_symbol(int hashBal, symbol* newNode);
int findSymbol(char* symbol); // 심볼 테이블에서 해당 해시값이 주인이 있는지 
int h_s(char* mnemonic);
int showSt(void);
int makeObFile(char *filename); // 오브젝트 코드를 갖고 .obj파일을 만듦
