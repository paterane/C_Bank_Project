//
// Created by Peter Oo on 03/11/2023
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#define true 1
#define false 0

int tkIndex = 0; //global index for string to token func
char *tkString = NULL; // global string buffer for token

/***************************************************************************************/
//////////////////////////////////Utility Functions//////////////////////////////////////
/***************************************************************************************/
/*
  make the given string lower
  and, Return char pointer
*/
char *wordLower(char *word){
    int i = 0;
    int len = stringLen(word)+1;
    char *buff = (char*)malloc(len*sizeof(char));
    while(word[i] != '\0'){
        if(word[i] >= 'A' && word[i] <= 'Z') buff[i] = word[i] + 32;
        else buff[i] = word[i];
        i++;
    }
    buff[i] = '\0';
    return buff;
}
/* Note: This func is only valid for char array, not for int array */
int stringLen(const char *str){
    int n=0;
    while(str[n++] != '\0');
    return n-1;
}
/* RETURN: 0 if not match , 1 if match*/
int stringCmp(char *str1, char *str2){
    if(stringLen(str1) == stringLen(str2)){ //if length same, pass OR terminate func and return -1
        while(*str1 != '\0'){
            if(*str1++ != *str2++)
                return 0;
        }
        return 1;  //if every character is matched, return cnt
    }
    else
        return 0;
}
/*
 * Copy the content of src to dst
 * */
void stringCopy(char *src, char *dst){
    int len = stringLen(src);
    for(int i=0; i<len; i++){
        dst[i] = src[i];
    }
    dst[len] = '\0';
}
// RETURN: -1 if indexString longer than the string or not found,and index position if found
// NOTE: this function will only work on first occurrence of the index.
int indexOf(char *str, char *idxStr){
    int cnt=0; int found=0;
    int str_len = stringLen(str);
    int idxStr_len = stringLen(idxStr);
    if(idxStr_len <= str_len){
        for(;cnt < (str_len+idxStr_len); cnt++){
            if(idxStr[found] == str[cnt]){
                found++;
                if(found == idxStr_len)
                    break;
            }
            else if(found){
                cnt--;
                found=0;
            }
        }
        if(found == idxStr_len)
            return cnt-(found-1);
        else
            return -1; // if not found
    }
    else{
        printf("Index length longer than the string!\n");
        return -1;
    }
}
/* continuous characters trimming in the string
 * continuous characters started from idxA upto idxB excluding character at idxB
 * RETURN: NULL if error indexing
 * */
char *subString(char *str, int idxA, int idxB){
    int length = stringLen(str); int x = 0;
    char *result = (char*)(malloc(length*sizeof(char)));
    if(idxA >= length || idxB > (length+1) || idxA >= idxB){
        printf("[ERROR!] indexing invalid!!!\n");
        return NULL;
    }
    for(int i=idxA; i<idxB; i++){
        result[x++] = str[i];
    }
    result[x] = '\0';
    return result;
}
/* number of occurrence of a substring "x" or "x.."
 * RETURN:-1 if error, 0 if not found, count of the occurrence if found
 * */
int stringCount(char *str, char *idxStr){
    int cnt=0; int found=0; int n_occur = 0;
    int str_len = stringLen(str);
    int idxStr_len = stringLen(idxStr);
    if(idxStr_len > str_len){
        printf("[Error!] index > string\n");
        return -1; // if error
    }
    for(;cnt < (str_len+idxStr_len); cnt++){
        if(idxStr[found] == str[cnt]){
            found++;
            if(found == idxStr_len){ //not to exceed idxStr len
                n_occur++;
                found=0;
            }
        }
        else if(found){
            cnt--;
            found=0;
        }
    }
    return n_occur;
}
/*
 * combination of main string and sub string in such a way that sub join at the end of main*/
