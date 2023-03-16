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
#define LOAN           3
/* dd-mm-yy info structure*/
struct Date{
    int dd;
    int mm;
    int yy;
};
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
    float loan_rate; // interest
    unsigned int isPer: 1; // Personal or business account
    // unsigned int acc_lvl: 2; // 0 low, 1 normal, 2 special
    unsigned int acc_status: 1; // 1 for active, 0 for suspended
    unsigned int loan_status: 1; // 1 for burrowed, 0 for loan clear
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

/***************************************************************************************/
//////////////////////////////////Bank Functions/////////////////////////////////////////
/***************************************************************************************/
/*
    if not Existed, global var 'uIndex' = -1, or if existed, uIndex holds index of that existing one.
*/
void isEmailExisted(char *uEmail){
    uIndex = -1;
    for(int i=0; i<dbIndex; i++){
        if(stringCmp(uEmail, userdb[i].email))
            uIndex = i;
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
    if(process == TRANSFER_MONEY){
        char *sender = toChar(amount);
        char *recipient = toChar(amount);
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
        char *newTrans = toChar(amount);
        stringConcat(&newTrans, "MMK saved in the account");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].record[userdb[userFound].tIndex].note);
        userdb[userFound].tIndex++;
    }
    else if(process == WITHDRAW){
        char *newTrans = toChar(amount);
        stringConcat(&newTrans, "MMK withdrawn from the account");
        stringConcat(&newTrans, time_buff);
        stringCopy(newTrans, userdb[userFound].record[userdb[userFound].tIndex].note);
        userdb[userFound].tIndex++;
    }
    else if(process == LOAN){
        printf("LOAN record process not available\n");
    }
}
/* Take amount from transaction record and RETRUN as INT */
int get_amount_from_trans(char *transact){
    char *temp = subString(transact, 0, indexOf(transact, "MMK"));
    int amount = toInt(temp);
    return amount;
}
/* Transform [mm-dd-yy-H:M] format to struct Date format to be arithmetically operated */
struct Date *str_To_StructDate(char *strDate){
    int start = indexOf(strDate, "[");
    int end = indexOf(strDate, "]");
    strDate = subString(strDate, start+1, end);
    struct Date *date = (struct Date*)malloc(sizeof(struct Date));
    int col = 0;
    char *temp = NULL;
    while((temp=readLine_csv(strDate, '-'))){
        switch(col){
            case 0: date->mm = month_number(temp); break;
            case 1: date->dd = toInt(temp); break;
            case 2: date->yy = toInt(temp); break;
            default: break;
        }
        free(temp);
        col++;
    }
    return date;
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

void transfer_money(int idxS, int idxR, unsigned int amount){
    if(isTrans_amt_limit_OK(idxS, amount)){
        if(isTrans_amt_limit_OK(idxR, amount)){
            userdb[idxS].curr_amt -= amount;
            userdb[idxR].curr_amt += amount;
            transaction_record(amount, TRANSFER_MONEY);
            printf("Transaction completed\n");
            printf("Transaction: %s\n", userdb[userFound].record[userdb[userFound].tIndex-1].note);
            printf("Balance: %u\n", userdb[userFound].curr_amt);
        }
        else{
            printf("Sorry, Recipient's account have reached max transaction amount limit.\nTry it, tomorrow.\n");
        }
    }
    else{
        printf("Sorry, You have reached your max transaction amount limit.\nTry it, tomorrow.\n");
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
            transaction_record(amount, WITHDRAW);
        }
        printf("Balance: %u\n", userdb[uIdx].curr_amt);
    }
    else{
        printf("Sorry, You have reached your max transaction amount limit.\nTry it, tomorrow.\n");       
    }
}
user_sector(){
    char userIn[30]; 
    printf("User name >> %s <<\n", userdb[userFound].name);
    while(1){
        printf("Transfer,Withdraw,Info,Cash In,Loan,History,Exit or 1 to main page: ");
        scanf(" %[^\n]%*c", userIn);
        if(stringCmp(wordLower(userIn), "transfer")){
            while(1){
                printf("Enter a phone number to receive: ");
                scanf(" %[^\n]%*c", userIn);
                isPhoneExisted(userIn);
                if(uIndex != userFound){
                    if(uIndex == -1){
                        printf("Received user's phone number not existed\n");
                        printf("Press 1 to user_sector or <enter> to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "1\n")) user_sector();
                    }else break;                    
                }
                else{
                    printf("Sorry you can't transfer to your same account\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector();
                }
            }
            phFound = uIndex;
            printf("Recipient found: %s\nEmail: %s\n", userdb[phFound].name, userdb[phFound].email);
            while(1){
                printf("Minimum amount allowable to be transferred >> 1000MMK\n");
                printf("Enter an amount to be transferred: ");
                scanf(" %d", &choice);
                fflush(stdin);
                if(choice >= 1000){
                    if(userdb[userFound].curr_amt-1000>=choice) break;
                    else{
                        printf("Insufficient balance\nPress 1 to user_sector or <enter> to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "1\n")) user_sector();
                    }
                }
                else{
                    printf("Transferred amount shouldn't be less than 10USD\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector(); 
                }
            }
            int amount_t = choice;
            while(1){
                printf("Enter password to send: ");
                scanf(" %[^\n]%*c", userIn);
                if(stringCmp(userIn, userdb[userFound].pass)) break;
                else{
                    printf("Wrong credential\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector();
                }
            }
            transfer_money(userFound, phFound, amount_t);
        }
        else if(stringCmp(wordLower(userIn), "withdraw")){
            while(1){
                printf("Minimum amount allowable to be withdrawn >> 5000MMK\n");
                printf("Submit amount to be withdrawn: ");
                scanf(" %d", &choice);
                fflush(stdin);
                if(choice >= 5000){
                    if(userdb[userFound].curr_amt-1000 >= choice) break;
                    else{
                        printf("Insufficient amount in the account\nPress 1 to user_sector or <enter> to re-submit: ");
                        fgets(userIn, 30, stdin);
                        if(stringCmp(userIn, "1\n")) user_sector(); 
                    }
                }
                else{   
                    printf("Amount not in the allowable limits\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector();            
                }
            }
            cashIn_withdraw(userFound, choice, WITHDRAW);

        }
        else if(stringCmp(wordLower(userIn), "info")){
            printf("Name: %s\nNRC: %s\nEmail: %s\nPhone: %s\nAddress: %s\nBalance: %uMMK\n", userdb[userFound].name, userdb[userFound].nrc,
                                                            userdb[userFound].email, userdb[userFound].phone,
                                                            userdb[userFound].address, userdb[userFound].curr_amt);
            printf("Income: %uMMK\nloan amount:%uMMK\n[Acc_Type: %s]\t[Acc_Status: %s]\t[Loan_Status: %s]\n", userdb[userFound].income,
                                                    userdb[userFound].loan_amt,
                                                    (userdb[userFound].isPer)? "Personal":"Business",
                                                    (userdb[userFound].acc_status)? "Active":"Suspended",
                                                    (userdb[userFound].loan_status)? "Burrowed":"Clear");
        }
        else if(stringCmp(wordLower(userIn), "cash in")){
            while(1){
                printf("Minimum amount allowable to be submitted >> 5000MMK\n");
                printf("Please insert CASH in the slot: ");
                scanf(" %d", &choice);
                fflush(stdin);
                if(choice >= 5000) break;
                else{
                    printf("Amount not in the allowable limits\nPress 1 to user_sector or <enter> to re-submit: ");
                    fgets(userIn, 30, stdin);
                    if(stringCmp(userIn, "1\n")) user_sector();
                }
            }
            cashIn_withdraw(userFound, choice, CASH_IN);
        }
        else if(stringCmp(wordLower(userIn), "history")){               
            for(int i=0; i<userdb[userFound].tIndex; i++){
                printf("%d) %s\n", i+1, userdb[userFound].record[i].note);
            }
        }
        else if(stringCmp(wordLower(userIn), "loan")) printf("Loan unavailable\n");
        else if(stringCmp(wordLower(userIn), "exit")) exitProgram();
        else if(stringCmp(userIn, "1")) welcome();
    }
}
void login_section(){
    char uEmail[50];
    char uPass[50];
    printf("LOGIN SECTOR\n");
    printf("Email: ");
    scanf(" %[^\n]%*c", uEmail);
    printf("Password: ");
    scanf(" %[^\n]%*c", uPass);
    isEmailExisted(uEmail);
    if(uIndex != -1){
        if(!stringCmp(uPass, userdb[uIndex].pass)){
            printf("Password incorrect!\n");
            funcCall(login_section, "login_section");
        }
    }
    else{
        printf("Email doesn't exist!\n");
        funcCall(login_section, "login_section");
    }
    userFound = uIndex;
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
        printf("An email shouldn't contain any special or upper case characters.\n");
        return 0;
    }
    if(stringCount(emailName, ".") >= 2){
        printf("An email shouldn't contain \".\" more than once in user name!!\n");
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
            printf("Email already existed\n");
            funcCall(mail_form, "mail_form");
        }
    }
    else{
        printf("Email format not valid\n");
        funcCall(mail_form, "mail_form");
    }
}
void nrc_form(){
    printf("NRC number: ");
    scanf(" %[^\n]%*c", userdb[dbIndex].nrc);
    if(isNRCValid(userdb[dbIndex].nrc)){
        isNRCExisted(userdb[dbIndex].nrc);
        if(uIndex != -1){
            printf("NRC already existed\n");
            funcCall(nrc_form, "nrc_form");
        }
    }
    else{
        printf("NRC format not valid\n");
        funcCall(nrc_form, "nrc_form");
    }
}
void pass_form(){
    char confirmPass[20];
    printf("Password >>\n[not less than 12 chars including at least one Capital and number and special]: ");
    scanf(" %[^\n]%*c", userdb[dbIndex].pass);
    if(isPassValid(userdb[dbIndex].pass)){
        printf("Re-type password: ");
        scanf(" %[^\n]%*c", confirmPass);
        if(!stringCmp(userdb[dbIndex].pass, confirmPass)){
            printf("Password not matched\n");
            funcCall(pass_form, "pass_form");
        }
    }
    else{
        printf("Password format invalid or weak password\n");
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
            printf("Phone number already existed\n");
            funcCall(phone_form, "phone_form");
        }
    }
    else {
        printf("Phone number not valid\n");
        funcCall(phone_form, "phone_form");
    }
}

void registration(){
    printf("USER REGISTRATION\n");
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
    userdb[dbIndex].loan_rate = 0.3;            // bank's default loan interest
    printf("account-type[1 for Personal or 0 for business]: ");
    scanf(" %d", &choice);
    fflush(stdin);
    userdb[dbIndex].isPer = choice;
    userdb[dbIndex].acc_status = 1;                                       // default account active
    userdb[dbIndex].loan_status = 0;                                      // default loan status clear
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
    printf("Welcome to ebanking service\n");
    while(1){
        printf("[login, register or exit]: ");
        scanf(" %[^\n]%*c", userIn);
        if(stringCmp(wordLower(userIn), "login")) login_section();
        else if(stringCmp(wordLower(userIn), "register")) registration();
        else if(stringCmp(wordLower(userIn), "exit")) exitProgram();
        else printf("[!]Wrong Input\n");
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
    printf("Loading data from file...\n");
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
                    case 10: userdb[dbIndex].loan_rate = toFloat(temp); break;
                    case 11: userdb[dbIndex].isPer = toInt(temp); break;
                    case 12: userdb[dbIndex].acc_status = toInt(temp); break;
                    case 13: userdb[dbIndex].loan_status = toInt(temp); break;
                    case 14: userdb[dbIndex].transLimit = toInt(temp); break;
                    case 15:
                        while((trans=readLine_csv(temp, '|'))){
                            stringCopy(trans, userdb[dbIndex].record[userdb[dbIndex].tIndex].note);
                            userdb[dbIndex].tIndex++;
                            free(trans);
                        }
                    break;
                }
                free(temp);
                col++;
                if(col>15) break;
            }
            dbIndex++;
        }
        fclose(fp);
    }
    else
        printf("[!]Loading database Failed.\n");
}
void printAllData(){
    for(int i=0; i<dbIndex; i++){
        printf("%u, %s, %s, %s, %s\n%s, %s, %u, %u, %u\n%.2f, %d, %d, %d, %d\n", userdb[i].id, userdb[i].name, userdb[i].nrc, 
                                                                        userdb[i].email, userdb[i].pass, userdb[i].phone,
                                                                        userdb[i].address, userdb[i].curr_amt, userdb[i].income,
                                                                        userdb[i].loan_amt, userdb[i].loan_rate, userdb[i].isPer,
                                                                        userdb[i].acc_status, userdb[i].loan_status, userdb[i].transLimit);
    }
}
void saveAllData(){
    printf("Saving Data...\n");
    FILE *fp = fopen("data.csv", "w");
    if(fp != NULL){
        for(int i=0; i<dbIndex; i++){
            fprintf(fp, "%u,%s,%s,%s,\"%s\",%s,\"%s\",%u,%u,%u,%.2f,%d,%d,%d,%d", userdb[i].id, userdb[i].name, userdb[i].nrc,
                                                                            userdb[i].email, userdb[i].pass, userdb[i].phone,
                                                                            userdb[i].address, userdb[i].curr_amt, userdb[i].income,
                                                                            userdb[i].loan_amt, userdb[i].loan_rate, userdb[i].isPer,
                                                                            userdb[i].acc_status, userdb[i].loan_status, userdb[i].transLimit);
            if(userdb[i].tIndex > 0) fprintf(fp, "%c", ',');
            for(int j=0; j<userdb[i].tIndex; j++){
                fprintf(fp, "%s", userdb[i].record[j].note);
                if(j+1 < userdb[i].tIndex) fprintf(fp, "%c", '|');
            }
            fprintf(fp, "%c", '\n');
        }
        printf("Data saved...\n");
        if(fp != NULL)
            fclose(fp);
    }
    else
        printf("File opening Failed\n");
}
