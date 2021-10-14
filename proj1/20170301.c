#include "20170301.h"

int main(){ 
	char command[82];
	char* processed[5]; // 가공된 명령어가 저장될 포인터 배열
	char* original;	
	int i;
	int check=-1;
	int check_hash=-1;
	while(1){
		check=-1; 
		if (check_hash==-1) check_hash=makeTable(); // 해쉬 테이블 생성
		printf("sicsim>");
		fgets(command,82,stdin);
		original=malloc(sizeof(char)*82);
		strcpy(original,command);
		// 입력받은 command를 가공하는 부분
		processed[0]=strtok(command," \n"); // 명령어는 indent 혹은 개행으로 구분
		for (i=1;i<5;i++){
			processed[i]=strtok(NULL,", \n"); // 구체적인 value는 반점,indent, 개행으로 구분
		}

		// ***여기서부터 shell 관련 명령어***
		//help
		if (!strcmp(processed[0],"h")||!strcmp(processed[0],"help")&&!processed[1]&&!processed[2]&&!processed[3])	
		{
			help();
			insert(original);
		}	
		//history
		else if (!strcmp(processed[0],"hi")||!strcmp(processed[0],"history")&&!processed[1]&&!processed[2]&&!processed[3])
		{		
			insert(original);
			history();
		}
		//quit
		else if (!strcmp(processed[0],"q")||!strcmp(processed[0],"quit")&&!processed[1]&&!processed[2]&&!processed[3])
		{
			return 0;
		}
		//dir
		else if (!strcmp(processed[0],"d")||!strcmp(processed[0],"dir")&&!processed[1]&&!processed[2]&&!processed[3])
		{
			dir();
			insert(original);
		}

		// ***여기서부터 memory 관련 명령어***
		// dump
		else if (!strcmp(processed[0],"du")||!strcmp(processed[0],"dump")){
			if (!processed[1]){ // dump only 
				dump();
				insert(original);
			}
			else if (!processed[2]){ // dump start
				int dec_s=strtol(processed[1],NULL,16);
				check=dump_start(dec_s);
				if (!check) insert(original);
			}
			else if (!processed[3]){ //dump start,end 
				int dec_s=strtol(processed[1],NULL,16);
				int dec_e=strtol(processed[2],NULL,16);
				check=dump_startend(dec_s,dec_e);
				if (!check) insert(original);
			} 
			else continue; //dump 1,2,3 같은 invalid한 입력이  들어온 경우다시 입력받도록
		}
		else if (!strcmp(processed[0],"edit")||!strcmp(processed[0],"e")&&processed[1]&&processed[2]&&!processed[3]) {
			int dec=strtol(processed[1],NULL,16);
			int dec_v=strtol(processed[2],NULL,16);
			check=edit(dec,dec_v);
			if(!check) insert(original);
		}
		else if ((!strcmp(processed[0],"fill")||!strcmp(processed[0],"f"))&&processed[1]&&processed[2]&&processed[3]&&!processed[4])
		{
			int dec_s=strtol(processed[1],NULL,16);
			int dec_e=strtol(processed[2],NULL,16);
			int dec_v=strtol(processed[3],NULL,16);
			check=fill(dec_s,dec_e,dec_v);
			if(!check) insert(original);
		}
		else if (!strcmp(processed[0],"reset")&&!processed[1]&&!processed[2]&&!processed[3]){
			memset(memory,0,sizeof(char)*col*row);
			insert(original);
		}
		else if (!strcmp(processed[0],"opcode")&&processed[1]&&!processed[2]){
			check=findOpcode(processed[1]);	
			if (!check) insert(original); // 값을 찾은 경우에만 history에 들어가게		
		}
		else if (!strcmp(processed[0],"opcodelist")&&!processed[1]){
			insert(original);
			showHt();
		}		
		else continue; // 정의되지 않은 명령어
	}
}


	
int help(){
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start,end]\ne[dit] address,value\nf[ill] start,end,value\nreset\nopcode mnemonic\nopcodelist\n");
	return 0; 
	};

int history(){
	for (worker=first;worker->link!=NULL;worker=worker->link){
		printf("%d %s\n",worker->number,worker->str);
	}
	return 0;
}

int insert(char* string){
	strtok(string,"\n");
	nodePointer newNode=(nodePointer)malloc(sizeof(node));
	if (!newNode) return 0; // memory allocation failed.
	newNode->str=string;
	newNode->number=insert_idx++;
	newNode->link=NULL;
	if (!first){ // 처음 삽입일 때만 작동
		first=newNode;
		worker=first;
	}
	else {	
		worker->link=newNode;
		worker=worker->link;
	}
	return 0;
}