void stringConcat(char **main, char *sub){
    int main_len = stringLen(*main);
    int sub_len = stringLen(sub);
    int total_len = main_len+sub_len;
    char *temp = (char*)(malloc((total_len+1)*sizeof(char)));
    for(int i=0; i < main_len;i++){
        temp[i] = *(*main+i); //or main[0][i],but, can't do like this *main[i], haha, WTF "C"
    }
    for(int i=0; i<sub_len; i++){
        temp[main_len+i] = sub[i];
    }
    temp[total_len] = '\0';
    *main = temp;
}
/*
    Taken the next row as string, iterating through the
    given string, whenever the next delimiter is met, a token started from previous index until delimiter index
    is returned.
    If iteration is finished, and end of string, NULL is returned.
*/
char *readLine_csv(char *str, char dl){
    char qt = '"';
    int len = stringLen(str);
    char *buff;
    if(tkString == NULL){
        tkString = (char*)malloc((len+1)*sizeof(char));
        stringCopy(str, tkString);
    }
    if(!stringCmp(str, tkString)){
        tkString = (char*)realloc(tkString, (len+1)*sizeof(char));
        stringCopy(str, tkString);
        tkIndex = 0;
    }
    else{
        if(tkIndex >= len){
            tkIndex = 0;
            return NULL;
        }
    }
    for(int i=tkIndex; i<=len; i++){
        if(str[i] == dl || str[i] == '\0'){
            if(i > tkIndex)
                buff=subString(str,tkIndex,i);
            else
                buff="nan";
            tkIndex = i+1;
            break;
        }
        else if(str[i] == qt){
            for(int q=i+1; q<len; q++){
                if(str[q] == qt){
                    buff = subString(str, i+1,q);
                    tkIndex = q+2;
                    break;
                }
            }
            break;
        }
    }
    return buff;
}
/*
    The equivalent float number is returned if the given string is representing digits or started with digits.
    If not, 0.0 is returned.
*/
float toFloat(char *str){
    int i = 0; float value = 0.0; int c = 0;
    int len = stringLen(str);
    float digit[len];
    int sign = (str[0]=='-')? 1:0;
    if(sign){
        i = 1;
        c = 1;
    }
    while(str[i] >= '0' && str[i] <= '9'){
        digit[i-c] = str[i] - '0';
        for(int j=0; j<len; j++){
            int times = 1;
            if((i-c-j)>0) times = 10;
            if((i-c-j)>=0) 
                digit[j] = digit[j] * times;
            else    
                break;
        }
        i++;
    }
    if(str[i] == '.'){
        int times = 10;
        while(str[i+1] >= '0' && str[i+1] <= '9'){
            digit[i-c] = str[i+1] - '0';
            digit[i-c] /= times;
            times *= 10;
            i++;
        }
    }
    for(int j=0; j<(i-c); j++){
        value += digit[j];
    }
    value = (sign)? -1*value : value;
    return value;
}
/*
    The equivalent int number is returned if the given string is representing digits or started with digits.
    If not , 0 is returned
*/  
unsigned int toInt(char *str){
    int i = 0; int value = 0; int c = 0;
    int len = stringLen(str);
    int digit[len];
    int sign = (str[0]=='-')? 1:0;
    if(sign){
        i = 1;
        c = 1;
    }
    while(str[i] >= '0' && str[i] <= '9'){ 
        digit[i-c] = str[i] - '0';
        for(int j=0; j<len; j++){
            int times = 1;
            if(((i-c)-j)>0) times = 10;
            if(((i-c)-j) >= 0)
                digit[j] = digit[j] * times;
            else
                break;
        }
        i++;
    }
    for(int k=0; k<(i-c); k++){
        value += digit[k];
    }
    value = (sign)? (~value)+1 : value;
    return value;
}
/*
    The equivalent char array is returned for given integer number.
*/
char *toChar(int a){
    int i = 10;
    int count = 1;
    int j=0;
    char *digit = (char*)malloc(11*sizeof(char));
    int sign = (a&0x8000)? 1 : 0;
    if(sign){
        a = (~a)+1;
        digit[0] = '-';
        count = 2;
        j = 1;
    }
    while(1){
        if(i > a){
            i /= 10;
            break;
        }
        else{
            i *= 10;
            count++;
        }
    }
    for(; j<count; j++){
        digit[j] = '0' + a/i;
        a %= i;
        i /= 10;
    }
    digit[count] = '\0';
    return digit;
}
/*
    Return 1 if given string have numerical form, else 0
*/
int isNum(char *str){
    while(*str != '\0'){
        if(*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}
/* "word Checker whether each character in the string existed in the patterns or not"
 * patterns => [start char] - [end char]
 * example  => a-z a,b,c,....,x,y,z
 * example  => 0-9 0,1,2,....,8,9
 * you can put any sequences from ascii table
 * RETURN: 1 if each character existed in patterns else 0
 * */
int regExpress(char *str, char *pattern){
    char pat_char[100] = {'\0'}; int pat_idx = 0;
    int pat_length = stringLen(pattern);
    int str_length = stringLen(str);
    for(int index=1; index < pat_length;){
        if(pattern[index] == '-'){
            if(pattern[index-1] <= pattern[index+1]){
                for(char ch=pattern[index-1]; ch<=pattern[index+1]; ch++){
                    pat_char[pat_idx++] = ch;
                }
            }
            else
                printf("[-] Found pattern index error!!![pattern excluded]\n");
        }
        else{
            printf("[-] Found strange patten!!![pattern excluded]\n");
        }
        index += 3;
    }
    int patChar_length = stringLen(pat_char);
    for(int i=0; i<str_length; i++){
        int found = false;
        for(int j=0; j<patChar_length; j++){
            if(str[i] == pat_char[j]){
                found = true;
                break;
            }
        }
        if(!found)
            return 0;
    }
    return 1;
}
//////////////////////////////////////////////////////////////////////////
/* ***********************Time Related Functions*********************** */
//////////////////////////////////////////////////////////////////////////
/*
    Call local time in customized format
    buff: pointer to character capable of holding formatted characters
    RETURN: char pointer
*/
void current_time(char *buff, size_t b_size){
    time_t timer;
    time(&timer);
    struct tm *timeInfo = localtime(&timer);
    strftime(buff, b_size, "[%b-%d-%Y %I:%M%p]", timeInfo); //[MM-DD-YY H:M]
}