//
// Created by Peter Oo on 02/28/2023
//
#include <stdio.h>
#include <stdlib.h>
#include "bankHandle.h"
#include "utils.h"
#include "encrypt_decrypt.h"

#define size 1000 // maximum number of users allowed in database
#define TRANSFER_MONEY 0
#define CASH_IN        1
#define WITHDRAW       2
#define BURROW         3
#define REPAY          4
#define RECEIVE_MONEY  5
/* User transaction */
struct Transaction{
    char note[100];
};
/* User DataBase */
typedef struct{
    unsigned int id; // Serial number of user
    char name[30]; // user name
    char nrc[50]; // user national registration card
    char email[50]; // user email
    char pass[20]; // user password
    char phone[20]; // user phone number
    char address[50]; // user address
    unsigned int curr_amt; // current amount in the account
    unsigned int income;   // User's monthly income
    unsigned int loan_amt; // amount burrowed from bank
    unsigned int loan_rate; // interest
    unsigned int isAdmin: 1; // 1 for admin 0 for user only one admin occur at a time
    unsigned int isPer: 1; // Personal or business account
    unsigned int acc_status: 1; // 1 for active, 0 for suspended
    unsigned int loan_status: 1; // 1 for burrowed, 0 for loan clear
    unsigned int p_count: 2; // counter for wrong password or email per user
    long lock_time; // time of account lock
    struct Date start; // Date of account creating
    long active; // time of account login
    unsigned int transLimit; //transaction amount limit per day
    struct Transaction transfer[100];
    unsigned int tIndex:8; //transfer transaction index
    struct Transaction receive[100]; 
    unsigned int rIndex:8; //receive transaction index
    struct Transaction cashIn[100];
    unsigned int cIndex:8; //cash_in transaction index
    struct Transaction withdraw[100];
    unsigned int wIndex:8; //withdraw transaction index
    struct Transaction burrow[100];
    unsigned int bIndex:8; //burrow transaction index
    struct Transaction repay[100];
    unsigned int pIndex:8; //repay transaction index
}db;
// id, name, nrc, email, pass, phone, address, curr_amt, income, loan_amt, loan_rate, isPer, acc_lvl, acc_status, loan_status, transLimit, records
db userdb[size];
/* Global variables*/
int dbIndex = 0; //database index
int uIndex = -1; //Special index to be used in searching user index
int userFound = -1; //General userIndex that can be used if the index is to be returned
int phFound = -1; //General userIndex that can be used if the index is to be returned
int nrcFound = -1; //General userIndex that can be used if the index is to be returned
unsigned int choice = 0;  //General user selection
char trans_indexes[100] = {-1}; // Special array for storing transaction indexes of paticular keyword found trans_notes

/***************************************************************************************/
//////////////////////////////////Bank Functions/////////////////////////////////////////
/***************************************************************************************/
/*
    if not Existed, global var 'uIndex' = -1, or if existed, uIndex holds index of that existing one.
*/
void isEmailExisted(char *uEmail){
    uIndex = -1;
    for(int i=0; i<dbIndex; i++){
        if(stringCmp(uEmail, userdb[i].email)){
            uIndex = i;
            break;
        }
    }
}
/*
    if not Existed, global var 'uIndex' = -1, or if existed, uIndex holds index of that existing one.
*/
void isNRCExisted(char *reNRC){
    uIndex = -1;
    for(int i=0; i<dbIndex; i++){
        if(stringCmp(reNRC, userdb[i].nrc)){
            uIndex = i;
            break;
        }
    }
}
/*
    if not Existed, global var 'uIndex' = -1, or if exited, uIndex holds index of that existing one.
*/
void isPhoneExisted(char *ph){
    uIndex = -1;
    for(int i=0; i<dbIndex; i++){
        if(stringCmp(ph, userdb[i].phone)){
            uIndex = i;
            break;
        }
    }
}
/*
    function check whether there is an admin or not if exited, return 1 else 0
*/
int isAdminExisted(){
    if(dbIndex >= 1) return 1;
    else return 0;
}
/*
    xxx USD your process string[MM-DD-YY H:M]
*/
void transaction_record(unsigned int amount, int process){
    char time_buff[30] = {'\0'};
    current_time(time_buff, 30);
    char *newTrans = itoChar(amount);
    if(process == TRANSFER_MONEY){
        char *sender = newTrans;
        char *recipient = newTrans;
        stringConcat(&sender, "MMK transferred to ");
        stringConcat(&sender, userdb[phFound].name);
        stringConcat(&sender, time_buff);
        stringCopy(sender, userdb[userFound].transfer[userdb[userFound].tIndex].note);
        userdb[userFound].tIndex++;
        stringConcat(&recipient, "MMK recieved from ");
        stringConcat(&recipient, userdb[userFound].name);
        stringConcat(&recipient, time_buff);
        stringCopy(recipient, userdb[phFound].receive[userdb[phFound].rIndex].note);
        userdb[phFound].rIndex++;
    }
    else if(process == CASH_IN){
        stringConcat(&newTrans, "MMK saved in the account");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].cashIn[userdb[userFound].cIndex].note);
        userdb[userFound].cIndex++;
    }
    else if(process == WITHDRAW){
        stringConcat(&newTrans, "MMK withdrawn from the account");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].withdraw[userdb[userFound].wIndex].note);
        userdb[userFound].wIndex++;
    }
    else if(process == BURROW){
        stringConcat(&newTrans, "MMK burrowed from the bank");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].burrow[userdb[userFound].bIndex].note);
        userdb[userFound].bIndex++;
    }
    else if(process == REPAY){
        stringConcat(&newTrans, "MMK repaid to the bank");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].repay[userdb[userFound].pIndex].note);
        userdb[userFound].pIndex++;
    }
}
/* Take amount from transaction record and RETRUN as INT */
int get_amount_from_trans(char *transact){
    char *temp = subString(transact, 0, indexOf(transact, "MMK"));
    int amount = toInt(temp);
    return amount;
}
/*
    Check total transaction amount per day,and RETURN: 1 if it is less than or equal to max transaction per day, else 0
*/
int isTrans_amt_limit_OK(int idx, unsigned int amount, int process){
    char time_buff[30] = {0};
    current_time(time_buff, 30); // format [Mar-15-2023-06:52PM]
    struct Date *curr_date = str_To_StructDate(time_buff);
    struct Date *hist_date = NULL;
    if(process == TRANSFER_MONEY){
        for(int i=userdb[idx].tIndex-1; i>=0; i--){
            hist_date = str_To_StructDate(userdb[idx].transfer[i].note);
            if(curr_date->dd == hist_date->dd && curr_date->mm == hist_date->mm && curr_date->yy == hist_date->yy){
                amount += get_amount_from_trans(userdb[idx].transfer[i].note);
                free(hist_date);
            }
            else{
                free(hist_date);
                break;
            }
        }
    }
    else if(process == RECEIVE_MONEY){
        for(int i=userdb[idx].rIndex-1; i>=0; i--){
            hist_date = str_To_StructDate(userdb[idx].receive[i].note);
            if(curr_date->dd == hist_date->dd && curr_date->mm == hist_date->mm && curr_date->yy == hist_date->yy){  
                amount += get_amount_from_trans(userdb[idx].receive[i].note);
                free(hist_date);
            }
            else{
                free(hist_date);
                break;
            }
        }
    }
    else if(process == WITHDRAW){
        for(int i=userdb[idx].wIndex-1; i>=0; i--){
            hist_date = str_To_StructDate(userdb[idx].withdraw[i].note);
            if(curr_date->dd == hist_date->dd && curr_date->mm == hist_date->mm && curr_date->yy == hist_date->yy){  
                amount += get_amount_from_trans(userdb[idx].withdraw[i].note);
                free(hist_date);
            }
            else{
                free(hist_date);
                break;
            }
        }
    }
    free(curr_date);
    if(amount <= userdb[idx].transLimit) return 1;
    else return 0;
}

