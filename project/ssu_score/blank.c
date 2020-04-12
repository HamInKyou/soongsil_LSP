#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "blank.h"

char datatype[DATATYPE_SIZE][MINLEN] = {"int", "char", "double", "float", "long"
			, "short", "ushort", "FILE", "DIR","pid"
			,"key_t", "ssize_t", "mode_t", "ino_t", "dev_t"
			, "nlink_t", "uid_t", "gid_t", "time_t", "blksize_t"
			, "blkcnt_t", "pid_t", "pthread_mutex_t", "pthread_cond_t", "pthread_t"
			, "void", "size_t", "unsigned", "sigset_t", "sigjmp_buf"
			, "rlim_t", "jmp_buf", "sig_atomic_t", "clock_t", "struct"};


operator_precedence operators[OPERATOR_CNT] = {
	{"(", 0}, {")", 0}
	,{"->", 1}	
	,{"*", 4}	,{"/", 3}	,{"%", 2}	
	,{"+", 6}	,{"-", 5}	
	,{"<", 7}	,{"<=", 7}	,{">", 7}	,{">=", 7}
	,{"==", 8}	,{"!=", 8}
	,{"&", 9}
	,{"^", 10}
	,{"|", 11}
	,{"&&", 12}
	,{"||", 13}
	,{"=", 14}	,{"+=", 14}	,{"-=", 14}	,{"&=", 14}	,{"|=", 14}
};

//두 트리를 비교하는 함수
void compare_tree(node *root1,  node *root2, int *result)
{
	node *tmp;
	int cnt1, cnt2;
	
	//두 트리중 하나라도 없을 경우
	if(root1 == NULL || root2 == NULL){
		*result = false; //결과는 false
		return; //함수 끝내기
	}
	
	//첫번째 트리 루트가 < 또는 > 또는 <= 또는 >=일 경우
	if(!strcmp(root1->name, "<") || !strcmp(root1->name, ">") || !strcmp(root1->name, "<=") || !strcmp(root1->name, ">=")){
		//첫번째 트리 루트와 두번째 트리 루트 비교했을 때 다를 경우
		if(strcmp(root1->name, root2->name) != 0){
			
			//두번째 트리 루트의 문자열의 첫 바이트가 '<'일 경우
			if(!strncmp(root2->name, "<", 1)){
				//두번째 트리 루트의 문자열의 첫 바이트를 '>'로 바꿔준다.
				strncpy(root2->name, ">", 1); 
			}

			//두번째 트리 루트의 문자열의 첫 바이트가 '>'일 경우
			else if(!strncmp(root2->name, ">", 1)){
				//두번째 트리 루트의 문자열의 첫 바이트를 '<'로 바꿔준다.
				strncpy(root2->name, "<", 1);
			}

			//두번째 트리 루트의 문자열의 두 바이트가 '<='일 경우
			else if(!strncmp(root2->name, "<=", 2)){
				//두번째 트리 루트의 문자열의 두 바이트를 '>='로 바꿔준다.
				strncpy(root2->name, ">=", 2);
			}

			//두번째 트리 루트의 문자열의 두 바이트가 '>='일 경우
			else if(!strncmp(root2->name, ">=", 2)){
				//두번째 트리 루트의 문자열의 두 바이트를 '<='로 바꿔준다.
				strncpy(root2->name, "<=", 2);
			}
			
			//두번째 트리 루트의 자식노드의 형제들을 바꿔준다.
			root2 = change_sibling(root2);
		}
	}
	
	//첫번째 트리 루트와 두번째 트리 루트가 다를경우
	if(strcmp(root1->name, root2->name) != 0){
		*result = false; //결과는 false
		return; //함수 끝내기
	}
	
	//첫번째 트리 루트의 자식이 존재하고 두번째 트리 루트의 자식은 존재하지 않거나
	//첫번째 트리 루트의 자식이 존재하지 않고 두번째 트리 루트의 자식은 존재할 경우
	if((root1->child_head != NULL && root2->child_head == NULL)
		|| (root1->child_head == NULL && root2->child_head != NULL)){
		*result = false; //결과는 false
		return; //함수 끝내기
	}
	//첫번째 트리 루트의 자식이 존재할 경우
	else if(root1->child_head != NULL){
		//첫번째 트리 루트의 자식의 형제 노드들의 개수와 두번째 트리 루트의 자식의 형제 노드들의 개수가 다르다면
		if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
			*result = false; //결과는 false
			return; //함수 끝내기
		}
		//첫번째 트리 루트의 값이 ==거나 !=일 경우
		if(!strcmp(root1->name, "==") || !strcmp(root1->name, "!="))
		{
			//첫번째 트리 루트의 자식을 루트로한 트리와 두번째 트리 루트의 자식을 루트로한 트리를 비교
			compare_tree(root1->child_head, root2->child_head, result);
			
			//비교한 결과가 false인 경우
			if(*result == false)
			{
				//비교한 결과를 true로 바꿔주고
				*result = true;
				//두번째 트리 루트의 자식 형제들을 바꿔준다.
				root2 = change_sibling(root2);
				//첫번째 트리 루트의 자식을 루트로한 트리와 두번째 트리 루트의 자식을 루트로한 트리를 비교
				compare_tree(root1->child_head, root2->child_head, result);
			}
		}
		//첫번째 트리 루트의 값이 + 또는 * 또는 | 또는 & 또는 || 또는 &&인 경우
		else if(!strcmp(root1->name, "+") || !strcmp(root1->name, "*")
				|| !strcmp(root1->name, "|") || !strcmp(root1->name, "&")
				|| !strcmp(root1->name, "||") || !strcmp(root1->name, "&&"))
		{
			//첫번째 트리 루트의 자식의 형제노드의 개수와 두번째 트리 루트의 자식의 형제 노드의 개수가 다르다면
			if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
				*result = false; //결과는 false
				return; //함수 끝내기
			}
			
			//tmp에 두번째 트리 루트의 자식을 저장해준다.
			tmp = root2->child_head;

			//tmp의 이전 형제 노드가 존재하는 동안
			while(tmp->prev != NULL)
				tmp = tmp->prev; //tmp를 이전 형제 노드로 해준다.
			
			//tmp가 존재할 경우
			while(tmp != NULL)
			{
				//첫번째 트리 루트의 자식을 루트로한 트리와 tmp를 루트로한 트리를 비교한다.
				compare_tree(root1->child_head, tmp, result);
				
				//result가 true일 경우 반복 그만
				if(*result == true)
					break;
				//result가 false일 경우
				else{
					//tmp의 다음 형제 노드가 존재할 경우
					if(tmp->next != NULL)
						*result = true; //result를 true로 해준다.
					tmp = tmp->next; //tmp를 다음 형제 노드로 해준다.
				}
			}
		}
		//첫번째 트리 루트의 값이 ==, !=, +, *, |, &, ||, &&가 아닐 경우
		else{
			//첫번째 트리 루트의 자식을 루트로한 트리와 두번째 트리 루트의 자식을 루트로한 트리를 비교한다.
			compare_tree(root1->child_head, root2->child_head, result);
		}
	}	

	//첫번째 트리 루트의 다음 형제가 존재한다면
	if(root1->next != NULL){
		//첫번째 트리 루트 노드의 형제노드들의 개수와 두번째 트리 루트 노드의 형제 노드들의 개수가 다를 경우
		if(get_sibling_cnt(root1) != get_sibling_cnt(root2)){
			*result = false; //result를 false로
			return; //함수 끝내기
		}
		//result가 true일 경우
		if(*result == true)
		{
			//첫번째 트리 루트의 형제중 제일 첫번째 형제의 부모를 리턴해서 tmp에 저장
			tmp = get_operator(root1); 
			
			//tmp노드가 + 또는 * 또는 | 또는 & 또는 || 또는 &&인 경우
			if(!strcmp(tmp->name, "+") || !strcmp(tmp->name, "*")
					|| !strcmp(tmp->name, "|") || !strcmp(tmp->name, "&")
					|| !strcmp(tmp->name, "||") || !strcmp(tmp->name, "&&"))
			{	
				//tmp에 root2 저장
				tmp = root2;
				
				//tmp의 이전 형제 노드가 존재하는 동안 반복
				while(tmp->prev != NULL)
					tmp = tmp->prev; //tmp를 tmp 이전 형제 노드를 가리키게
				
				//tmp가 존재할 경우
				while(tmp != NULL)
				{
					//첫번째 트리 루트의 다음 형제 노드를 루트로 한 트리를 tmp를 루트로한 트리를 비교한다.
					compare_tree(root1->next, tmp, result);
					
					//result가 true일 경우
					if(*result == true)
						break; //반복문 그만한다.
					else{ //result가 false인 경우
						if(tmp->next != NULL) //tmp의 다음형제 노드가 존재할 경우
							*result = true; //result를 true로
						tmp = tmp->next; //tmp가 tmp의 다음형제 노드 가리키게
					}
				}
			}

			else{
				//첫번째 트리의 루트노드의 다음 형제노드를 루트로 하는 트리와
				//두번째 트리의 루트노드의 다음 형제노드를 루트로 하는 트리를 비교
				compare_tree(root1->next, root2->next, result);
			}
		}
	}
}