int dir(){
	DIR *dir_ptr=NULL;
	struct dirent *file=NULL; 
	struct stat buf;
	if((dir_ptr=opendir("."))==NULL) return 0; // file open error
	while ((file=readdir(dir_ptr))!=NULL){
		stat(file->d_name,&buf); // 파일의 속성이 buf에 저장됨
		if (S_ISDIR(buf.st_mode)) // 여기서 쓰는 함수는 매크로  
			printf("%s/\t\t",file->d_name);
		else if ((S_IEXEC & buf.st_mode)!=0) // st_mode값과 and연산으로 소유자의 실행 권한 확인
			printf("%s*\t\t",file->d_name);
		else printf("%s\t\t", file->d_name);
	}
	printf("\n");
	closedir(dir_ptr);
	return 0;
}


int dump(){
	if (address+159>0xfffff) return dump_startend(address,0xfffff);
	else return dump_startend(address,address+159);
}	
int dump_start(int start){ 
	if (start+159>0xfffff) return dump_startend(start,0xfffff);
	else return dump_startend(start,start+159);
}
int dump_startend(int start, int end){
	// 번지수를 10진수로 변환 후 16으로 나눈 몫은 memory의 행, 나눈 나머지는 열
	if (start>0xfffff||start<0||end>0xfffff||end<0||start>end){
		printf("Error : Invalid input.\n");
		return -1;
	}
	int r=start/16; // will be the row of memory[][] 
	int c=start%16; // will be the col of memory[][]
	int num=end-start+1; // 출력할 항의 총 개수(decimal)
	
	int i,j,l; //l은 ASCII 컬럼에서 쓰일 인덱스
	for (i=0;i<num;){
		printf("%04X0 ",r); // address field
		for(j=0;j<16;j++){
			if (i==0&&j<c) printf("   "); // c열 이전의 열들은 출력 x 
			else if (i>=num) break;
			else {
				printf("%02X ",memory[r][j]); // memory field
				i++;
			}
				
		}
		for (j;j<16;j++) printf("   ");


		printf(" ; ");
		for(l=0;l<16;l++){
			if (i==0&&l<c) printf("."); // c열 이전의 행은 . 으로 출력
			else {
				if(memory[r][l]<0x20||memory[r][l]>0x7e) printf(".");
				else printf("%c",memory[r][l]);
			}
		}
		r=r+1;
		printf("\n");
	}
	// address 갱신
	if (end==0xfffff) address=0; 
	else address=end+1;
	return 0;
}

			
int edit(int address, int value){ 
	if (address>0xfffff||address<0) {
		printf("Error : Invalid address\n");
		return -1;
	}
	if (value<0||value>0xff) {
		printf("Error : Invalid value\n");
		return -1;
	}
	int r=address/16;
	int c=address%16;
	memory[r][c]=value;
	return 0;
}

int fill(int start, int end, int value){
	if (start>0xfffff||start<0||end>0xfffff||end<0||start>end){
		printf("Error : Invalid address\n");
		return -1;
	}
	if (value<0||value>0xff){
		printf("Error : Invalid value\n");
		return -1;
	}
	
	int r=start/16;
	int c=start%16;
	int num=end-start+1;

	int i=r,j=c;
	while(num>0){
		if(j==16){ j=0; i++;}
		memory[i][j++]=value;
		num--;
	}
	return 0;
}


int h(char* mnemonic){
	int sum=0;
	int i=0;
	int total=strlen(mnemonic);
	for (i=0;i<total;i++) sum+=mnemonic[i];
	return sum%20;
}

int insert_hash(int hb, bucket* newNode){
	if(!ht[hb]) ht[hb]=newNode;
	else{
		newNode->link=ht[hb];
		ht[hb]=newNode;
	}
	return 0;
}

int makeTable(){
	char mnemonic[10];
	char fmt[10];
	int op;
	FILE *fp=fopen("opcode.txt","r");
	if (fp==NULL){
		printf("file open error\n");
		return -1;
	}
	while (fscanf(fp,"%x %s %s\n",&op,mnemonic,fmt)!=EOF){
		bucket* newNode=(bucket*)malloc(sizeof(bucket));
		newNode->key=malloc(sizeof(char)*10);
		strcpy(newNode->key,mnemonic);
		newNode->opcode=op;
		newNode->link=NULL;
		int hb=h(newNode->key); // newNode의 버킷 주소
		insert_hash(hb,newNode);
	}
	fclose(fp);
	return 0;
}
	

int findOpcode(char* mnemonic){
	int flag=0;
	bucketPtr worker=ht[h(mnemonic)];
	while(worker){
		if (!strcmp(worker->key,mnemonic)){
			printf("opcode is %X\n",worker->opcode);
			flag=1;
			break;
		}
		worker=worker->link;
	}
	if (flag==1) return 0;
	else return -1;
}

int showHt(void){
	bucketPtr worker;
	int i;
	for (i=0;i<20;i++){
		printf("%d : ",i);
		if (!ht[i]){ printf("\n"); continue;}
		for(worker=ht[i];worker->link!=NULL;worker=worker->link){
			printf("[%s,%02X] -> ",worker->key,worker->opcode);
		}
		printf("[%s,%02X]",worker->key,worker->opcode);
		printf("\n");
	}
	return 0;
}

				
