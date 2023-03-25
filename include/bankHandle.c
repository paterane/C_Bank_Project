//
// Created by Peter Oo on 02/28/2023
//
#include <stdio.h>
#include <stdlib.h>
#include "bankHandle.h"
#include "utils.h"

#define size 1000 // maximum number of users allowed in database
#define TRANSFER_MONEY 0
#define CASH_IN        1
#define WITHDRAW       2
#define BURROW         3
#define REPAY          4
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
    unsigned int isPer: 1; // Personal or business account
    unsigned int acc_status: 1; // 1 for active, 0 for suspended
    unsigned int loan_status: 1; // 1 for burrowed, 0 for loan clear
    unsigned int p_count: 2; // counter for wrong password or email per user
    long lock_time; // time of account lock
    struct Date start; // Date of account creating
    long active; // time of account login
    unsigned int transLimit; //transaction amount limit per day
    struct Transaction record[100]; // transaction records per user
    int tIndex; //transaction index
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
        stringCopy(sender, userdb[userFound].record[userdb[userFound].tIndex].note);
        userdb[userFound].tIndex++;
        stringConcat(&recipient, "MMK recieved from ");
        stringConcat(&recipient, userdb[userFound].name);
        stringConcat(&recipient, time_buff);
        stringCopy(recipient, userdb[phFound].record[userdb[phFound].tIndex].note);
        userdb[phFound].tIndex++;
    }
    else if(process == CASH_IN){
        stringConcat(&newTrans, "MMK saved in the account");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].record[userdb[userFound].tIndex].note);
        userdb[userFound].tIndex++;
    }
    else if(process == WITHDRAW){
        stringConcat(&newTrans, "MMK withdrawn from the account");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].record[userdb[userFound].tIndex].note);
        userdb[userFound].tIndex++;
    }
    else if(process == BURROW){
        stringConcat(&newTrans, "MMK burrowed from the bank");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].record[userdb[userFound].tIndex].note);
        userdb[userFound].tIndex++;
    }
    else if(process == REPAY){
        stringConcat(&newTrans, "MMK repaid to the bank");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].record[userdb[userFound].tIndex].note);
        userdb[userFound].tIndex++;
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
int isTrans_amt_limit_OK(int idx, unsigned int amount){
    char time_buff[30] = {0};
    current_time(time_buff, 30); // format [Mar-15-2023-06:52PM]
    struct Date *curr_date = str_To_StructDate(time_buff);
    struct Date *hist_date = NULL;
    for(int i=userdb[idx].tIndex-1; i>=0; i--){
        hist_date = str_To_StructDate(userdb[idx].record[i].note);
        if(curr_date->dd == hist_date->dd && curr_date->mm == hist_date->mm && curr_date->yy == hist_date->yy){  
            amount += get_amount_from_trans(userdb[idx].record[i].note);
            free(hist_date);
        }
        else{
            free(hist_date);
            break;
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
    if(isTrans_amt_limit_OK(idxS, amount)){
        if(isTrans_amt_limit_OK(idxR, amount)){
            userdb[idxS].curr_amt -= amount;
            userdb[idxR].curr_amt += amount;
            service_fee(idxS, amount);
            transaction_record(amount, TRANSFER_MONEY);
            printf("Transaction:"BLUE" %s\n"RESET, userdb[idxS].record[userdb[idxS].tIndex-1].note);
            printf("Balance:"BLUE" %u mmk\n"RESET, userdb[idxS].curr_amt);
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
    if(isTrans_amt_limit_OK(uIdx, amount)){
        if(process==CASH_IN){
            userdb[uIdx].curr_amt += amount;
            transaction_record(amount, CASH_IN);
        }
        else if(process==WITHDRAW){
            userdb[uIdx].curr_amt -= amount;
            service_fee(uIdx, amount);
            transaction_record(amount, WITHDRAW);
        }
        printf("Transaction:"BLUE" %s\n"RESET, userdb[uIdx].record[userdb[uIdx].tIndex-1].note);
        printf("Balance:"BLUE" %u mmk\n"RESET, userdb[uIdx].curr_amt);
    }
    else{
        printf(RED"Sorry, You have reached your max transaction amount limit.\nTry it, tomorrow.\n"RESET);       
    }
}
/*
    RETURN char ARRAY ended by -1 OF INDEXES OF TRANSACTION NOTES of a given user if keyword looking for is found or arrary of -1 is 
    returned.
*/
void trans_note_index(int uIdx, char *strIdx){
    int idx=0;
    int j = 0;
    for(int i=0; i<100; i++) trans_indexes[i] = -1; // clear array's content
    for(idx=userdb[uIdx].tIndex-1; idx>=0; idx--){ // Searching from latest
        int index = indexOf(userdb[uIdx].record[idx].note, strIdx);
        if(index != -1) trans_indexes[j++] = idx;
    }
    trans_indexes[j] = -1;
}
/*
    RETURN: days left for loan repayment, if exceed, negative numbers are returned.
*/
int days_left(int uIdx){
    long curr_time = current_time_L();
    trans_note_index(uIdx, "burrow"); // you will get index found at trans_indexes array
    struct Date *time_info = str_To_StructDate(userdb[uIdx].record[trans_indexes[0]].note);
    long due_time = timeStruct_to_L(time_info) + 2592000; // 30 days (30*24*60*60)
    long time_left = diff_time_L(due_time, curr_time);
    return time_left / 86400;
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
        printf("Transaction:"BLUE" %s\n"RESET, userdb[uIdx].record[userdb[uIdx].tIndex-1].note);
        printf("Balance:"BLUE" %u mmk"RESET"\nLoan amount: "L_BLUE"%u mmk"RESET \
                "\nLoan interest: "BLUE"%u mmk"RESET"\n", userdb[uIdx].curr_amt, userdb[uIdx].loan_amt - userdb[uIdx].loan_rate, 
                                                        userdb[uIdx].loan_rate);
    }
    else if(process == REPAY){
        if(amount < userdb[uIdx].loan_amt){
            userdb[uIdx].loan_amt -= amount;
            userdb[uIdx].curr_amt -= amount;
            transaction_record(amount, REPAY);
            printf("transaction: "BLUE"%s"RESET"\n", userdb[uIdx].record[userdb[uIdx].tIndex-1].note);
        }
        else{
            userdb[uIdx].curr_amt -= amount;
            userdb[uIdx].curr_amt +=  amount - userdb[uIdx].loan_amt;
            transaction_record(userdb[uIdx].loan_amt, REPAY);
            printf("transaction: "BLUE"%s"RESET"\n", userdb[uIdx].record[userdb[uIdx].tIndex-1].note);
            userdb[uIdx].loan_status = 0;
            userdb[uIdx].loan_amt = 0;
            userdb[uIdx].loan_rate = 0;
        }
    }
}
user_sector(){
    char userIn[30];
    printf("User name >>"BLUE" %s "RESET"<<\n", userdb[userFound].name);
    while(1){
        printf(GREEN"Transfer"RESET","GREEN" Withdraw"RESET","GREEN" Info" \
                RESET","GREEN" Cash In"RESET","GREEN" Loan"RESET","GREEN" Log" \
                RESET","GREEN" Exit"RESET" or"GREEN" 1"RESET" to main page: ");
        scanf(" %[^\n]%*c", userIn);
        if(stringCmp(wordLower(userIn), "transfer")){
            while(1){
                printf("Enter a phone number to receive: ");
                scanf(" %[^\n]%*c", userIn);
                isPhoneExisted(userIn);
                if(uIndex != userFound){
                    if(uIndex == -1){
                        printf(RED"Received user's phone number not existed\n"RESET);
                        printf("Press 1 to user_sector or <enter> to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "1\n")) user_sector();
                    }else break;                    
                }
                else{
                    printf(RED"Sorry you can't transfer to your same account"RESET"\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector();
                }
            }
            phFound = uIndex;
            printf("Recipient found:"BLUE" %s\n"RESET"Email:"BLUE" %s\n"RESET, userdb[phFound].name, userdb[phFound].email);
            while(1){
                printf(GREEN"Minimum amount allowable to be transferred >> 1000MMK\n"RESET);
                printf("Enter an amount to be transferred: ");
                scanf(" %d", &choice);
                fflush(stdin);
                if(choice >= 1000){
                    if(userdb[userFound].curr_amt-1000>=choice) break;
                    else{
                        printf(RED"Insufficient balance"RESET"\nPress 1 to user_sector or <enter> to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "1\n")) user_sector();
                    }
                }
                else{
                    printf(RED"Transferred amount shouldn't be less than 10USD"RESET"\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector(); 
                }
            }
            int amount_t = choice;
            while(1){
                printf("Enter password to send: ");
                password_input(userIn);
                if(stringCmp(userIn, userdb[userFound].pass)) break;
                else{
                    printf(RED"Wrong credential"RESET"\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector();
                }
            }
            transfer_money(userFound, phFound, amount_t);
        }
        else if(stringCmp(wordLower(userIn), "withdraw")){
            while(1){
                printf(GREEN"Minimum amount allowable to be withdrawn >> 5000MMK\n"RESET);
                printf("Submit amount to be withdrawn: ");
                scanf(" %d", &choice);
                fflush(stdin);
                if(choice >= 5000){
                    if(userdb[userFound].curr_amt-1000 >= choice) break;
                    else{
                        printf(RED"Insufficient amount in the account"RESET"\nPress 1 to user_sector or <enter> to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "1\n")) user_sector(); 
                    }
                }
                else{   
                    printf(RED"Amount not in the allowable limits"RESET"\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector();            
                }
            }
            cashIn_withdraw(userFound, choice, WITHDRAW);

        }
        else if(stringCmp(wordLower(userIn), "info")){
            printf("Name: "BLUE"%s\n"RESET"NRC: "L_BLUE"%s\n"RESET"Email: "BLUE \ 
                    "%s\n"RESET"Phone: "L_BLUE"%s\n"RESET"Address: "BLUE"%s\n"RESET \
                    "Balance: "L_BLUE"%uMMK\n"RESET, userdb[userFound].name, userdb[userFound].nrc,
                                                     userdb[userFound].email, userdb[userFound].phone,
                                                     userdb[userFound].address, userdb[userFound].curr_amt);
            unsigned int loan_amt = 0;
            if(userdb[userFound].loan_status == 1){
                trans_note_index(userFound, "burrow");
                loan_amt = get_amount_from_trans(userdb[userFound].record[trans_indexes[0]].note);
            }
            printf("Income: "BLUE"%uMMK\n"RESET"Burrowed amount: "L_BLUE"%uMMK\n"RESET"loan interest: "BLUE"%uMMK\n"RESET \
                    "[Acc_Type: "BLUE"%s"RESET"]\t[Loan_Status: "BLUE"%s"RESET"]\n", userdb[userFound].income, loan_amt,
                                                    userdb[userFound].loan_rate,(userdb[userFound].isPer)? "Personal":"Business",
                                                    (userdb[userFound].loan_status)? "Burrowed":"Clear");
        }
        else if(stringCmp(wordLower(userIn), "cash in")){
            while(1){
                printf(GREEN"Minimum amount allowable to be submitted >> 5000MMK\n"RESET);
                printf("Please insert CASH in the slot: ");
                scanf(" %d", &choice);
                fflush(stdin);
                if(choice >= 5000) break;
                else{
                    printf(RED"Amount not in the allowable limits"RESET"\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector();
                }
            }
            cashIn_withdraw(userFound, choice, CASH_IN);
        }
        else if(stringCmp(wordLower(userIn), "log")){               
            for(int i=0; i<userdb[userFound].tIndex; i++){
                printf(BLUE"%d"RESET") "L_BLUE"%s\n"RESET, i+1, userdb[userFound].record[i].note);
            }
        }
        else if(stringCmp(wordLower(userIn), "loan")){
            if(userdb[uIndex].loan_status == 1){
                int time_left = days_left(userFound);
                printf("Loan amount left to be repaid: "BLUE"%u mmk"RESET"\n", userdb[userFound].loan_amt);
                if(time_left < 0) printf("Loan overdue\nContact to HO\n time overdue:"RED" %d day%c"RESET, (-1)*time_left,
                                                                                                ((-1)*time_left > 1)? 's':' ');
                else printf("Time left to repay:"BLUE" %d day%c"RESET"\n", time_left, (time_left > 1)? 's':' ');
            }
            else    printf("Loan Status Clear\nWould you like to get some loan...?\n");
            printf("Press 1 to user_sector or <enter> to continue: ");
            fgets(userIn, 30, stdin);
            if(stringCmp(userIn, "1\n")) user_sector();                                     // change it later with admin approval
            if(userdb[userFound].loan_status == 0 && userdb[userFound].income >= 400000 && userdb[userFound].curr_amt >= 200000){
                while(1){
                    printf("Enter an amount to be burrowed: ");
                    scanf(" %d", &choice);
                    fflush(stdin);
                    if(choice <= 2*userdb[userFound].income && choice >= 400000)
                        break;
                    else{
                        printf(RED"Your max loan amount is limited to double of income, and\nmini loan amount at least 400,000"RESET \
                                "\nPress 1 to user_sector or <enter> to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "1\n")) user_sector();
                    }
                }
                burrow_repay(userFound, choice, BURROW);
            }
            else{
                if(userdb[userFound].income < 400000){
                    printf(RED"Sorry, you can't get loan as your salary is lower than 400,000MMK"RESET"\n");
                    user_sector();
                }
                else if(userdb[userFound].curr_amt < 200000){
                    printf(RED"Sorry, you can't get loan since money in the account is lower than 200,000MMK"RESET"\n");
                    user_sector();
                }
                while(1){
                    printf("Enter an amount to be repaid for loan: ");
                    scanf(" %d", &choice);
                    fflush(stdin);
                    trans_note_index(userFound, "burrow");
                    int a = trans_indexes[0];
                    unsigned int monthly_repay = (get_amount_from_trans(userdb[userFound].record[a].note)+userdb[userFound].loan_rate) / 30;
                    if(userdb[uIndex].curr_amt - 1000 >= choice && choice >= monthly_repay){
                        burrow_repay(userFound, choice, REPAY);
                        break;
                    }
                    else{
                        printf(RED"Sorry, insufficient balance or repaid amount should not less than monthly repay amount: %u mmk"RESET \
                                    "\nPress 1 to user_sector or <enter> to re-submit: ", monthly_repay);
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "1\n")) user_sector();
                    }
                }
            }
        }
        else if(stringCmp(wordLower(userIn), "exit")) exitProgram();
        else if(stringCmp(userIn, "1")) welcome();
    }
}
void login_section(){
    // int locked = 0;
    char uEmail[50];
    char uPass[50];
    long curr_time = current_time_L();
    printf(YELLOW"LOGIN SECTOR\n"RESET);
    printf("Email: ");
    scanf(" %[^\n]%*c", uEmail);
    isEmailExisted(uEmail);
    if(uIndex != -1){
        if(diff_time_L(curr_time, userdb[uIndex].active) >= 7776000.0) userdb[uIndex].acc_status = 0;
        if(userdb[uIndex].p_count >= 3){
            userdb[uIndex].p_count = 0;
            userdb[uIndex].lock_time = curr_time;
        }
        if(diff_time_L(curr_time, userdb[uIndex].lock_time) <= 300.0 || userdb[uIndex].acc_status == 0){
            if(userdb[uIndex].acc_status == 0) printf(RED"Account suspended due to inactive status, contact to HO"RESET"\n");
            else printf(RED"Account locked temporarily\nTry another"RESET"\n");
            funcCall(login_section, "login_section");
        }
        printf("Password: ");
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
    printf("An Email: ");
    scanf(" %[^\n]%*c", userdb[dbIndex].email);
    if(isMailValid(userdb[dbIndex].email, "a-z0-9")){
        isEmailExisted(userdb[dbIndex].email);
        if(uIndex == -1){
            printf("Name: ");
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
    printf("NRC number: ");
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
    printf("Password >>\n[not less than 12 chars including at least one Capital and number and special]: ");
    password_input(userdb[dbIndex].pass);
    if(isPassValid(userdb[dbIndex].pass)){
        printf("Re-type password: ");
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
    printf("Enter a phone number: ");
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
    printf(YELLOW"USER REGISTRATION\n"RESET);
    mail_form();
    nrc_form();
    pass_form();
    phone_form();
    printf("Address: ");
    scanf(" %[^\n]%*c", userdb[dbIndex].address);
    userdb[dbIndex].curr_amt = 1000;            // default money amount in account for initial state
    printf("Enter monthly income: ");
    scanf(" %d", &userdb[dbIndex].income);
    fflush(stdin);
    userdb[dbIndex].loan_amt = 0;               // default loan clear
    userdb[dbIndex].loan_rate = 0;            // default loan rate
    printf("account-type[1 for Personal or 0 for business]: ");
    scanf(" %d", &choice);
    fflush(stdin);
    userdb[dbIndex].isPer = choice;
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
    printf("Press 1 to go back to Main page or 0 to exit: ");
    scanf(" %d",&choice);
    fflush(stdin);
    if(choice==1) welcome();
    else          exitProgram();
}
void welcome(){
    char userIn[30];
    printf(YELLOW"Welcome to ebanking service\n"RESET);
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
    printf("Press 1 to main page or <enter> to %s: ", fname);
    fgets(userIn, 30, stdin);
    if(stringCmp(userIn, "1\n"))    welcome();
    else    f();
}
/*
    Save data to file and exit the program
*/
void exitProgram(){
    saveAllData();
    exit(0);
}
// id,name,nrc,email,pass,phone,address,curr_amt,income,loan_amt,loan_rate,isPer,acc_status,loan_status,translimit,records,transactions
void loadingDataFromFile(){
    printf(B_GROUND PURPLE"Loading data from file...\n"RESET);
    FILE *fp = NULL;
    char buffer[10000]; // note 100 * record 100
    fp = fopen("data.csv", "r");
    if(fp != NULL){
        while(fscanf(fp, " %[^\n]%*c", buffer) != EOF){
            char *temp = NULL;
            char *trans = NULL;
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
                    case 11: userdb[dbIndex].isPer = toInt(temp); break;
                    case 12: userdb[dbIndex].acc_status = toInt(temp); break;
                    case 13: userdb[dbIndex].loan_status = toInt(temp); break;
                    case 14: userdb[dbIndex].p_count = toInt(temp); break;  
                    case 15: userdb[dbIndex].start.yy = toInt(temp); break;  
                    case 16: userdb[dbIndex].start.mm = toInt(temp); break;
                    case 17: userdb[dbIndex].start.dd = toInt(temp); break;
                    case 18: userdb[dbIndex].active = toInt(temp); break;    
                    case 19: userdb[dbIndex].lock_time = toInt(temp); break;
                    case 20: userdb[dbIndex].transLimit = toInt(temp); break;
                    case 21:
                        while((trans=readLine_csv(temp, '|'))){
                            stringCopy(trans, userdb[dbIndex].record[userdb[dbIndex].tIndex].note);
                            userdb[dbIndex].tIndex++;
                            free(trans);
                        }
                    break;
                }
                free(temp);
                if(col>=21) break;
                col++;
            }
            dbIndex++;
        }
        fclose(fp);
    }
    else
        printf(RED"[!]Loading database Failed.\n"RESET);
}
void saveAllData(){
    printf(PURPLE"Saving Data...\n"RESET);
    FILE *fp = fopen("data.csv", "w");
    if(fp != NULL){
        for(int i=0; i<dbIndex; i++){                 //,yy,mm,dd,active
            fprintf(fp, "%u,%s,%s,%s,\"%s\",%s,\"%s\",%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%ld,%ld,%u", userdb[i].id, userdb[i].name, 
                                                                            userdb[i].nrc, userdb[i].email, userdb[i].pass, 
                                                                            userdb[i].phone, userdb[i].address, userdb[i].curr_amt, 
                                                                            userdb[i].income, userdb[i].loan_amt, userdb[i].loan_rate, 
                                                                            userdb[i].isPer, userdb[i].acc_status, userdb[i].loan_status, 
                                                                            userdb[i].p_count, userdb[i].start.yy, userdb[i].start.mm,
                                                                            userdb[i].start.dd, userdb[i].active, userdb[i].lock_time, 
                                                                            userdb[i].transLimit);
            if(userdb[i].tIndex > 0) fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].tIndex; j++){
                fprintf(fp, "%s", userdb[i].record[j].note);
                if(j+1 < userdb[i].tIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", '\n');
        }
        printf(PURPLE"Data saved...\n"G_RESET);
        if(fp != NULL)
            fclose(fp);
    }
    else
        printf(RED"File opening Failed\n"RESET);
}