//문자열과 토큰을 저장하는 tokens 배열을 인자로 받아와서
//tokens에 문자열을 토큰들로 바꾸어 저장해주는 함수
int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN])
{
	char *start, *end;
	char tmp[BUFLEN];
	char str2[BUFLEN];
	char *op = "(),;><=!|&^/+-*\""; 
	int row = 0;
	int i;
 	int isPointer;
	int lcount, rcount;
	int p_str;
	
	clear_tokens(tokens); //tokens 배열의 모든 값을 0으로 초기화한다.

	start = str;          //str의 시작주소를 start가 가리키게 한다.
	
	if(is_typeStatement(str) == 0) //gcc 혹은 데이터 타입이 이상한 위치에 있을 경우 
		return false;              //false 리턴
	
	while(1)
	{
		if((end = strpbrk(start, op)) == NULL) //str(학생이 제출한 답)에 연산자가 없을경우 
			break; //반복 끝내기

		if(start == end){ //연산자가 있는 위치가 start 위치일 경우
			
			//그 연산자가 --혹은 ++연산자일 경우
			if(!strncmp(start, "--", 2) || !strncmp(start, "++", 2)){
				if(!strncmp(start, "++++", 4)||!strncmp(start,"----",4)) //++++ 혹은 ----일 경우
					return false; //잘못된 연산이니 false 리턴

				if(is_character(*ltrim(start + 2))){ //++ 또는 --뒤에 숫자혹은 영문자가 왔을경우
					//제일 첫 토큰이 아니고 바로 전 토큰의 마지막 글자가 숫자 혹은 영문자일 경우
					if(row > 0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]))
						return false; //false 리턴

					end = strpbrk(start + 2, op); //++ 혹은 -- 연산자 뒤에 존재하는 연산자 위치를 end가 가리키게
					if(end == NULL) //뒤에 연산자가 없을 경우
						end = &str[strlen(str)];   //end를 str의 제일 끝을 가리키게
					while(start < end) {          //str을 끝까지 반복
						//현재 위치의 전거가 공백이고 현재 토큰의 마지막 글자가 숫자 혹은 영문자일 경우
						if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
							return false; //false 리턴
						else if(*start != ' ') //현재 위치의 값이 공백이 아닐 경우
							strncat(tokens[row], start, 1); //현재 토큰에 start가 가리키는 제일 첫번째 값을 붙인다.
						start++;	
					}
				}
				
				//제일 첫 토큰이 아니고 바로 전 토큰의 마지막 글자가 숫자 혹은 영문자일 경우
				else if(row>0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
					//바로 전 토큰에 ++이 있거나 바로 전 토큰에 --가 있다면
					if(strstr(tokens[row - 1], "++") != NULL || strstr(tokens[row - 1], "--") != NULL)	
						return false; //false 리턴

					memset(tmp, 0, sizeof(tmp));  //tmp를 초기화
					strncpy(tmp, start, 2);       //tmp에 start부터 두글자를 입력한다.
					strcat(tokens[row - 1], tmp); //바로 전 토큰에 start로부터 두글자를 입력받은 tmp를 뒤에 붙여준다.
					start += 2; //start의 위치를 뒤로 두칸 이동한다.
					row--;      //한 토큰 전으로 돌아감
				}

				//제일 첫 토큰이거나 바로 전 토큰의 마지막 글자가 숫자 혹은 영문자가 아닐 경우
				else{
					memset(tmp, 0, sizeof(tmp)); //tmp를 초기화
					strncpy(tmp, start, 2);      //tmp에 start부터 두글자를 입력한다.
					strcat(tokens[row], tmp);    //현재 토큰 뒤에 start부터 두글자를 입력한 tmp를 붙여준다.
					start += 2;                  //start를 뒤로 두칸 움직인다.
				}
			}

			//그 연산자가 비교연산자일 경우
			else if(!strncmp(start, "==", 2) || !strncmp(start, "!=", 2) || !strncmp(start, "<=", 2)
				|| !strncmp(start, ">=", 2) || !strncmp(start, "||", 2) || !strncmp(start, "&&", 2) 
				|| !strncmp(start, "&=", 2) || !strncmp(start, "^=", 2) || !strncmp(start, "!=", 2) 
				|| !strncmp(start, "|=", 2) || !strncmp(start, "+=", 2)	|| !strncmp(start, "-=", 2) 
				|| !strncmp(start, "*=", 2) || !strncmp(start, "/=", 2)){

				strncpy(tokens[row], start, 2); //현재 토큰에 start부터 두글자를 복사해서 넣어준다.
				start += 2;                     //start를 뒤로 두칸 옮긴다.
			}
			
			//그 연산자가 구조체 포인터 참조 연산자일 경우
			else if(!strncmp(start, "->", 2))
			{
				end = strpbrk(start + 2, op);   //'->'연산자 뒤로부터 연산자가 또 있는지 찾아 그 위치를 end가 가리키게 한다.

				if(end == NULL)
					end = &str[strlen(str)];     //'->'연산자 뒤에 또 다른 연산자가 없을 경우 end를 맨끝을 가리키게 한다.

				while(start < end){             //start를 뒤로 옮겨가며 다음 연산자 도달할 때까지 반복
					if(*start != ' ')            //start가 가리키는 값이 공백이 아니라면
						strncat(tokens[row - 1], start, 1); //바로 전 토큰 뒤에 start에서 한글자를 읽어서 붙인다.
					start++;                     //start를 뒤로 한칸 옮긴다.
				}
				row--;                          //한 토큰 전으로 돌아감
			}

			//그 연산자가 & 연산자일 경우
			else if(*end == '&')
			{
				//현재 첫번째 토큰이거나 바로 전 토큰에서 연산자를 발견하지 못했을 경우
				if(row == 0 || (strpbrk(tokens[row - 1], op) != NULL)){
					end = strpbrk(start + 1, op); //end는 &연산자 다음으로 연산자가 위치한 곳을 가리키게
					if(end == NULL)               //다음 연산자가 없다면
						end = &str[strlen(str)];   //end가 str의 맨끝을 가리키게
					
					strncat(tokens[row], start, 1); //현재토큰 뒤에 start에서 한글자를 읽어서 붙인다.
					start++;                        //start를 뒤로 한칸 옮긴다.

					while(start < end){  //다음 연산자에 도달할때까지 반복
						//start바로 전이 공백이고 현재 토큰의 마지막 글자가 '&'일 경우 false 리턴
						if(*(start - 1) == ' ' && tokens[row][strlen(tokens[row]) - 1] != '&')
							return false;
						else if(*start != ' ')             //현재 start가 가리키는 것이 공백이 아니라면
							strncat(tokens[row], start, 1); //현재 토큰 뒤에 start에서 한글자를 읽어 붙인다.
						start++;
					}
				}
				
				else{ //현재 첫번째 토큰이 아니고 바로 전 토큰에서 연산자를 발견했을 경우
					strncpy(tokens[row], start, 1); //현재 토큰 뒤에 start에서 한글자를 읽어 붙인다.
					start += 1; //start를 뒤로 한칸 옮긴다.
				}
				
			}
			
			//그 연산자가 * 연산자일 경우
		  	else if(*end == '*')
			{
				isPointer=0;
				
				if(row > 0) //현재 토큰이 제일 첫번째 토큰이 아니라면
				{
					
					for(i = 0; i < DATATYPE_SIZE; i++) { //데이터 타입이 저장된 배열을 차례로 훑으면서
						if(strstr(tokens[row - 1], datatype[i]) != NULL){ //바로 전 토큰에 데이터타입이 있다면
							strcat(tokens[row - 1], "*");                  //바로 전 토큰뒤에 '*' 붙인다.
							start += 1;	                                   //start를 뒤로 한칸 옮긴다.
							isPointer = 1;                                 //isPointer를 킨다.
							break;                                         //탐색 그만
						}
					}
					if(isPointer == 1) //isPointer가 켜져있을 경우
						continue;       //이번 반복차례는 넘어감
					if(*(start+1) !=0) //start 바로 다음 문자가 null이 아닐 경우
						end = start + 1; //end를 start 바로 다음 문자를 가르키게 해준다.

					//제일 첫 토큰이 아니고, 전전토큰에 '*'이 있고, 전토큰이 모두 '*'로 이루어져 있을 경우
					if(row>1 && !strcmp(tokens[row - 2], "*") && (all_star(tokens[row - 1]) == 1)){
						strncat(tokens[row - 1], start, end - start); //바로 전 토큰의 뒤에 start부터 end까지의 문자열을 붙인다.
						row--; //한 토큰 전으로 돌아간다.
					}
					
					//바로 전 토큰의 마지막 글자가 숫자혹은 영문자였다면
					else if(is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]) == 1){ 
						strncat(tokens[row], start, end - start); //현재 토큰의 끝에 start부터 end까지의 문자열을 붙인다. 
					}

					//바로 전 토큰에 연산자가 존재할 경우
					else if(strpbrk(tokens[row - 1], op) != NULL){		
						strncat(tokens[row] , start, end - start);  //현재 토큰의 끝에 start부터 end까지의 문자열을 붙인다.
							
					}
					else //위의 세 조건 중에 맞는 조건이 없을 경우
						strncat(tokens[row], start, end - start); //현재 토큰의 끝에 start부터 end까지의 문자열을 붙인다.

					start += (end - start); //start를 end 위치로 옮겨준다.
				}

			 	else if(row == 0) //현재 제일 처음 토큰일 경우
				{
					//end를 '*'연산자 다음으로 나오는 연산자 위치를 가리키게 했는데, 뒤에 연산자가 없었을 경우
					if((end = strpbrk(start + 1, op)) == NULL){ 
						strncat(tokens[row], start, 1); //현재 토큰의 뒤에 start에서 한글자를 읽어서 붙인다.
						start += 1; //start를 뒤로 한칸 옮긴다.
					}
					//end를 '*'연산자 다음으로 나오는 연산자 위치를 가리키게 했고, 연산자 존재할 경우
					else{
						while(start < end){ //다음 연산자 전까지 반복
							//start의 바로 전 문자가 공백이고 현재토큰의 제일 끝 글자가 숫자 혹은 영문자일 경우
							if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
								return false; //false 리턴
							else if(*start != ' ') //현재 start가 가리키는 값이 공백일 경우
								strncat(tokens[row], start, 1); //현재 토큰의 뒤에 start에서 한글자를 읽어 붙인다.
							start++;	//start를 뒤로 한칸 옮긴다.
						}
						if(all_star(tokens[row])) //현재 토큰이 모두 '*'로 이루어져 있을 경우
							row--; //한 토큰 전으로 돌아간다.
						
					}
				}
			}

			//그 연산자가 여는괄호 연산자였을 경우
			else if(*end == '(') 
			{
				lcount = 0; //여는괄호 개수 저장하는 변수
				rcount = 0; //닫는괄호 개수 저장하는 변수
				//현재 제일 첫 토큰이 아니고, 바로 전 토큰이 '&'이거나 '*'일 경우
				if(row>0 && (strcmp(tokens[row - 1],"&") == 0 || strcmp(tokens[row - 1], "*") == 0)){
					while(*(end + lcount + 1) == '(') //반복을 통해 연속적인 여는 괄호의 개수를 체크
						lcount++;                      
					start += lcount;                  //start를 여는 괄호 개수만큼 뒤로 이동한다.

					end = strpbrk(start + 1, ")");    //end를 닫는 괄호가 처음 나온 위치로 이동한다.   

					if(end == NULL)  //닫는 괄호가 없을 경우
						return false; //false 리턴
					else{                                  //닫는 괄호가 있을 경우
						while(*(end + rcount +1) == ')')    //반복을 통해 연속적인 닫는 괄호의 개수를 체크
							rcount++;
						end += rcount; //end를 닫는 괄호 다 닫힌 그 다음 위치로 이동시킨다.

						if(lcount != rcount) //여는 괄호와 닫는 괄호의 개수가 다를 경우
							return false; //false 리턴
						
						//첫번째 토큰이 아니고, 전전 토큰의 마지막 글자가 숫자 혹은 영문자가 아니거나, 두번째 토큰일 경우
						if( (row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) || row == 1){ 
							//전 토큰의 끝에 start 다음 위치부터 닫는 괄호 전까지의 문자열을 갖다붙인다.
							strncat(tokens[row - 1], start + 1, end - start - rcount - 1); 
							row--; //한 토큰 전으로 돌아간다.
							start = end + 1; //start를 모든 닫는 괄호의 다음 위치로 이동시킨다.
						}
						//첫번째 토큰이거나, 전전 토큰의 마지막 글자가 솟자 혹은 영문자고, 두번째 토큰이 아닐경우
						else{     
							strncat(tokens[row], start, 1); //현재 토큰의 뒤에 start로부터 한글자를 읽어서 붙인다.
							start += 1;                     //start의 위치를 뒤로 한칸 옮긴다.
						}
					}
						
				}
				//현재 제일 첫 토큰이거나, 바로 전 토큰이 '&'과 '*'가 아닐 경우
				else{
					strncat(tokens[row], start, 1); //현재 토큰 뒤에 start에서 한글자를 읽어서 갖다붙이고
					start += 1;                     //start를 뒤로 한칸 이동시킨다.
				}

			}

			//그 연산자가 \ 연산자였을 경우
			else if(*end == '\"') 
			{
				end = strpbrk(start + 1, "\""); //end를 현재 연산자 이후에 '\'연산자가 나오는 위치로 이동시킨다.
				
				if(end == NULL)  //'\'연산자가 존재하지 않을 경우
					return false; //false 리턴

				else{ //'\'연산자가 존재할 경우
					strncat(tokens[row], start, end - start + 1); //현재 토큰의 끝에 다음 연산자를 포함한 문자열을 붙인다.
					start = end + 1; //start를 다음 연산자 위치 다음으로 이동시킨다.
				}

			}
			
			//나머지 연산자들의 경우
			else{
				
				//현재 제일 처음 토큰이 아니고 전토큰이 "++"가 아닐 경우
				if(row > 0 && !strcmp(tokens[row - 1], "++"))
					return false; //false 리턴

				//현재 제일 처음 토큰이 아니고 전 토큰이 "--"가 아닐 경우
				if(row > 0 && !strcmp(tokens[row - 1], "--"))
					return false; //false 리턴
	
				strncat(tokens[row], start, 1); //현재 토큰 뒤에 start에서 한글자를 읽어서 붙인다.
				start += 1; //start를 뒤로 보낸다.
				
			
				//현재 토큰이 '-','+',"--","++" 중에 하나일 경우
				if(!strcmp(tokens[row], "-") || !strcmp(tokens[row], "+") || !strcmp(tokens[row], "--") || !strcmp(tokens[row], "++")){
				
					if(row == 0) //현재 제일 첫 토큰일 경우
						row--;    //한 토큰 전으로 돌아간다.

					//바로 전 토큰의 마지막 글자가 숫자 혹은 영문자일 경우
					else if(!is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
						
						//바로 전 토큰에 "++"과 "--"가 없다면 한 토큰 전으로 돌아간다.
						if(strstr(tokens[row - 1], "++") == NULL && strstr(tokens[row - 1], "--") == NULL)
							row--;
					}
				}
			}
		}
		else{ //연산자가 있는 위치가 start에 있지 않을 경우
			//바로 전 토큰이 모두 '*'로 이루어져 있고, 전전토큰의 마지막 글자가 숫자 혹은 영문자가 아닐 경우
			if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))   
				row--; //한 토큰 전으로 돌아간다.				
			
			//바로 전 토큰이 모두 '*'로 이루어져 있고, 지금 현재 두번째 토큰인 경우
			if(all_star(tokens[row - 1]) && row == 1)   
				row--; //한 토큰 전으로 돌아간다.	

			
			//다음으로 나오는 명령어의 위치에 도달할 때까지 반복
			for(i = 0; i < end - start; i++){
				//첫번째 반복이 아니고, start에서 i만큼 간 위치의 값이 '.'일 경우
				if(i > 0 && *(start + i) == '.'){
					strncat(tokens[row], start + i, 1); //현재 토큰 뒤에 start에서 i만큼 간 위치의 값을 붙인다.
					
					//'.'뒤의 문자가 공백이고, 현재 end까지 도달하지 않은 상태이면
					while( *(start + i +1) == ' ' && i< end - start )
						i++; //i를 하나 늘려준다.
				}
				else if(start[i] == ' '){ //start에서 i만큼 간 위치의 값이 공백일 경우
					while(start[i] == ' ') //공백이 끝날때까지
						i++;                //i를 하나씩 늘려준다.
					break;					  //공백이 끝났을 경우 반복을 멈춘다.
				}
				else//start에서 i만큼 간 위치의 값이 공백과 '.'이 아닐 경우
					strncat(tokens[row], start + i, 1); //현재 토큰의 끝에 start에서 i만큼 간 위치의 값을 붙여준다.
			}

			if(start[0] == ' '){ //start가 공백을 가리키고 있을 경우
				start += i;       //start를 i만큼 뒤로 옮긴다.
				continue;         //다음 차례의 반복으로 넘어간다.
			}
			start += i;          //start를 i만큼 뒤로 옮긴다.
		}
		
		strcpy(tokens[row], ltrim(rtrim(tokens[row]))); //현재 토큰의 좌 우측 공백을 잘라 현재 토큰을 최신화한다.

		//현재 첫 토큰이 아니고 현재토큰의 마지막 글자가 숫자 혹은 영문자일 경우
		//그리고 전토큰에 gcc 또는 데이터 타입이 적절한 위치에 있을 경우
		//또는 전 토큰의 마지막 글자가 숫자 혹은 영문자일 경우
		//또는 전 토큰의 마지막 글자가 '.'일 경우
		 if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.' ) ){
			
			//만약 두번째 토큰 이후이고, 전전 토큰이 '(' 일 경우
			if(row > 1 && strcmp(tokens[row - 2],"(") == 0)
			{
				//전 토큰이 "struct"와 "unsigned"가 아니라면
				if(strcmp(tokens[row - 1], "struct") != 0 && strcmp(tokens[row - 1],"unsigned") != 0)
					return false; //false 리턴
			}
			//만약 두번째 토큰이고, 현재 토큰의 마지막 글자가 숫자 혹은 영문자일 경우
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				//첫번째 토큰이 "extern"과 unsigned가 아니고 첫번째 토큰에 gcc 와 데이터 타입이 적절한 위치에 없을 경우
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false; //false 리턴
			}
			//만약 첫번째 토큰 이후이고, 전 토큰에 gcc 또는 데이터 타입이 적합한 위치에 있을 경우 
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				//전전토큰이 "unsigned"와 "extern"이 아닐 경우
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false; //false 리턴
			}
		}
		
		//현재 첫번째 토큰이고 현재 토큰이 "gcc"라면
		if((row == 0 && !strcmp(tokens[row], "gcc")) ){
			clear_tokens(tokens); //tokens 초기화해주기
			strcpy(tokens[0], str);	 //첫번째 토큰을 인자로 받은 문자열로 정해준다.
			return 1; //1 리턴
		} 

		row++; //다음 토큰으로 넘어가주기
	}
	
	//전단계의 토큰이 모두 '*'로 이루어져 있고, 현재 두번째 토큰 이후이고, 전전단계의 마지막 글자가 숫자 혹은 영문자가 아니라면
	if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))  
		row--; //한 토큰 전으로 돌아간다.
	//바로 전 토큰이 모두 '*'로 이루어져 있고, 현재 두번째 토큰일 경우	
	if(all_star(tokens[row - 1]) && row == 1)   
		row--; //한 토큰 전으로 돌아간다.

	//start가 가리키는 문자열 끝까지 반복
	for(i = 0; i < strlen(start); i++)   
	{
		if(start[i] == ' ')       //start에서 i번째 이후가 공백이라면
		{
			while(start[i] == ' ') //공백은 넘어가게
				i++;
			if(start[0]==' ') {    //start 시작주소가 공백이라면
				start += i;         //start를 공백이 끝나는 곳으로 이동
				i = 0;              //i를 0으로
			}
			else //start 시작주소가 공백이 아니라면
				row++; //한 토큰 전으로 돌아간다.
			
			i--; //i를 하나 전으로
		} 
		else //start에서 i번째 이후가 공백이 아니라면
		{
			//현재 토큰의 끝에 start에서 i번째에서 한글자를 읽어서 뒤에 붙여준다.
			strncat(tokens[row], start + i, 1);

			if( start[i] == '.' && i<strlen(start)){ //start에서 i번째의 값이 '.'이고, i가 문자열 끝까지 도달 안했을 경우
				//start에서 i번째의 값 바로 다음 값이 공백이고, i가 문자열 끝까지 도달 안했을 경우
				while(start[i + 1] == ' ' && i < strlen(start)) 
					i++; //i를 하나 증가시켜서 공백 무시하고 체크하게

			}
		}
		//현재 토큰에 현재 토큰의 좌 우측 공백을 모두 자른 문자열을 복사해서 최신해준다.
		strcpy(tokens[row], ltrim(rtrim(tokens[row]))); 
		
		//현재 토큰이 lpthread고, 현재 토큰이 첫번째가 아니고, 전 토큰이 '-'가 아닐 경우
		if(!strcmp(tokens[row], "lpthread") && row > 0 && !strcmp(tokens[row - 1], "-")){ 
			strcat(tokens[row - 1], tokens[row]);          //전 토큰의 끝에 현 토큰의 문자열을 붙여서 써준다.
			memset(tokens[row], 0, sizeof(tokens[row]));   //현재 토큰을 0으로 초기화해준다.
			row--; //한 토큰 전으로 돌아간다.
		}
		//현재 토큰이 첫번째가 아니고, 현재 토큰의 마지막 글자가 숫자 혹은 영문자이고,
		//전 토큰에 gcc 혹은 데이터타입이 적합한 위치에 있거나
		//전 토큰의 마지막 글자가 숫자 혹은 영문자이거나,
		//전 토큰의 마지막 글자가 '.'인 경우
	 	else if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.') ){
			
			//현 토큰이 두번째 토큰 이후이고, 전전토큰이 '('일 경우
			if(row > 1 && strcmp(tokens[row-2],"(") == 0)
			{
				//전토큰이 "struct"와 "unsigned"가 아닐 경우
				if(strcmp(tokens[row-1], "struct") != 0 && strcmp(tokens[row-1], "unsigned") != 0)
					return false; //false 리턴
			}
			//현 토큰이 두번째 토큰이고, 현 토큰의 마지막 글자가 숫자 혹은 영문자일 경우
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				//첫번째 토큰이 "extern"와 unsigned"가 아니고, gcc 또는 데이터 타입이 첫번째 토큰에서 적합한 위치에 있지 않을 경우
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false; //false 리턴
			}
			//현 토큰이 두번째 토큰 이후이고, 전 토큰에서 gcc 또는 데이터 타입이 적합한 위치에 있을 경우
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				//전전 토큰이 unsigned와 extern이 아닐 경우
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false; //false 리턴
			}
		} 
	}

	//첫번째 토큰이 아닐 경우
	if(row > 0)
	{
		//첫번째 토큰이 "#include"이거나 "include"이거나 "struct"인 경우
		if(strcmp(tokens[0], "#include") == 0 || strcmp(tokens[0], "include") == 0 || strcmp(tokens[0], "struct") == 0){ 
			clear_tokens(tokens);  //tokens를 초기화 한다.
			strcpy(tokens[0], remove_extraspace(str)); //첫번째 토큰에 공백을 제거한 인자로 받은 문자열을 넣는다.
		}
	}
	
	//첫번째 토큰에 gcc 혹은 데이터 타입이 적합한 위치에 있거나 첫번째 토큰에 "extern"이 있을 경우
	if(is_typeStatement(tokens[0]) == 2 || strstr(tokens[0], "extern") != NULL){
		for(i = 1; i < TOKEN_CNT; i++){  //토큰의 끝까지 반복
			if(strcmp(tokens[i],"") == 0) //토큰이 비어있을 경우 
				break;		               //반복 그만

			if(i != TOKEN_CNT -1 )        //현재 반복이 마지막 토큰이 아닐 경우
				strcat(tokens[0], " ");    //첫번째 토큰 끝에 공백을 넣어준다.
			strcat(tokens[0], tokens[i]); //첫번째 토큰 끈에 현재 토큰을 붙여준다.
			memset(tokens[i], 0, sizeof(tokens[i])); //현재 토큰을 0으로 초기화해준다.
		}
	}
	
	//p_str에 해당하는 인덱스의 토큰이
	//(토큰)& 거나  (토큰)- 거나
	//(토큰)* 거나  (토큰)+ 거나
	//(토큰)) 거나  (토큰)숫자 혹은 영문자면 반복
	while((p_str = find_typeSpecifier(tokens)) != -1){
		//괄호들을 없앤 tokens를 만드는데 실패했을 경우
		if(!reset_tokens(p_str, tokens))
			return false; //false 리턴
	}

	//p_str에 해당하는 인덱스의 토큰이
   //토큰이 "struct"고 토큰의 다음이 토큰의 끝 인덱스를 넘어가지 않고, 
	//다음 토큰의 마지막 글자가 숫자 혹은 영문자일 경우 반복
	while((p_str = find_typeSpecifier2(tokens)) != -1){ 
		//괄호들을 없앤 tokens를 만드는데 실패했을 경우
		if(!reset_tokens(p_str, tokens))
			return false; //false 리턴
	}
	
	return true; //모든 과정을 잘 마쳤을 경우 true 리턴
}

