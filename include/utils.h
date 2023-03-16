//
// Created by Peter Oo on 03/11/2023
//

#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

char *wordLower(char *word);
int stringLen(const char *str);
int stringCmp(char *str1, char *str2);
void stringCopy(char *src, char *dst);
int indexOf(char *str, char *idxStr);
char *subString(char *str, int idxA, int idxB);
int stringCount(char *str, char *idxStr);
void stringConcat(char **main, char *sub);
char *readLine_csv(char *str, char dl);
float toFloat(char *str);
unsigned int toInt(char *str);
char *toChar(int a);
int isNum(char *str);
int regExpress(char *str, char *pattern);
void current_time(char *buff, size_t b_size);
int month_number(char *abbreviated_month);

#endif // INCLUDE_UTILS_H