//
// Created by Peter Oo on 03/25/2023
//
#include <stdio.h>
#include <stdlib.h>
#include "encrypt_decrypt.h"
#include "utils.h"

void encrypt_file(char *file){
    FILE *fp = fopen(file, "r");
    FILE *fw = fopen("crypt.txt", "w");
    int ch = 0;
    if(fp != NULL && fw != NULL){
        while((ch=fgetc(fp)) != EOF){
            ch += 100;
            fputc(ch, fw);
        }
        fclose(fp);
        fclose(fw);
    }
    else printf(RED"FILE operation failed\n"RESET);
}

void decrypt_file(char *file){
    FILE *fp = fopen("crypt.txt", "r");
    FILE *fw = fopen(file, "w");
    int ch = 0;
    if(fp != NULL && fw != NULL){
        while((ch=fgetc(fp)) != EOF){
            ch -= 100;
            fputc(ch, fw);
        }
        fclose(fp);
        fclose(fw);
    }
    else printf(RED"FILE operation failed\n"RESET);
}

void delete_file_data(char *file){
    FILE *fp = fopen(file, "w");
    if(fp != NULL) fclose(fp);
    else printf(RED"FILE operation failed\n"RESET);
}