//토큰들로 트리를 만들고 루트를 리턴해주는 함수
node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses)
{
	node *cur = root;
	node *new;
	node *saved_operator;
	node *operator;
	int fstart;
	int i;

	while(1)	
	{	
		//현재 토큰이 공백일 경우
		if(strcmp(tokens[*idx], "") == 0)
			break; //반복 중지
		
		//현재 토큰이 닫는 괄호일 경우
		if(!strcmp(tokens[*idx], ")"))
			return get_root(cur); //루트 노드를 리턴해준다.
	
		//현재 토큰이 ','일 경우
		else if(!strcmp(tokens[*idx], ","))
			return get_root(cur); //루트 노드를 리턴해준다.

		//현재 토큰이 여는괄호일 경우
		else if(!strcmp(tokens[*idx], "("))
		{
			//현재 첫번째 토큰이 아니고, 전 토큰이 숫자 혹은 영문자가 아니고, 전 토큰이 ','가 아닐 경우
			if(*idx > 0 && !is_operator(tokens[*idx - 1]) && strcmp(tokens[*idx - 1], ",") != 0){
				fstart = true; //fstart를 true로 켜준다.

				while(1)
				{ 
					//다음 인덱스로 넘어간다.
					*idx += 1;
					
					//현재 토큰이 ')'일 경우 반복 멈춘다
					if(!strcmp(tokens[*idx], ")"))
						break; 
					
					//parentheses를 하나 늘려 트리를 다시한번 만들어주고, new에 그 루트를 저장해준다.
					new = make_tree(NULL, tokens, idx, parentheses + 1);
					
					if(new != NULL){ //트리가 만들어졌을 경우
						if(fstart == true){         //fstart가 true일 경우
							cur->child_head = new;   //현재 노드의 chile_head를 new로 해준다.
							new->parent = cur;       //new의 부로를 현재 노드로 해준다.
	
							fstart = false;          //fstart를 꺼준다.
						}
						else{ //트리가 만들어지지 않았을 경우
							cur->next = new; //현재노드의 next를 new로,
							new->prev = cur; //new노드의 prev를 현재 노드로 지정해준다.
						}

						cur = new; //현재노드에서 new 노드로 넘어간다.
					}

					if(!strcmp(tokens[*idx], ")")) //현재 토큰이 ')'일 경우 반복 그만한다.
						break;
				}
			}
			//현재 첫번째토큰이거나, 전 토큰이 숫자 혹은 영문자거나, 전 토큰이 ','일 경우
			else{
				*idx += 1; //다은 인덱스로 넘어간다.
					
				//parentheses를 하나 늘려 트리를 다시한번 만들어주고, new에 그 루트를 저장해준다.
				new = make_tree(NULL, tokens, idx, parentheses + 1);
				
				if(cur == NULL) //현재 노드가 아무것도 아닐 경우
					cur = new;   //new 노드를 현재 노드로 해준다.
				
				//new노드의 값이랑 cur 노드의 값이 동일할 경우
				else if(!strcmp(new->name, cur->name)){
					//new 노드의 값이 | 또는 || 또는 & 또는 &&일 경우
					if(!strcmp(new->name, "|") || !strcmp(new->name, "||") 
						|| !strcmp(new->name, "&") || !strcmp(new->name, "&&"))
					{
						//현재 노드의 제일 마지막 자식의 노드를 현재노드로 해준다.
						cur = get_last_child(cur);
						
						//만약 new노드의 자식노드가 존재할 경우
						if(new->child_head != NULL){
							new = new->child_head; //new노드의 자식노드를 new 노드로
							
							//new노드의 부모노드의 자식노드를 null로
							new->parent->child_head = NULL; 
							//new 노드의 부모 노드를 null로
							new->parent = NULL;
							//new 노드의 이전 형제노드를 cur 노드로
							new->prev = cur;
							//cur 노드의 다음 형제 노드를 new 노드로
							cur->next = new;
						}
					}
					//new 노드의 값이 +거나 *일 경우
					else if(!strcmp(new->name, "+") || !strcmp(new->name, "*"))
					{
						i = 0;

						while(1)
						{
							//현재토큰의 i번째 이후 토큰이 공백일 경우 반복 그만
							if(!strcmp(tokens[*idx + i], ""))
								break; 
							//현재토큰의 i번째 이후 토큰이 연산자이고, 그 연산자가 )가 아닐 경우 그만
							if(is_operator(tokens[*idx + i]) && strcmp(tokens[*idx + i], ")") != 0)
								break;

							i++; 
						}
						
						//현재 토큰의 i번째 이후 토큰이 연산자랑 new노드의 연산자의 우선순위를 비교해서
						//토큰의 연산자 우선순위가 더 높을 경우
						if(get_precedence(tokens[*idx + i]) < get_precedence(new->name))
						{
							//현재노드의 제일 마지막 자식노드를 현재 노드로 해준다.
							cur = get_last_child(cur);
							//현재 노드의 다음 형제 노드를 new 노드로 해준다.
							cur->next = new;
							//new 노드의 이전 형제 노드를 현재 노드로 해준다.
							new->prev = cur;
							//new 노드를 현재 노드로 해준다.
							cur = new;
						}
						//토큰의 연산자 우선순위가 더 낮을 경우
						else
						{
							//현재노드의 제일 마지막 자식의 노드를 현재 노드로
							cur = get_last_child(cur);
							
							//new노드의 자식노드가 존재할 경우
							if(new->child_head != NULL){
								//new노드의 자식을 new 노드로
								new = new->child_head;
								//new노드의 부모 노드의 자식노드를 null로
								new->parent->child_head = NULL;
								//new노드의 부모 노드를 null로
								new->parent = NULL;
								//new 노드의 이전 형제 노드를 현재 노드로
								new->prev = cur;
								//현재 노드의 다음 형제 노드를 new 노드로
								cur->next = new;
							}
						}
					}

					//new 노드의 값이 |, ||, &, &&, +, *가 아닐 경우
					else{
						//현재노드의 제일 마지막 자식의 노드를 현재 노드로
						cur = get_last_child(cur);
						//현재노드의 다음 형제의 노드를 new 노드로
						cur->next = new;
						//new 노드의 이전 형제의 노드를 현제 노드로
						new->prev = cur;
						//new 노드를 현재 노드로
						cur = new;
					}
				}
				//new노드의 값이랑 cur 노드의 값이 동일하지 않을 경우
				else
				{
					//현재노드의 제일 마지막 자식의 노드를 현재 노드로
					cur = get_last_child(cur);

					//현재노드의 다음 형제의 노드를 new 노드로
					cur->next = new;
					//new 노드의 이전 형제의 노드를 현제 노드로
					new->prev = cur;
	
					//new 노드를 현재 노드로
					cur = new;
				}
			}
		}
		//현재 토큰이 연산자일 경우
		else if(is_operator(tokens[*idx]))
		{
			//현재 토큰이 || 또는 && 또는 | 또는 & 또는 + 또는 *인 경우
			if(!strcmp(tokens[*idx], "||") || !strcmp(tokens[*idx], "&&")
					|| !strcmp(tokens[*idx], "|") || !strcmp(tokens[*idx], "&") 
					|| !strcmp(tokens[*idx], "+") || !strcmp(tokens[*idx], "*"))
			{
				//현재 노드가 연산자이고, 현재 노드의 연산자가 토큰과 일치할 경우
				if(is_operator(cur->name) == true && !strcmp(cur->name, tokens[*idx]))
					operator = cur; //operator를 현재 노드로 해준다.
				
				//현재 노드와 토큰이 일치하지 않을 경우
				else
				{
					//현재 토큰으로 노드를 생성한다.
					new = create_node(tokens[*idx], parentheses);
					//제일 우선순위 높은 노드를 operator가 가리키게 한다.
					operator = get_most_high_precedence_node(cur, new);
					
					//operator의 부모가 없고, 이전형제도 없을 경우
					if(operator->parent == NULL && operator->prev == NULL){
						
						//operator가 새로 만든 노드보다 우선순위 높을 경우
						if(get_precedence(operator->name) < get_precedence(new->name)){
							//new노드를 트리에 알맞은 위치에 넣고 그 노드를 현재 노드로
							cur = insert_node(operator, new);
						}
						//operator랑 새로 만들어서 들어온 노드 중 새로 들어온게 우선순위 높을 경우
						else if(get_precedence(operator->name) > get_precedence(new->name))
						{
							//operator의 자식노드가 존재할 경우
							if(operator->child_head != NULL){
								//operator를 그 노드의 제일 마지막 자식으로
								operator = get_last_child(operator);
								//new노드를 트리에 알맞은 위치에 넣고 그 노드를 현재 노드로
								cur = insert_node(operator, new);
							}
						}
						//operator랑 새로 들어온 노드랑 우선순위 똑같을 경우
						else
						{
							//현재 노드를 operator로
							operator = cur;
	
							while(1)
							{
								//operator 노드가 연산자이고, operator 노드의 값이랑 현재 토큰이랑 일치할 경우 그만
								if(is_operator(operator->name) == true && !strcmp(operator->name, tokens[*idx]))
									break; 

								//operator의 이전 형제노드가 존재할 경우 operator를 이전 형제 노드로
								if(operator->prev != NULL)
									operator = operator->prev;
								else //존재하지 않을 경우 그만
									break;
							}
							
							//operator의 값과 현재 토큰이 같지 않을 경우
							if(strcmp(operator->name, tokens[*idx]) != 0){
								//operator의 부모를 operator로
								operator = operator->parent;
							}
							
							//operator가 존재하는 노드일 경우
							if(operator != NULL){
								//operator의 값과 토큰이 일치할 경우 operator를 현재 토큰으로
								if(!strcmp(operator->name, tokens[*idx]))
									cur = operator;
							}
						}
					}

					//operator의 부모가 있거나, 이전형제가 있을 경우
					else{
						//new노드를 트리에 삽입하고, 그 노드를 현재 노드로
						cur = insert_node(operator, new);
					}
				}

			}
			//현재 토큰이 ||, &&, |, &, +, *가 아닐 경우
			else
			{
				//토큰을 노드로 생성한다.
				new = create_node(tokens[*idx], parentheses);

				//현재 노드가 아무것도 가리키지 않을 경우 new 가리키게
				if(cur == NULL)
					cur = new;
	
				//현재 노드가 가리키고 있는경우
				else
				{
					//operator가 제일 높은 우선순위를 갖는 노드를 가리키게
					operator = get_most_high_precedence_node(cur, new);
					
					//operator의 parentheses가 new의 parentheses보다 클 경우
					if(operator->parentheses > new->parentheses){
						//트리에 new노드를 삽입하고, 그 위치로 cur 노드를 이동한다.
						cur = insert_node(operator, new);
					}
					
					//operator의 부모가 없고, operator의 이전형제가 없을 경우
					else if(operator->parent == NULL && operator->prev ==  NULL){
						//operator가 new의 우선순위보다 낮을 경우
						if(get_precedence(operator->name) > get_precedence(new->name))
						{
							//operator의 자식이 있을 경우
							if(operator->child_head != NULL){
								//operator의 제일 마지막 자식을 operator가 가리키게
								operator = get_last_child(operator);
								//new 노드를 트리에 삽입하여 그 위치를 cur가 가리키게
								cur = insert_node(operator, new);
							}
						}
						//operator가 new의 우선순위보다 높은 경우
						else{
							//new 노드를 트리에 삽입하여 그 위치를 cur가 가리키게
							cur = insert_node(operator, new);
						}
					}
					//operator의 parentheses가 new의 parentheses보다 크지 않고
					//operator의 부모가 있거나, operator의 이전형제가 있을 경우
					else{
						//new 노드를 트리에 삽입하여 그 위치를 cur가 가리키게
						cur = insert_node(operator, new);
					}
				}
			}
		}
		//현재 토큰이 연산자가 아닐 경우
		else 
		{
			//현재 토큰으로 새로운 노드를 만들어준다.
			new = create_node(tokens[*idx], parentheses);
			
			//현재 노드가 없을 경우
			if(cur == NULL)
				cur = new; //새로운 노드를 현재 노드로 한다.
			
			//현재 노드의 자식이 없을 경우
			else if(cur->child_head == NULL){
				//현재 노드의 자식 노드를 new 노드로 한다.
				cur->child_head = new;
				//새로운 노드의 부모를 현재노드로 한다.
				new->parent = cur;

				cur = new; //새로운 노드를 현재 노드로 한다.
			}
			//현재 노드의 자식이 있을 경우
			else{
				//현재노드는 현재 노드의 제일 마지막 자식
				cur = get_last_child(cur);
				//현재노드의 다음 형제는 new 노드
				cur->next = new;
				//new 노드의 이전 형제는 현재 노드
				new->prev = cur;
				
				cur = new; //새로운 노드를 현재 노드로 한다.
			}
		}
		//다음 인덱스의 토큰으로 넘어간다.
		*idx += 1;
	}
	//현재 트리의 루트를 리턴한다.
	return get_root(cur);
}

