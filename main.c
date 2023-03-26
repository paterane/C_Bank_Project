//
// Created by Peter Oo on 02/28/2023
//
#include <stdio.h>
#include <stdlib.h>
#include <bankHandle.h>
#include <encrypt_decrypt.h>

int main(){
    // encrypt_file("data.csv");
    // delete_file_data("data.csv");
    decrypt_file("data.csv");
    loadingDataFromFile();
    delete_file_data("data.csv");
    welcome();

    return 0;
}