void service_fee(int uIdx, unsigned int amount){
    userdb[uIdx].curr_amt -= amount*0.01;
}

void transfer_money(int idxS, int idxR, unsigned int amount){
    if(isTrans_amt_limit_OK(idxS, amount, TRANSFER_MONEY)){
        if(isTrans_amt_limit_OK(idxR, amount, RECEIVE_MONEY)){
            userdb[idxS].curr_amt -= amount;
            userdb[idxR].curr_amt += amount;
            service_fee(idxS, amount);
            transaction_record(amount, TRANSFER_MONEY);
            printf("%-20s:"BLUE" %s\n"RESET, "Transaction", userdb[idxS].transfer[userdb[idxS].tIndex-1].note);
            printf("%-20s:"BLUE" %u mmk\n"RESET, "Balance", userdb[idxS].curr_amt);
        }
        else{
            printf(RED"Sorry, Recipient's account have reached max transaction amount limit.\nTry it, tomorrow.\n"RESET);
        }
    }
    else{
        printf(RED"Sorry, You have reached your max transaction amount limit.\nTry it, tomorrow.\n"RESET);
    }
}
void cashIn_withdraw(int uIdx, unsigned int amount, int process){
    if(process==CASH_IN){
        userdb[uIdx].curr_amt += amount;
        transaction_record(amount, CASH_IN);
        printf("%-20s:"BLUE" %s\n"RESET, "Transaction", userdb[uIdx].cashIn[userdb[uIdx].cIndex-1].note);
        printf("%-20s:"BLUE" %u mmk\n"RESET, "Balance", userdb[uIdx].curr_amt);
    }
    else if(process==WITHDRAW){
        if(isTrans_amt_limit_OK(uIdx, amount, WITHDRAW)){
            userdb[uIdx].curr_amt -= amount;
            service_fee(uIdx, amount);
            transaction_record(amount, WITHDRAW);
            printf("%-20s:"BLUE" %s\n"RESET, "Transaction", userdb[uIdx].withdraw[userdb[uIdx].wIndex-1].note);
            printf("%-20s:"BLUE" %u mmk\n"RESET, "Balance", userdb[uIdx].curr_amt);
        }
        else printf(RED"Sorry, You have reached your max transaction amount limit.\nTry it, tomorrow.\n"RESET);
    }
}
/*
    RETURN: days left for loan repayment, if exceed, negative numbers are returned.
*/
int days_left(int uIdx){
    long curr_time = current_time_L();
    struct Date *time_info = str_To_StructDate(userdb[uIdx].burrow[userdb[uIdx].bIndex-1].note);
    long due_time = timeStruct_to_L(time_info) + 2592000; // 30 days (30*24*60*60)
    double time_left = diff_time_L(due_time, curr_time);
    return time_left / 86400.0;
}
void burrow_repay(int uIdx, unsigned int amount, int process){
    if(process == BURROW){
        int process_fee = 5000;
        userdb[uIdx].loan_amt = amount;
        userdb[uIdx].loan_rate = amount * 0.3;
        userdb[uIdx].curr_amt += userdb[uIdx].loan_amt - process_fee;
        userdb[uIdx].loan_amt += userdb[uIdx].loan_rate;
        userdb[uIdx].loan_status = 1;            // mark loan status as active
        transaction_record(amount, BURROW);
        printf("%-20s:"BLUE" %s\n"RESET, "Transaction", userdb[uIdx].burrow[userdb[uIdx].bIndex-1].note);
        printf("%-20s:"BLUE" %u mmk"RESET"\n%-20s: "L_BLUE"%u mmk"RESET \
                "\n%-20s: "BLUE"%u mmk"RESET"\n", "Balance", userdb[uIdx].curr_amt, 
                                                        "Loan amount", userdb[uIdx].loan_amt - userdb[uIdx].loan_rate, 
                                                        "Loan interest", userdb[uIdx].loan_rate);
    }
    else if(process == REPAY){
        if(amount < userdb[uIdx].loan_amt){
            userdb[uIdx].loan_amt -= amount;
            userdb[uIdx].curr_amt -= amount;
            transaction_record(amount, REPAY);
            printf("%-20s: "BLUE"%s"RESET"\n", "transaction", userdb[uIdx].repay[userdb[uIdx].pIndex-1].note);
        }
        else{
            userdb[uIdx].curr_amt -= amount;
            userdb[uIdx].curr_amt +=  amount - userdb[uIdx].loan_amt;
            transaction_record(userdb[uIdx].loan_amt, REPAY);
            printf("%-20s: "BLUE"%s"RESET"\n", "transaction", userdb[uIdx].repay[userdb[uIdx].pIndex-1].note);
            userdb[uIdx].loan_status = 0;
            userdb[uIdx].loan_amt = 0;
            userdb[uIdx].loan_rate = 0;
        }
    }
}
/*
    copying user's data to current idx from next idx
*/
void shift_user_data(int curIdx, int nextIdx){
    stringCopy(userdb[nextIdx].name, userdb[curIdx].name); 
    stringCopy(userdb[nextIdx].nrc, userdb[curIdx].nrc);
    stringCopy(userdb[nextIdx].email, userdb[curIdx].email);
    stringCopy(userdb[nextIdx].pass, userdb[curIdx].pass);
    stringCopy(userdb[nextIdx].phone, userdb[curIdx].phone);
    stringCopy(userdb[nextIdx].address, userdb[curIdx].address);
    userdb[curIdx].curr_amt = userdb[nextIdx].curr_amt;
    userdb[curIdx].income = userdb[nextIdx].income;
    userdb[curIdx].loan_amt = userdb[nextIdx].loan_amt;
    userdb[curIdx].loan_rate = userdb[nextIdx].loan_rate;
    userdb[curIdx].isAdmin = userdb[nextIdx].isAdmin;
    userdb[curIdx].isPer = userdb[nextIdx].isPer;
    userdb[curIdx].acc_status = userdb[nextIdx].acc_status;
    userdb[curIdx].loan_status = userdb[nextIdx].loan_status;
    userdb[curIdx].p_count = userdb[nextIdx].p_count;
    userdb[curIdx].start.yy = userdb[nextIdx].start.yy;
    userdb[curIdx].start.mm = userdb[nextIdx].start.mm;
    userdb[curIdx].start.dd = userdb[nextIdx].start.dd;
    userdb[curIdx].active = userdb[nextIdx].active;
    userdb[curIdx].lock_time = userdb[nextIdx].lock_time;
    userdb[curIdx].transLimit = userdb[nextIdx].transLimit;
    userdb[curIdx].tIndex = userdb[nextIdx].tIndex;
    for(int i=0; i<userdb[curIdx].tIndex;i++){
        stringCopy(userdb[nextIdx].transfer[i].note, userdb[curIdx].transfer[i].note);
    }
    userdb[curIdx].rIndex = userdb[nextIdx].rIndex;
    for(int i=0; i<userdb[curIdx].rIndex;i++){
        stringCopy(userdb[nextIdx].receive[i].note, userdb[curIdx].receive[i].note);
    }
    userdb[curIdx].cIndex = userdb[nextIdx].cIndex;
    for(int i=0; i<userdb[curIdx].cIndex;i++){
        stringCopy(userdb[nextIdx].cashIn[i].note, userdb[curIdx].cashIn[i].note);
    }
    userdb[curIdx].wIndex = userdb[nextIdx].wIndex;
    for(int i=0; i<userdb[curIdx].wIndex;i++){
        stringCopy(userdb[nextIdx].withdraw[i].note, userdb[curIdx].withdraw[i].note);
    }
    userdb[curIdx].bIndex = userdb[nextIdx].bIndex;
    for(int i=0; i<userdb[curIdx].bIndex;i++){
        stringCopy(userdb[nextIdx].burrow[i].note, userdb[curIdx].burrow[i].note);
    }
    userdb[curIdx].pIndex = userdb[nextIdx].pIndex;
    for(int i=0; i<userdb[curIdx].pIndex;i++){
        stringCopy(userdb[nextIdx].repay[i].note, userdb[curIdx].repay[i].note);
    }
}
/*
    pop a user and re-arrange the db's content in order
*/
void pop_out_user(int idx){
    dbIndex--;
    for(int i=idx; i<dbIndex; i++){
        shift_user_data(i, i+1);
    }
}
user_sector(){
    char userIn[30];
    printf("User name >>"BLUE" %s "RESET"<<\n", userdb[userFound].name);
    while(1){
        printf(GREEN"Transfer"RESET","GREEN" Withdraw"RESET","GREEN" Info" \
                RESET","GREEN" Cash In"RESET","GREEN" Loan"RESET","GREEN" Log" \
                RESET","YELLOW" %s"RESET"%s"GREEN"Exit"RESET" or"GREEN" 1"RESET" to main page: ",
                    (userdb[userFound].isAdmin)? "admin" : "", (userdb[userFound].isAdmin)? ", " : "");
        scanf(" %[^\n]%*c", userIn);
        if(stringCmp(wordLower(userIn), "transfer")){
            while(1){
                printf("Enter a phone number to receive: ");
                scanf(" %[^\n]%*c", userIn);
                isPhoneExisted(userIn);
                if(uIndex != userFound){
                    if(uIndex == -1){
                        printf(RED"Received user's phone number not existed\n"RESET);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }else break;                    
                }
                else{
                    printf(RED"Sorry you can't transfer to your same account\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            phFound = uIndex;
            printf("Recipient found:"BLUE" %s\n"RESET"Email:"BLUE" %s\n"RESET, userdb[phFound].name, userdb[phFound].email);
            while(1){
                printf(GREEN"Minimum amount allowable to be transferred >> 1000MMK\n"RESET);
                printf("Enter an amount to be transferred: ");
                digit_input(&choice);
                if(choice >= 1000){
                    if(userdb[userFound].curr_amt-1000>=choice) break;
                    else{
                        printf(RED"Insufficient balance\n"RESET);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }
                }
                else{
                    printf(RED"Transferred amount shouldn't be less than 1000 mmk\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            int amount_t = choice;
            while(1){
                printf("Enter password to send: ");
                password_input(userIn);
                if(stringCmp(userIn, userdb[userFound].pass)) break;
                else{
                    printf(RED"Wrong credential\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            transfer_money(userFound, phFound, amount_t);
        }
        else if(stringCmp(wordLower(userIn), "withdraw")){
            while(1){
                printf(GREEN"Minimum amount allowable to be withdrawn >> 5000MMK\n"RESET);
                printf("Submit amount to be withdrawn: ");
                digit_input(&choice);
                if(choice >= 5000){
                    if(userdb[userFound].curr_amt-1000 >= choice) break;
                    else{
                        printf(RED"Insufficient amount in the account\n"RESET);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }
                }
                else{   
                    printf(RED"Amount not in the allowable limits\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            cashIn_withdraw(userFound, choice, WITHDRAW);
        }
        else if(stringCmp(wordLower(userIn), "info")){
            printf("%-20s: "BLUE"%s\n"RESET"%-20s: "L_BLUE"%s\n"RESET"%-20s: "BLUE \ 
                    "%s\n"RESET"%-20s: "L_BLUE"%s\n"RESET"%-20s: "BLUE"%s\n"RESET \
                    "%-20s: "L_BLUE"%uMMK\n"RESET"%-20s:"BLUE" %d/%d/%d"RESET"\n", "Name", userdb[userFound].name, "NRC", 
                                                    userdb[userFound].nrc, "Email", userdb[userFound].email, 
                                                    "Phone", userdb[userFound].phone, "Address", userdb[userFound].address, 
                                                    "Balance", userdb[userFound].curr_amt, "Created on", userdb[userFound].start.dd,
                                                    userdb[userFound].start.mm+1, userdb[userFound].start.yy);
            unsigned int loan_amt = 0;
            if(userdb[userFound].loan_status == 1){
                loan_amt = get_amount_from_trans(userdb[userFound].burrow[userdb[userFound].bIndex-1].note);
            }
            printf("%-20s: "BLUE"%uMMK\n"RESET"%-20s: "L_BLUE"%uMMK\n"RESET"%-20s: "BLUE"%uMMK\n"RESET \
                    "[Acc_Type: "BLUE"%s"RESET"]\t[Loan_Status: "BLUE"%s"RESET"]\n", "Income", userdb[userFound].income,
                                                    "Burrowed amount", loan_amt, "loan interest",
                                                    userdb[userFound].loan_rate,(userdb[userFound].isPer)? "Personal":"Business",
                                                    (userdb[userFound].loan_status)? "Burrowed":"Clear");
        }
        else if(stringCmp(wordLower(userIn), "cash in")){
            while(1){
                printf(GREEN"Minimum amount allowable to be submitted >> 5000MMK\n"RESET);
                printf("Please insert CASH in the slot: ");
                digit_input(&choice);
                if(choice >= 5000) break;
                else{
                    printf(RED"Amount not in the allowable limits\n"RESET);
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
            }
            cashIn_withdraw(userFound, choice, CASH_IN);
        }
        else if(stringCmp(wordLower(userIn), "log")){ 
            int num = 1;              
            for(int i=0; i<userdb[userFound].tIndex; i++) 
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, userdb[userFound].transfer[i].note);
            if(userdb[userFound].tIndex > 0) printf("\n");
            for(int i=0; i<userdb[userFound].rIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, userdb[userFound].receive[i].note);
            if(userdb[userFound].rIndex > 0) printf("\n");
            for(int i=0; i<userdb[userFound].cIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, userdb[userFound].cashIn[i].note);
            if(userdb[userFound].cIndex > 0) printf("\n");
            for(int i=0; i<userdb[userFound].wIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, userdb[userFound].withdraw[i].note);
            if(userdb[userFound].wIndex > 0) printf("\n");
            for(int i=0; i<userdb[userFound].bIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, userdb[userFound].burrow[i].note);
            if(userdb[userFound].bIndex > 0) printf("\n");            
            for(int i=0; i<userdb[userFound].pIndex; i++)
                printf(GREEN"%d"RESET") "BLUE"%s\n"RESET, num++, userdb[userFound].repay[i].note);
            if(userdb[userFound].pIndex > 0) printf("\n");
        }
        else if(stringCmp(wordLower(userIn), "loan")){
            if(userdb[userFound].loan_status == 1){
                int time_left = days_left(userFound);
                printf("%-30s: "BLUE"%u mmk"RESET"\n", "Loan amount left to be repaid", userdb[userFound].loan_amt);
                if(time_left < 0) printf("Loan overdue\nContact to HO\n%-30s:"RED" %d day%c"RESET, "time overdue", (-1)*time_left,
                                                                                                ((-1)*time_left > 1)? 's':' ');
                else printf("%-30s:"BLUE" %d day%c"RESET"\n", "Time left to repay", time_left, (time_left > 1)? 's':' ');
            }
            else    printf(GREEN"Loan Status Clear\nWould you like to get some loan...?\n"RESET);
            while(1){
                printf("Press "BLUE"<enter>"RESET" to user_sector or"BLUE" 1"RESET" to continue: ");
                fgets(userIn, 30, stdin);
                if(stringCmp(userIn, "\n")) user_sector();
                else if(stringCmp(userIn, "1\n")) break;
                else printf(RED"Wrong Input\n"RESET);
            }
            long start_time = timeStruct_to_L(&userdb[userFound].start);
            long curr_time = current_time_L();
            double acc_exist_time = diff_time_L(curr_time, start_time);                                        // 3 days
            if(userdb[userFound].loan_status == 0 && userdb[userFound].income >= 400000 && acc_exist_time >= 259200.0){ //In progress
                while(1){
                    printf("Enter an amount to be burrowed: ");
                    digit_input(&choice);
                    if(choice <= 2*userdb[userFound].income && choice >= 400000)
                        break;
                    else{
                        printf(RED"Your max loan amount is limited to double of income, and\nmini loan amount at least 400,000\n"RESET);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }
                }
                burrow_repay(userFound, choice, BURROW);
            }
            else{
                if(userdb[userFound].income < 400000){
                    printf(RED"Sorry, you can't get loan as your salary is lower than 400,000MMK"RESET"\n");
                    user_sector();
                }
                else if(acc_exist_time < 259200.0){
                    printf(RED"Sorry, you can get loan only when your account existance is more than 3 days"RESET"\n");
                    user_sector();
                }
                while(1){
                    printf("Enter an amount to be repaid for loan: ");
                    digit_input(&choice);
                    int a = userdb[userFound].bIndex-1;
                    unsigned int monthly_repay = (get_amount_from_trans(userdb[userFound].burrow[a].note)+userdb[userFound].loan_rate) / 30;
                    if(userdb[uIndex].curr_amt - 1000 >= choice && choice >= monthly_repay){
                        burrow_repay(userFound, choice, REPAY);
                        break;
                    }
                    else{
                        printf(RED"Sorry, insufficient balance or repaid amount should not less than monthly repay amount: %u mmk\n"
                                RESET, monthly_repay);
                        while(1){
                            printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to re-submit: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(userIn, "\n")) user_sector();
                            else if(stringCmp(userIn, "1\n")) break;
                            else printf(RED"Wrong Input\n"RESET);
                        }
                    }
                }
            }
        }
        else if(stringCmp(wordLower(userIn), "admin") && userdb[userFound].isAdmin){
            while(1){
                printf(BLUE"users"RESET" ,"BLUE"change admin"RESET" ,"BLUE"manage"RESET" ,"BLUE"remove"RESET" or " \
                        BLUE"exit"RESET" to user_sector: ");
                scanf(" %[^\n]%*c", userIn);
                if(stringCmp(wordLower(userIn), "users")){
                    for(int i=0; i<dbIndex; i++){
                        printf("%-20s:"BLUE" %s"RESET"\n%-20s:"BLUE" %s"RESET"\n%-20s:"BLUE" %s"RESET \
                                "\n%-20s:"BLUE" %s"RESET"\n\n", "User", userdb[i].name, "Phone", userdb[i].phone,
                                                        "Email", userdb[i].email,
                                                        "Password", userdb[i].pass);
                    }
                    while(1){
                        printf("Press"BLUE" <enter>"RESET" to user_sector or"BLUE" 1"RESET" to admin: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "\n")) user_sector();
                        else if(stringCmp(userIn, "1\n")) break;
                        else printf(RED"Wrong Input\n"RESET);
                    }
                }
                else if(stringCmp(wordLower(userIn), "change admin")){
                    printf(GREEN"Change the Admin account"RESET"\n");
                    while(1){
                        int loopBreak = 0;
                        printf("%-15s: ", "Enter Email");
                        scanf(" %[^\n]%*c", userIn);
                        isEmailExisted(userIn);
                        if(uIndex != -1 && uIndex != userFound){
                            printf("Are you sure to pass to"BLUE" %s"RESET" [Yes|No][No]: ");
                            fgets(userIn, 30, stdin);
                            if(stringCmp(wordLower(userIn), "yes\n")){
                                userdb[userFound].isAdmin = 0;
                                userdb[uIndex].isAdmin = 1;
                                printf(GREEN"Admin position has been successfully passed to %s\n"RESET, userdb[uIndex].name);
                                user_sector();
                            }
                            else printf("You are still"BLUE" Admin"RESET"\n");
                            break;
                        }
                        else{
                            printf(RED"Email not found\n"RESET);
                            while(1){
                                printf("Press"BLUE" <enter>"RESET" to admin or"BLUE" 1"RESET" to re-submit: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(userIn, "\n")){
                                    loopBreak = 1;
                                    break;
                                }
                                else if(stringCmp(userIn, "1\n")) break;
                                else printf(RED"Wrong Input\n"RESET);
                            }
                            if(loopBreak) break;
                        }
                    }
                }
                else if(stringCmp(wordLower(userIn), "manage")){
                    printf(GREEN"Admin can suspend an account or unlock from suspension\n"RESET);
                    while(1){
                        int loopbreak = 0;
                        printf("%-15s: ", "Enter Email");
                        scanf(" %[^\n]%*c", userIn);
                        isEmailExisted(userIn);
                        if(uIndex != -1 && uIndex != userFound){
                            printf("This account, %s, is "BLUE"%s\n"RESET, userdb[uIndex].email, 
                                                                            (userdb[uIndex].acc_status)? "active":"suspended");
                            if(userdb[uIndex].acc_status){
                                printf("Would you like to lock the account?[Yes|No][No]: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(wordLower(userIn), "yes\n")){
                                    userdb[uIndex].acc_status = 0;
                                    printf("The account has been locked successfully\n");
                                }
                            }
                            else{
                                printf("Would you like to unlock the account?[Yes|No][No]: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(wordLower(userIn), "yes\n")){
                                    userdb[uIndex].acc_status = 1;
                                    printf("The account has been unlocked successfully\n");
                                }                         
                            }
                            break;
                        }
                        else{
                            printf(RED"Email not found\n"RESET);
                            while(1){
                                printf("Press"BLUE" <enter>"RESET" to admin or"BLUE" 1"RESET" to re-submit: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(userIn, "\n")){
                                    loopbreak = 1;
                                    break;
                                }
                                else if(stringCmp(userIn, "1\n")) break;
                                else printf(RED"Wrong Input\n"RESET);
                            }
                            if(loopbreak) break;
                        }
                    }
                }
                else if(stringCmp(wordLower(userIn), "remove")){
                    printf(GREEN"Permanently delete an account\n"RESET);
                    while(1){
                        int loopbreak = 0;
                        printf("%-15s: ", "Enter Email");
                        scanf(" %[^\n]%*c", userIn);
                        isEmailExisted(userIn);
                        if(uIndex != -1 && uIndex != userFound){
                            printf("%-15s>>> "BLUE"%s, %s\n"RESET,"User found", userdb[uIndex].name, userdb[uIndex].email);
                            printf(YELLOW"Are you sure to remove %s?[yes/no][no]: "RESET, userdb[uIndex].name);
                            fgets(userIn, 30, stdin);
                            if(stringCmp(wordLower(userIn), "yes\n")){
                                pop_out_user(uIndex);
                                printf("The account has been successfully removed.\n");
                            }
                            break;
                        }
                        else{
                            printf(RED"Email not found\n"RESET);
                            while(1){
                                printf("Press"BLUE" <enter>"RESET" to admin or"BLUE" 1"RESET" to re-submit: ");
                                fgets(userIn, 30, stdin);
                                if(stringCmp(userIn, "\n")){
                                    loopbreak = 1;
                                    break;
                                }
                                else if(stringCmp(userIn, "1\n")) break;
                                else printf(RED"Wrong Input\n"RESET);
                            }
                            if(loopbreak) break;
                        }
                    }
                }
                else if(stringCmp(wordLower(userIn), "exit")) user_sector();
                else printf(RED"Wrong Input\n"RESET);
            }

        }
        else if(stringCmp(wordLower(userIn), "exit")) exitProgram();
        else if(stringCmp(userIn, "1")) welcome();
        else printf(RED"[-]Wrong Input\n"RESET);
    }
}
void login_section(){
    // int locked = 0;
    char uEmail[50];
    char uPass[50];
    long curr_time = current_time_L();
    printf(YELLOW"\t\tLOGIN SECTOR\n"RESET);
    printf("%-10s: ", "Email");
    scanf(" %[^\n]%*c", uEmail);
    isEmailExisted(uEmail);
    if(uIndex != -1){
        if(diff_time_L(curr_time, userdb[uIndex].active) >= 7776000.0) userdb[uIndex].acc_status = 0;
        if(userdb[uIndex].p_count >= 3){
            userdb[uIndex].p_count = 0;
            userdb[uIndex].lock_time = curr_time;
        }
        if(diff_time_L(curr_time, userdb[uIndex].lock_time) <= 300.0 || userdb[uIndex].acc_status == 0){
            if(userdb[uIndex].acc_status == 0) printf(RED"Account suspended due to inactive status or admin locked, contact to HO"RESET"\n");
            else printf(RED"Account locked temporarily\nTry another"RESET"\n");
            funcCall(login_section, "login_section");
        }
        printf("%-10s: ", "Password");
        password_input(uPass);
        if(!stringCmp(uPass, userdb[uIndex].pass)){
            userdb[uIndex].p_count++;
            printf(RED"Password incorrect!\n"RESET);
            funcCall(login_section, "login_section");
        }
    }
    else{
        printf(RED"Email doesn't exist!\n"RESET);
        funcCall(login_section, "login_section");
    }
    userFound = uIndex;
    userdb[userFound].p_count = 0;
    userdb[userFound].active = current_time_L();
    user_sector();
}
/*
 * RETURN: 0 if not email format, 1 if it is an email.
 * */
int isMailValid(char *email, char *r_pattern){ // Format: xxxx@[A-Za-z0-9].com
    int indexA = indexOf(email, "@");
    int indexB = indexOf(email, ".com");
    if(indexA == -1 || indexB == -1)
        return 0;
    char *emailName = subString(email, 0, indexA);
    if(emailName == NULL)
        return 0;
    if(!regExpress(emailName, "a-z0-9.-.")){
        printf(RED"An email shouldn't contain any special or upper case characters.\n"RESET);
        return 0;
    }
    if(stringCount(emailName, ".") >= 2){
        printf(RED"An email shouldn't contain \".\" more than once in user name!!\n"RESET);
        return 0;
    }
    if(email[indexB+4] != '\0')
        return 0;
    char *domain = subString(email, indexA+1,indexB);
    if(domain == NULL)
        return 0;
    if(regExpress(domain, r_pattern))
        return 1;
    else
        return 0;
}
/*
    Return 1 if NRC format is valid, otherwise 0.
*/
int isNRCValid(char *nrc){
    int indexA = indexOf(nrc, "/");
    int indexB = indexOf(wordLower(nrc), "<n>");
    int indexC = indexOf(nrc, "\0");
    char *code = subString(nrc, 0, indexA);
    char *region = subString(wordLower(nrc), indexA+1, indexB);
    char *digit = subString(nrc, indexB+3, indexC);
    if(indexA == -1 || indexB == -1)    return 0;
    if(!isNum(code))    return 0;
    if(!regExpress(region, "a-zA-Z"))   return 0;
    if(isNum(digit))    return 1;
    else    return 0;
}
/*
    Return 1 if password format is correct and strong, else 0
*/
int isPassValid(char *pass){
    int len = stringLen(pass);
    int count = 0;
    for(int i=0; i<len; i++){
        if(pass[i] == ' ')  return 0;
    }
    if(len < 12) return 0;
    if(!regExpress(pass, "!-!#-~")) return 0;
    for(char c='A'; c<='Z'; c++){
        char temp[2] = {c,'\0'};
        count += stringCount(pass, temp);
        if(count > 0) break;
    }
    if(count < 1) return 0;
    count = 0;
    for(char c='0'; c<='9'; c++){
        char temp[2] = {c,'\0'};
        count += stringCount(pass, temp);
        if(count > 0) break;
    }
    if(count < 1) return 0;
    count = 0;
    char spChar[] = "!#$%&'()*+,-./:;<=>?@[]\\^_`{}|~";
    for(int i=0; i<stringLen(spChar); i++){
        char temp[2] = {spChar[i], '\0'};
        count += stringCount(pass, temp);
        if(count > 0) break;
    }
    if (count < 1) return 0;
    else return 1;
}
void mail_form(){
    printf("%-10s: ", "An Email");
    scanf(" %[^\n]%*c", userdb[dbIndex].email);
    if(isMailValid(userdb[dbIndex].email, "a-z0-9")){
        isEmailExisted(userdb[dbIndex].email);
        if(uIndex == -1){
            printf("%-10s: ", "Name");
            scanf(" %[^\n]%*c", userdb[dbIndex].name);
        }
        else{
            printf(RED"Email already existed\n"RESET);
            funcCall(mail_form, "mail_form");
        }
    }
    else{
        printf(RED"Email format not valid\n"RESET);
        funcCall(mail_form, "mail_form");
    }
}
void nrc_form(){
    printf("%-10s: ", "NRC number");
    scanf(" %[^\n]%*c", userdb[dbIndex].nrc);
    if(isNRCValid(userdb[dbIndex].nrc)){
        isNRCExisted(userdb[dbIndex].nrc);
        if(uIndex != -1){
            printf(RED"NRC already existed\n"RESET);
            funcCall(nrc_form, "nrc_form");
        }
    }
    else{
        printf(RED"NRC format not valid\n"RESET);
        funcCall(nrc_form, "nrc_form");
    }
}
void pass_form(){
    char confirmPass[20];
    printf("[Not less than 12 chars including at least one Capital and number and special]\n%-20s: ", "Password");
    password_input(userdb[dbIndex].pass);
    if(isPassValid(userdb[dbIndex].pass)){
        printf("%-20s: ", "Re-type password");
        password_input(confirmPass);
        if(!stringCmp(userdb[dbIndex].pass, confirmPass)){
            printf(RED"Password not matched\n"RESET);
            funcCall(pass_form, "pass_form");
        }
    }
    else{
        printf(RED"Password format invalid or weak password\n"RESET);
        funcCall(pass_form, "pass_form");
    }
}
/*
    RETURN: 1 if phone format valid, else 0
    //09xxxxxxxxx [9digits next to 09]
*/
int isPhoneValid(char *ph){
    int indexA = indexOf(ph, "09");
    int indexB = indexOf(ph, "\0");
    if(indexA == -1)    return 0;
    char *digits = subString(ph, indexA+2, indexB);
    if(isNum(digits) && (stringLen(digits)==9))   return 1;
    else    return 0;
}
void phone_form(){
    printf("%-25s: ", "Enter a phone number");
    scanf(" %[^\n]%*c", userdb[dbIndex].phone);
    if(isPhoneValid(userdb[dbIndex].phone)){
        isPhoneExisted(userdb[dbIndex].phone);
        if(uIndex != -1){
            printf(RED"Phone number already existed\n"RESET);
            funcCall(phone_form, "phone_form");
        }
    }
    else {
        printf(RED"Phone number not valid\n"RESET);
        funcCall(phone_form, "phone_form");
    }
}

void registration(){
    char time_buff[30];
    current_time(time_buff, 30);
    struct Date *current_time = str_To_StructDate(time_buff);
    printf(YELLOW"\t\tUSER REGISTRATION\n"RESET);
    mail_form();
    nrc_form();
    pass_form();
    phone_form();
    printf("%-15s: ", "Address");
    scanf(" %[^\n]%*c", userdb[dbIndex].address);
    userdb[dbIndex].curr_amt = 1000;            // default money amount in account for initial state
    printf("%-15s: ", "Monthly income");
    digit_input(&userdb[dbIndex].income);
    userdb[dbIndex].loan_amt = 0;               // default loan clear
    userdb[dbIndex].loan_rate = 0;            // default loan rate
    printf("[1 for Personal or 0 for business]\n%-15s: ", "Account-type");
    digit_input(&choice);
    userdb[dbIndex].isPer = choice;
    if(!isAdminExisted()) userdb[dbIndex].isAdmin = 1;                    //make you admin
    else userdb[dbIndex].isAdmin = 0;                                     //make you user. Ask admin if want to be captain
    userdb[dbIndex].acc_status = 1;                                       // default account active
    userdb[dbIndex].loan_status = 0;                                      // default loan status clear
    userdb[dbIndex].p_count = 0;                                          // wrong password count
    userdb[dbIndex].start.yy = current_time->yy;                          // Account created year
    userdb[dbIndex].start.mm = current_time->mm;                          // Account created month
    userdb[dbIndex].start.dd = current_time->dd;                          // Account created day
    userdb[dbIndex].active = current_time_L();                            // current login time                   
    userdb[dbIndex].lock_time = 0;                                        // password lock time
    if(userdb[dbIndex].isPer == 1)  userdb[dbIndex].transLimit = 1000000; // 1,000,000 MMK per day
    else    userdb[dbIndex].transLimit = 10000000;                        // 10,000,000 MMK per day
    userdb[dbIndex].id = dbIndex + 1;                                     // last index number is next id number
    printf("Registeration succeeded.\n");
    dbIndex++;  //Updating user count after successful registeration
    printf(BLUE"EXIT"RESET" or Press "BLUE" <enter>"RESET" to Main Section: ");
    char userIn[30];
    fgets(userIn, 30, stdin);
    if(stringCmp(wordLower(userIn), "exit\n")) exitProgram();
    else          welcome();
}
void welcome(){
    char userIn[30];
    printf(YELLOW"\t\tWelcome to ebanking service\n"RESET);
    while(1){
        printf("["GREEN"login"RESET","GREEN" register"RESET" or"GREEN" exit"RESET"]: ");
        scanf(" %[^\n]%*c", userIn);
        if(stringCmp(wordLower(userIn), "login")) login_section();
        else if(stringCmp(wordLower(userIn), "register")) registration();
        else if(stringCmp(wordLower(userIn), "exit")) exitProgram();
        else printf(RED"[!]Wrong Input\n"RESET);
    }
}
/*
    particular function call to shorten the repeated codes
*/
void funcCall(void(*f)(), char *fname){
    char userIn[30] = {0};
    while(1){
        printf("Press"BLUE" <enter>"RESET" to main page or"BLUE" 1"RESET" to %s: ", fname);
        fgets(userIn, 30, stdin);
        if(stringCmp(userIn, "\n")){
            welcome();
            break;
        }
        else if(stringCmp(userIn, "1\n")){
            f();
            break;
        }
        else printf(RED"Wrong Input\n"RESET);
    }
}
/*
    Save data to file and exit the program
*/
void exitProgram(){
    saveAllData();
    // encrypt_file("data.csv");
    // delete_file_data("data.csv");
    exit(0);
}
// id,name,nrc,email,pass,phone,address,curr_amt,income,loan_amt,loan_rate,isPer,acc_status,loan_status,translimit,records,transactions
void loadingDataFromFile(){
    printf(B_GROUND PURPLE"\t\tLoading data from file...\n"RESET);
    FILE *fp = NULL;
    char buffer[60000]; // note 100 * record 600
    fp = fopen("data.csv", "r");
    if(fp != NULL){
        while(fscanf(fp, " %[^\n]%*c", buffer) != EOF){
            char *temp = NULL;
            char tBuffer[10000];
            char rBuffer[10000];
            char cBuffer[10000];
            char wBuffer[10000];
            char bBuffer[10000];
            char pBuffer[10000];
            int col = 0;
            while((temp=readLine_csv(buffer, ','))){
                switch(col){
                    case 0: userdb[dbIndex].id = toInt(temp); break;
                    case 1: stringCopy(temp, userdb[dbIndex].name); break;
                    case 2: stringCopy(temp, userdb[dbIndex].nrc); break;
                    case 3: stringCopy(temp, userdb[dbIndex].email); break;
                    case 4: stringCopy(temp, userdb[dbIndex].pass); break;
                    case 5: stringCopy(temp, userdb[dbIndex].phone); break;
                    case 6: stringCopy(temp, userdb[dbIndex].address); break;
                    case 7: userdb[dbIndex].curr_amt = toInt(temp); break;
                    case 8: userdb[dbIndex].income = toInt(temp); break;
                    case 9: userdb[dbIndex].loan_amt = toInt(temp); break;
                    case 10: userdb[dbIndex].loan_rate = toInt(temp); break;
                    case 11: userdb[dbIndex].isAdmin = toInt(temp); break;
                    case 12: userdb[dbIndex].isPer = toInt(temp); break;
                    case 13: userdb[dbIndex].acc_status = toInt(temp); break;
                    case 14: userdb[dbIndex].loan_status = toInt(temp); break;
                    case 15: userdb[dbIndex].p_count = toInt(temp); break;
                    case 16: userdb[dbIndex].start.yy = toInt(temp); break;
                    case 17: userdb[dbIndex].start.mm = toInt(temp); break;
                    case 18: userdb[dbIndex].start.dd = toInt(temp); break;
                    case 19: userdb[dbIndex].active = toInt(temp); break;    
                    case 20: userdb[dbIndex].lock_time = toInt(temp); break;
                    case 21: userdb[dbIndex].transLimit = toInt(temp); break;
                    //transfer , receive, cash_in, withdraw, burrow, repay
                    case 22: stringCopy(temp, tBuffer); break;
                    case 23: stringCopy(temp, rBuffer); break;
                    case 24: stringCopy(temp, cBuffer); break;
                    case 25: stringCopy(temp, wBuffer); break;
                    case 26: stringCopy(temp, bBuffer); break;
                    case 27: stringCopy(temp, pBuffer); break;
                }
                free(temp);
                if(col>=27) break;
                col++;
            }
            if(!stringCmp(tBuffer, "nan")){
                while((temp=readLine_csv(tBuffer, '|'))){
                    stringCopy(temp, userdb[dbIndex].transfer[userdb[dbIndex].tIndex].note);
                    userdb[dbIndex].tIndex++;
                    free(temp);
                }
            }
            if(!stringCmp(rBuffer, "nan")){
                while((temp=readLine_csv(rBuffer, '|'))){
                    stringCopy(temp, userdb[dbIndex].receive[userdb[dbIndex].rIndex].note);
                    userdb[dbIndex].rIndex++;
                    free(temp);
                }
            }
            if(!stringCmp(cBuffer, "nan")){
                while((temp=readLine_csv(cBuffer, '|'))){
                    stringCopy(temp, userdb[dbIndex].cashIn[userdb[dbIndex].cIndex].note);
                    userdb[dbIndex].cIndex++;
                    free(temp);
                }
            }       
            if(!stringCmp(wBuffer, "nan")){
                while((temp=readLine_csv(wBuffer, '|'))){
                    stringCopy(temp, userdb[dbIndex].withdraw[userdb[dbIndex].wIndex].note);
                    userdb[dbIndex].wIndex++;
                    free(temp);
                }
            } 
            if(!stringCmp(bBuffer, "nan")){
                while((temp=readLine_csv(bBuffer, '|'))){
                    stringCopy(temp, userdb[dbIndex].burrow[userdb[dbIndex].bIndex].note);
                    userdb[dbIndex].bIndex++;
                    free(temp);
                }
            }
            if(!stringCmp(pBuffer, "nan")){
                while((temp=readLine_csv(pBuffer, '|'))){
                    stringCopy(temp, userdb[dbIndex].repay[userdb[dbIndex].pIndex].note);
                    userdb[dbIndex].pIndex++;
                    free(temp);
                }
            }
            dbIndex++;
        }
        fclose(fp);
    }
    else
        printf(RED"[!]Loading database Failed.\n"RESET);
}
void saveAllData(){
    printf(PURPLE"\t\tSaving Data...\n"RESET);
    FILE *fp = fopen("data.csv", "w");
    if(fp != NULL){
        for(int i=0; i<dbIndex; i++){                 //,yy,mm,dd,active
            fprintf(fp, "%u,%s,%s,%s,\"%s\",%s,\"%s\",%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%ld,%ld,%u", userdb[i].id, userdb[i].name, 
                                                                            userdb[i].nrc, userdb[i].email, userdb[i].pass, 
                                                                            userdb[i].phone, userdb[i].address, userdb[i].curr_amt,
                                                                            userdb[i].income, userdb[i].loan_amt, userdb[i].loan_rate, 
                                                                            userdb[i].isAdmin, userdb[i].isPer, userdb[i].acc_status, 
                                                                            userdb[i].loan_status, userdb[i].p_count, userdb[i].start.yy, 
                                                                            userdb[i].start.mm, userdb[i].start.dd, userdb[i].active, 
                                                                            userdb[i].lock_time, userdb[i].transLimit);
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].tIndex; j++){
                fprintf(fp, "%s", userdb[i].transfer[j].note);
                if(j+1 < userdb[i].tIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].rIndex; j++){
                fprintf(fp, "%s", userdb[i].receive[j].note);
                if(j+1 < userdb[i].rIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].cIndex; j++){
                fprintf(fp, "%s", userdb[i].cashIn[j].note);
                if(j+1 < userdb[i].cIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].wIndex; j++){
                fprintf(fp, "%s", userdb[i].withdraw[j].note);
                if(j+1 < userdb[i].wIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].bIndex; j++){
                fprintf(fp, "%s", userdb[i].burrow[j].note);
                if(j+1 < userdb[i].bIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].pIndex; j++){
                fprintf(fp, "%s", userdb[i].repay[j].note);
                if(j+1 < userdb[i].pIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", ',');
            fprintf(fp, "%c", '\n');
        }
        printf(PURPLE"\t\tData saved...\n"G_RESET);
        if(fp != NULL)
            fclose(fp);
    }
    else
        printf(RED"File opening Failed\n"RESET);
}