//노드를 받아서 자식노드의 형제노드들을 변경해준다.
node *change_sibling(node *parent)
{
	node *tmp;
	
	//tmp에 인자로받은 노드의 자식을 저장한다.
	tmp = parent->child_head;
	
	//인자로 받은 노드의 자식 노드를 자식노드의 다음형제 노드로
	parent->child_head = parent->child_head->next;
	//인자로 받은 노드의 자식 노드의 부모를 인자로 받은 노드로
	parent->child_head->parent = parent;
	//인자로 받은 노드의 자식 노드의 이전 형제 노드를 null로
	parent->child_head->prev = NULL;

	//인자로 받은 노드의 자식노드의 다음 형제 노드를 인자로 받은 노드의 자식으로
	parent->child_head->next = tmp;
	//인자로 받은 노드의 자식노드의 다음 형제 노드의 이전 노드를 인자로 받은 노드의 자식노드로
	parent->child_head->next->prev = parent->child_head;
	//인자로 받은 노드의 자식노드의 다음 형제 노드의 다음 형제 노드를 null로
	parent->child_head->next->next = NULL;
	//인자로 받은 노드의 자식노드의 다음 형제 노드의 부모 노드를 null로
	parent->child_head->next->parent = NULL;		

	//바뀐 parent를 리턴
	return parent;
}

//토큰으로 노드를 생성하는 함수
node *create_node(char *name, int parentheses)
{
	node *new;
	
	new = (node *)malloc(sizeof(node)); //새 노드 자리 동적할당
	//새 노드의 값 넣을 자리를 동적할당
	new->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
	//토큰을 새 노드의 값으로 복사한다.
	strcpy(new->name, name);

	//new 노드의 parentheses를 인자로 받은 parentheses로
	new->parentheses = parentheses;
	new->parent = NULL;      //new노드의 부모 없고
	new->child_head = NULL;  //자식 없고
	new->prev = NULL;        //이전형제 없고
	new->next = NULL;        //다음형제 없게 만들어준다.

	return new; //생성한 노드를 리턴
}

//토큰을 받아와서 토큰이 연산자일 경우 연산자의 우선순위를 리턴해주는 함수
int get_precedence(char *op)
{
	int i;

	for(i = 2; i < OPERATOR_CNT; i++){
		if(!strcmp(operators[i].operator, op))
			return operators[i].precedence; //연산자의 우선순위 리턴
	}
	return false; //연산자 아닐 경우 false 리턴
}

//토큰을 받아와서 그 토큰이 연산자인지 체크해주는 함수
int is_operator(char *op)
{
	int i;

	for(i = 0; i < OPERATOR_CNT; i++)
	{
		if(operators[i].operator == NULL) //연산자가 아닐경우
			break;       //반복문 나오기
		if(!strcmp(operators[i].operator, op)){ //연산자일 경우
			return true; //true 리턴
		}
	}

	return false; //연산자 아닐 경우 false 리턴
}

/*/////////////////////////삭제////////////////////////////////
void print(node *cur)
{
	if(cur->child_head != NULL){
		print(cur->child_head);
		printf("\n");
	}

	if(cur->next != NULL){
		print(cur->next);
		printf("\t");
	}
	printf("%s", cur->name);
}
///////////////////////////////////////////////////////////// */

//노드를 인자로 받아와서
node *get_operator(node *cur)
{
	if(cur == NULL) //현재 노드가 null일 경우
		return cur;  //현재 노드 리턴

	if(cur->prev != NULL) //현재 노드의 이전 형제 노드가 존재할 경우
		while(cur->prev != NULL) //이전 형제 노드가 존재하는 동안
			cur = cur->prev; //현재 노드를 이전 형제 노드로

	return cur->parent; //현재 노드의 부모노드 리턴
}

//루트 노드를 리턴해주는 함수
node *get_root(node *cur)
{
	if(cur == NULL) //현재 아무런 노드도 가리키지 않고 있을 경우
		return cur;  //현재 노드를 리턴

	while(cur->prev != NULL) //prev 노드가 존재하는 동안
		cur = cur->prev;      //현재 노드를 prev노드로 전환

	if(cur->parent != NULL) //현재 노드의 부모노드가 있을 경우
		cur = get_root(cur->parent); //현재 노드를 부모노드의 루트노드로 전환

	return cur; //현재 노드를 리턴
}

//현재 노드와 new 노드를 인자로 받아와서 높은 우선순위 갖는 노드 리턴
node *get_high_precedence_node(node *cur, node *new)
{
	//현재 노드의 값이 연산자라면
	if(is_operator(cur->name))
		//현재 노드의 연산자랑 new노드의 연산자의 우선순위를 비교하여
		//현재 노드의 연산자 우선순위가 더 높을 경우
		if(get_precedence(cur->name) < get_precedence(new->name))
			return cur; //현재 노드 리턴
	
	//현재 노드의 이전형제 노드가 존재할 경우
	if(cur->prev != NULL){
		while(cur->prev != NULL){ //현재 노드의 이전 형제 노드가 존재하는 한 반복
			cur = cur->prev; //현재 노드를 이전 형제 노드로 해준다.
			
			//현재 노드와 new노드중에서 우선순위 높은 노드를 리턴
			return get_high_precedence_node(cur, new);
		}

		//현재 노드의 부모노드가 존재할 경우
		if(cur->parent != NULL){
			//부모노드와 new 노드 중에서 우선순위 높은 노드를 리턴
			return get_high_precedence_node(cur->parent, new);
		}
	}
	
	//현재 노드의 부모노드가 존재하지 않을 경우 현재노드 리턴
	if(cur->parent == NULL)
		return cur;
}

//현재 노드와 new 노드를 인자로 받아와서
//트리에서 제일 우선순위 노픈 연산자 리턴
node *get_most_high_precedence_node(node *cur, node *new)
{
	//연산자 저장하는 노드를 현재 노드와 new 노드중에서 우선순위 높은 노드로
	node *operator = get_high_precedence_node(cur, new);
	//임시로 연산자 저장하는 노드가 연산자 저장하는 노드 가리키게
	node *saved_operator = operator;

	while(1)
	{
		//연산자의 부모 노드가 존재하지 않을 경우 그만
		if(saved_operator->parent == NULL)
			break;

		//연산자의 이전형제 노드가 존재할 경우
		if(saved_operator->prev != NULL){
			//연산자의 이전형제 노드랑 new 노드 중 우선순위 높은 연산자 operator가 가리키게
			operator = get_high_precedence_node(saved_operator->prev, new);
		}
		//연산자의 부모노드가 존재할 경우
		else if(saved_operator->parent != NULL){
			//연산자의 부모노드랑 new 노드 중 우선순위 높은 연산자 operator가 가리키게
			operator = get_high_precedence_node(saved_operator->parent, new);
		}
		//saved_operator가 우선순위 높은 연산자 가리키게
		saved_operator = operator;
	}
	//최종적으로 제일 우선순위 높은 연산자 리턴
	return saved_operator;
}

//두 노드를 인자로 받아와서 new 노드를 트리에 껴주는 함수
node *insert_node(node *old, node *new)
{
	//old의 이전 형제가 존재할 경우
	if(old->prev != NULL){
		new->prev = old->prev; //new 노드의 이전형제를 old의 이전형제로
		old->prev->next = new; //old 노드의 이전형제의 다음형제를 new 노드로
		old->prev = NULL;      //old 노드의 이전형제를 null로
	}

	new->child_head = old; //new의 자식은 old
	old->parent = new; //old의 parent는 new로

	return new; //new 리턴
}

//현재 노드에서의 제일 마지막 자식의 노드를 리턴해주는 함수
node *get_last_child(node *cur)
{
	//인자로 받은 노드에 child_head 노드가 존재할 경우
	if(cur->child_head != NULL) 
		cur = cur->child_head; //현재 노드의 child_head노드를 현재 노드로

	//인자로 받은 노드에 next 노드가 존재하는 동안
	while(cur->next != NULL) //현재 노드의 next 노드를 현재 노드로
		cur = cur->next;

	return cur; //현재 노드 리턴
}

//노드를 받아서 전체 형제노드 개수 리턴해주는 함수
int get_sibling_cnt(node *cur)
{
	int i = 0;
	//전형제 노드가 있을 경우
	while(cur->prev != NULL){
		//현재 노드를 전 노드로
		cur = cur->prev;
	}
	//다음 형제 노드가 있을 경우
	while(cur->next != NULL){
		//현재 노드를 다음 노드로
		cur = cur->next;
		i++; //형제 노드들 개수 카운트 올리기
	}

	return i; //형제 노드들 개수 리턴
}

//노드를 인자로 받아
void free_node(node *cur)
{
	//자식 노드가 존재할 경우 자식 노드를 비워준다.
	if(cur->child_head != NULL){
		free_node(cur->child_head);
	}
	//인자로 받은 노드의 다음 형제노드가 존재할 경우 다음 형제 노드를 비워준다.
	if(cur->next != NULL)
		free_node(cur->next);
	
	//인자로 받은 노드가 존재할 경우
	if(cur != NULL){
		cur->prev = NULL; //현재 노드의 이전 형제노드 비우기
		cur->next = NULL; //현재 노드의 다음 형제노드 비우기
		cur->parent = NULL; //현재 노드의 부모노드를 비운다.
		cur->child_head = NULL; //현재 노드의 자식노드 비우기
		free(cur); //현재 노드의 할당 풀어준다.
	}
}

//해당 문자가 0~9, a~z, A~Z안에 속하는 문자인지 판단하는 함수
int is_character(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

//문자열을 인자로 받아와서
//인자에서 gcc와 데이터타입 체크하는 함수,
//gcc가 써져있거나, 데이터타입이 써져있을경우 2리턴
//gcc가 이상한 위치에 있거나, 데이터타입이 이상한 위치에 있을 경우 0 리턴
//gcc와 데이터타입이 없는 문자열이었을 경우 1리턴
//gcc가 적합한 위치에 있거나, 데이터타입이 적합한 위치에 있을 경우 2 리턴
int is_typeStatement(char *str)
{ 
	char *start;
	char str2[BUFLEN] = {0}; 
	char tmp[BUFLEN] = {0}; 
	char tmp2[BUFLEN] = {0}; 
	int i;	 
	
	start = str;                     //문자열의 시작주소를 start가 가리키게
	strncpy(str2,str,strlen(str));   //인자로 받은 문자열을 str2에 복사
	remove_space(str2);              //str2에 저장된 문자열의 공백을 다 지워준다/

	//start가 공백을 뛰어넘어 str의 첫 글자를 가리키게
	while(start[0] == ' ') //start의 제일 첫째값이 공백인 경우
		start += 1;         //start가 그 다음위치 가리키게

	if(strstr(str2, "gcc") != NULL) //gcc가 str2에 있을경우
	{
		strncpy(tmp2, start, strlen("gcc")); //tmp2에 start로부터 세글자 복사
		if(strcmp(tmp2,"gcc") != 0)          //tmp2가 gcc가 아니라면
			return 0;                         //0리턴
		else                                 //tmp2가 gcc라면
			return 2;                         //2리턴
	}
	
	for(i = 0; i < DATATYPE_SIZE; i++) //데이터타입이 저장된 문자열배열 모두 검사
	{
		if(strstr(str2,datatype[i]) != NULL) //str2에서 해당하는 데이터타입 찾았을 때
		{	
			//tmp에 st2에서 그 데이터타입의 문자열 길이만큼 복사해서 써주고
			strncpy(tmp, str2, strlen(datatype[i]));   
			//tmp2에 start에서 그 데이터타입의 문자열 길이만큼 복사해서 써주고
			strncpy(tmp2, start, strlen(datatype[i])); 
			
			//만약에 tmp에 저장된 문자열이랑 데이터타입이 동일할 경우
			if(strcmp(tmp, datatype[i]) == 0)
				if(strcmp(tmp, tmp2) != 0) //tmp1 이랑 tmp2 비교해서 다르면
					return 0;               //0 리턴
				else                       //같으면
					return 2;               //2 리턴
		}

	}
	return 1;

}

//토큰들이 저장된 tokens를 인자로 받아
//조건에 맞는 토큰의 인덱스를 리턴
int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]) 
{
	int i, j;

	//토큰을 끝까지 반복
	for(i = 0; i < TOKEN_CNT; i++)
	{
		for(j = 0; j < DATATYPE_SIZE; j++) //데이터 타입을 검사하는데
		{
			//첫번째 토큰이 아닌 토큰에서 데이터 타입을 찾았을 경우
			if(strstr(tokens[i], datatype[j]) != NULL && i > 0) 
			{
				//(현재 토큰)& 거나  (현재토큰)- 거나
				//(현재 토큰)* 거나  (현재토큰)+ 거나
				//(현재 토큰)) 거나  (현재토큰)숫자혹은영문자 일 경우
				if(!strcmp(tokens[i - 1], "(") && !strcmp(tokens[i + 1], ")") 
						&& (tokens[i + 2][0] == '&' || tokens[i + 2][0] == '*' 
							|| tokens[i + 2][0] == ')' || tokens[i + 2][0] == '(' 
							|| tokens[i + 2][0] == '-' || tokens[i + 2][0] == '+' 
							|| is_character(tokens[i + 2][0])))  
					return i; //현재 인덱스 리턴
			}
		}
	}
	return -1; //위에 조건 하나도 일치 안할경우 -1 리턴
}

//토큰들이 저장된 tokens를 인자로 받아
//조건에 맞는 토큰의 인덱스 리턴
int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]) 
{
    int i, j;

    //토큰을 끝까지 반복
    for(i = 0; i < TOKEN_CNT; i++)
    {		
        for(j = 0; j < DATATYPE_SIZE; j++) //데이터 타입을 검사하는데
        {	
			   //현재토큰이 "struct"고 i+1이 토큰의 끝 인덱스를 넘어가지 않고, 다음 토큰의 마지막 글자가 숫자 혹은 영문자일 경우
            if(!strcmp(tokens[i], "struct") && (i+1) <= TOKEN_CNT && is_character(tokens[i + 1][strlen(tokens[i + 1]) - 1]))  
                    return i; //현재 인덱스 리턴
        }
    }
    return -1;
}

//인자로 문자열 받아서 그 문자열이 모두 *로 이루어져 있는지
int all_star(char *str)
{
	int i;
	int length= strlen(str); //그 문자열의 길이 구해서 저장
	
 	if(length == 0) //문자열이 비어있을 경우
		return 0;    //0리턴
	
	for(i = 0; i < length; i++) //문자열을 탐색하면서
		if(str[i] != '*')        //'*'가 아닐 경우
			return 0;             //0 리턴
	return 1;       //모두 '*'로 이루어져 있을 경우 1 리턴

}

/*//////////////////////////삭제////////////////////////////////
int all_character(char *str)
{
	int i;

	for(i = 0; i < strlen(str); i++)
		if(is_character(str[i]))
			return 1;
	return 0;
	
}
///////////////////////////////////////////////////////////// */


//find_typeSpecifier또는 find_typeSpecifier의 조건에 맞는 토큰의 인덱스와 tokens를 인자로 받아
//괄호를 빼주고 토큰들을 만들어주는 함수
int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]) 
{
	int i;
	int j = start - 1; //j는 start바로 전 인덱스
	int lcount = 0, rcount = 0;
	int sub_lcount = 0, sub_rcount = 0;

	if(start > -1){ //start가 최소 첫번째 토큰일 경우
		if(!strcmp(tokens[start], "struct")) {	    //start인덱스의 토큰이 "struct"일 경우	
			strcat(tokens[start], " ");             //start인덱스의 토큰 끝에 공백을 붙인다.
			strcat(tokens[start], tokens[start+1]); //start인덱스의 토큰 끝에 그 다음 토큰을 붙인다.	     

			for(i = start + 1; i < TOKEN_CNT - 1; i++){ //start인덱스의 토큰 다음부터 토큰의 끝 하나 전까지
				strcpy(tokens[i], tokens[i + 1]);        //현재 토큰에 그 다음 토큰을 복사한다.
				memset(tokens[i + 1], 0, sizeof(tokens[0])); //다음 토큰을 초기화해준다.
			}
		}
		//start인덱스의 토큰이 "unsigned고 그 다음 토큰이 ')'이 아닐 경우
		else if(!strcmp(tokens[start], "unsigned") && strcmp(tokens[start+1], ")") != 0) {		
			strcat(tokens[start], " "); //start인덱스의 토큰 끝에 공백을 붙인다.
			strcat(tokens[start], tokens[start + 1]);	 //현재 토큰 뒤에 그 다음 토큰 붙이고    
			strcat(tokens[start], tokens[start + 2]);  //그 다음 토큰의 다음 토큰도 붙인다.

			for(i = start + 1; i < TOKEN_CNT - 1; i++){ //start인덱스의 토큰 다음부터 토큰의 끝 하나 전까지
				strcpy(tokens[i], tokens[i + 1]);        //현재 토큰에 그 다음 토큰을 복사한다.
				memset(tokens[i + 1], 0, sizeof(tokens[0])); //그 다음 토큰을 초기화해준다.
			}
		}

     	j = start + 1; //j를 start 바로 다음 인덱스로 지정해준다.           
     	while(!strcmp(tokens[j], ")")){ //닫는 괄호가 나올때마다 반복
        	rcount ++; //우측괄호 카운트를 증가시켜준다.
        	if(j==TOKEN_CNT) //j가 토큰들 끝에 도달하면
          	break;        //그만
       	j++; //j가 다음 토큰을 가리키게
      }
	
		j = start - 1; //j를 start 바로 전 인덱스로 지정해준다.
		while(!strcmp(tokens[j], "(")){ //여는 괄호가 나올 때마다 반복
        	        lcount ++; //좌측괄호 카운트를 증가시켜준다.
                	if(j == 0) //제일 첫 인덱스로 도착했을 경우
                        	break; //그만
               		j--; //j가 그 전 토큰을 가리키게
		}
		//j가 제일 첫인덱스도 아니고, 인덱스 j의 토큰의 마지막 글자가 숫자 혹은 영문자이거나 j가 제일 첫 인덱스인 경우
		if( (j!=0 && is_character(tokens[j][strlen(tokens[j])-1]) ) || j==0)
			lcount = rcount; //좌측괄호 카운트를 우측괄호 카운트와 동일하게
		
		//여는괄호의 개수와 닫는 괄호의 개수가 다를 경우
		if(lcount != rcount )
			return false; //false 리턴

		//start인덱스에서 여는괄호 개수만큼 빼도 0보다 크고,
		//start인덱스에서 여는괄호 개수만큼 뺀거의 전 인덱스의 토큰이 "sizeof"인 경우
		if( (start - lcount) >0 && !strcmp(tokens[start - lcount - 1], "sizeof")){
			return true; //true 리턴
		}
		//start인덱스의 토큰이 unsigned이거나, struct이고 다음토큰이 ')'인 경우
		else if((!strcmp(tokens[start], "unsigned") || !strcmp(tokens[start], "struct")) && strcmp(tokens[start+1], ")")) {		
			//start인덱스에서 여는괄호 개수만큼 뺀거의 인덱스의 토큰에 start인덱스의 토큰을 붙인다.
			strcat(tokens[start - lcount], tokens[start]); 
			//start인덱스의 토큰을 붙인 start인덱스에서 여는괄호 개수만큼 뺀거의 인덱스의 토큰에 start인덱스의 다음 인덱스의 토큰을 붙인다.
			strcat(tokens[start - lcount], tokens[start + 1]);
			//start인덱스에서 여는괄호 개수만큼 뺀거의 인덱스의 토큰 다음 토큰에 start인덱스에서 닫는괄호 개수만큼 더한 인덱스의 토큰을 복사한다.
			strcpy(tokens[start - lcount + 1], tokens[start + rcount]);
		 	
			//start인덱스에서 좌측괄호의 개수만큼 뺀 인덱스의 다음 인덱스부터 토큰의 끝에서 괄호 개수들 다 뺀 인덱스까지
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount; i++) {
				//현재 인덱스에 현재 인덱스에서 좌측괄호 우측괄호 개수 더한만큼 더 간 인덱스의 값을 복사해준다.
				strcpy(tokens[i], tokens[i + lcount + rcount]);
				//현재 인덱스에서 좌측괄호 우측괄호 개수 더한만큼 더 간 인덱스의 값을 초기화해준다.
				memset(tokens[i + lcount + rcount], 0, sizeof(tokens[0]));
			}

		}
		//위의 두 조건에 걸리지 않은 경우
 		else{
			if(tokens[start + 2][0] == '('){ //start인덱스의 다다음 토큰의 첫글자가 '('인 경우
				j = start + 2; //j를 start인덱스의 다다음 인덱스로 지정
				while(!strcmp(tokens[j], "(")){ //j인덱스의 토큰의 값이 '('인 경우 반복한다
					sub_lcount++; //sub_lcount하나 증가시킨다.
					j++; //다음 인덱스로
				}
				//j인덱스의 토큰의 바로 다음 토큰이 ")"일 경우
				if(!strcmp(tokens[j + 1],")")){
					j = j + 1; //j를 다음 인덱스로 이동
					while(!strcmp(tokens[j], ")")){ //j인덱스의 토큰의 값이 ')'인 경우 반복한다.
						sub_rcount++; //sub_rcount 하나 증가시킨다.
						j++; //다음 인덱스로
					}
				}
				//둘다 아닌 경우 false 리턴
				else 
					return false; 
				
				//sub_lcount의 값이랑 sub_rcount의 값이 다를 경우 false 리턴
				if(sub_lcount != sub_rcount) 
					return false;
				
				//start인덱스의 다다음번째 토큰에 start인덱스의 다다음번째 토큰에서 sub_lcout만큼 더 간 인덱스의 토큰을 복사해준다.
				strcpy(tokens[start + 2], tokens[start + 2 + sub_lcount]);	
				for(int i = start + 3; i<TOKEN_CNT; i++) //start인덱스의 다다다음 인덱스부터 끝까지
					memset(tokens[i], 0, sizeof(tokens[0])); //해당 인덱스를 초기화해준다.

			}
			//start인덱스에 좌측괄호 개수만큼 빼준 인덱스의 토큰에 start의 토큰을 붙여준다.
			strcat(tokens[start - lcount], tokens[start]);
			//추가로, start인덱스의 바로 다음 인덱스의 토큰도 붙여준다.
			strcat(tokens[start - lcount], tokens[start + 1]);
			//추가로, start인덱스에서 닫는괄호 개수만큼 더한 인덱스의 다음 인덱스의 토큰도 붙여준다.
			strcat(tokens[start - lcount], tokens[start + rcount + 1]);
		 
		 	//start에서 좌측괄호 개수만큼 빼준 인덱스의 다음부터 전체 토큰 개수에서 괄호개수 뺀거의 하나 전까지 반복
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount -1; i++) {
				//현재 토큰에 현재토큰에서 좌측괄호 우측괄호 더한 인덱스의 다음 토큰을 복사해준다.
				strcpy(tokens[i], tokens[i + lcount + rcount +1]);
				//복사를 해준 그 토큰을 초기화해준다.
				memset(tokens[i + lcount + rcount + 1], 0, sizeof(tokens[0]));

			}
		}
	}
	return true; //정상적으로 중간에 리턴되지 않았다면 true 리턴
}

//tokens 배열을 인자로 받아와서
void clear_tokens(char tokens[TOKEN_CNT][MINLEN])
{
	int i;
	//tokens 배열의 모든 행을 0으로 초기화해준다.
	for(i = 0; i < TOKEN_CNT; i++)
		memset(tokens[i], 0, sizeof(tokens[i]));
}

//문자열을 입력받아서 오른쪽 공백 자르는 함수
char *rtrim(char *_str)
{
	char tmp[BUFLEN];
	char *end;

	strcpy(tmp, _str); //인자로 받은 문자열을 tmp에 복사
	end = tmp + strlen(tmp) - 1; //end의 위치를 tmp에 복사된 문자열의 끝으로 설정
	while(end != tmp && isspace(*end)) //end의 위치의 값이 공백이고 맨 앞의 위치가 아니면 반복
		--end; //end의 위치를 하나 앞으로 땡긴다.

	*(end + 1) = '\0'; //end의 다음 위치에 널문자 삽입
	_str = tmp;        //_str에 오른쪽 공백 자른값을 넣어준다.
	return _str;       //오른쪽 공백을 자른 _str을 리턴
}

//문자열을 입력받아서 왼쪽 공백 자르는 함수
char *ltrim(char *_str)
{
	char *start = _str; //_str의 시작주소를 start가 가리키게 한다.

	//start의 위치의 값이 공백이고, 맨 끝에 도달하기 전까지 반복
	while(*start != '\0' && isspace(*start)) 
		++start; //start를 다음칸으로 넘긴다.
	_str = start; //_str의 시작주소를 공백이 끝난 뒤 시작한 위치로 옮긴다.
	return _str;  //왼쪽의 공백을 제거한 _str을 리턴한다.
}

//문자열을 입력받아서 그 문자열의 공백을 다 없애주는 함수
char* remove_extraspace(char *str)
{
	int i;
	char *str2 = (char*)malloc(sizeof(char) * BUFLEN); 
	char *start, *end;
	char temp[BUFLEN] = "";
	int position;

	if(strstr(str,"include<")!=NULL){ //문자열에 "include<"가 존재할 경우
		start = str;                   //start가 str의 시작주소 가리키게
		end = strpbrk(str, "<");       //end가 str에서 "<" 처음 시작하는 주소 가리키게
		position = end - start;        //start에서 end까지의 차이만큼 position에 저장
	
		strncat(temp, str, position);  //temp와 str의 position만큼의 문자열을 연결한다.
		strncat(temp, " ", 1);         //temp 끝에 공백 하나를 붙인다.
		//temp 끝에 end부터 끝까지의 문자열을 붙인다.
		strncat(temp, str + position, strlen(str) - position + 1);

		str = temp;	//str을 temp로 바꿔준다.
	}
	
	//처음부터 str의 끝까지 반복
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] ==' ') //공백이 나왔을 경우
		{
			if(i == 0 && str[0] ==' ') //처음 시작이 공백일 경우
				while(str[i + 1] == ' ') //공백이 나올때마다 패스
					i++;	//넘어가게
			else{ //처음 시작이 공백이 아닐 경우
				//첫번째 글자가 아닌 글자들에 대해 전 글자가 공백이 아니라면
				if(i > 0 && str[i - 1] != ' ')
					str2[strlen(str2)] = str[i]; //str2 끝에 str의 i번째 글자 추가한다.
				while(str[i + 1] == ' ') //다음이 공백이라면
					i++; //넘어가게
			} 
		}
		else //공백이 아닐경우
			str2[strlen(str2)] = str[i]; //str2의 끝에 str의 i번째 글자 추가한다.
	}

	return str2; //공백을 싹 제거한 str2를 리턴한다.
}


//문자열을 인자로 받아와서 문자열의 공백을 다 지워주는 함수
void remove_space(char *str)
{
	char* i = str; //i가 str의 시작주소를 가리키게
	char* j = str; //j가 str의 시작주소를 가리키게
	
	while(*j != 0) //j가 NULL문자를 가르치지 않으면 반복
	{
		*i = *j++;     //i가 가리키는 곳에 j가 가리키는 곳 값 넣고 j가 다음거 가리키게
		if(*i != ' ')  //i가 가리키는 값이 공백이 아니면
			i++;        //i를 다음칸으로
	}
	*i = 0;           //i자리에 널문자를 넣는다.
}

//인자로 받은 문자열의 여는 괄호와 닫는 괄호 개수가 동일한지 체크해주는 함수
int check_brackets(char *str)
{
	char *start = str;
	int lcount = 0, rcount = 0;
	
	while(1){ //인자로 받은 문자열에서 ( 또는 )이 제일 처음 나온 위치를 start로 설정
		if((start = strpbrk(start, "()")) != NULL){
			if(*(start) == '(') //start 위치의 값이 ( 일 경우
				lcount++;        //왼쪽 괄호 카운트 증가
			else                //) 일 경우
				rcount++;        //오른쪽 괄호 카운트 증가

			start += 1; 		  //start위치를 다음칸으로 넘겨준다.
		}
		else
			break;              //더이상 ( 또는 ) 을 읽지 못했을 경우 반복 끝
	}

	if(lcount != rcount)      //왼쪽 괄호 개수와 오른쪽 괄호 개수가 같지 않다면
		return 0;              //false 리턴
	else                      //같다면
		return 1;              //true 리턴
}

/*//////////////////////////////////////삭제///////////////////////////////////////
int get_token_cnt(char tokens[TOKEN_CNT][MINLEN])
{
	int i;
	
	for(i = 0; i < TOKEN_CNT; i++)
		if(!strcmp(tokens[i], ""))
			break;

	return i;
}
//////////////////////////////////////////////////////////////////////////////// */
