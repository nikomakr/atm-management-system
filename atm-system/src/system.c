#include "header.h"
#include <ctype.h>

const char *RECORDS = "./data/records.txt";
const char *TRANSACTIONS = "./data/transactions.txt";

// ── Validation helpers ─────────────────────────────────────────────────────

// Digits only, 1–15 characters (preserves leading zeros e.g. 0044...)
static int isValidPhone(const char *s)
{
    int len = (int)strlen(s);
    if (len < 1 || len > 15)
        return 0;
    for (int i = 0; i < len; i++)
        if (!isdigit((unsigned char)s[i]))
            return 0;
    return 1;
}

// Exactly 12 digits
static int isValidAccountNbr(const char *s)
{
    if ((int)strlen(s) != 12)
        return 0;
    for (int i = 0; i < 12; i++)
        if (!isdigit((unsigned char)s[i]))
            return 0;
    return 1;
}

// ── File I/O ───────────────────────────────────────────────────────────────

void saveTransactionToFile(struct User u, const char *accountNbr, const char *type,
                           double amount, double newBalance, struct Date date)
{
    FILE *pf = fopen(TRANSACTIONS, "a+");
    if (pf == NULL)
    {
        printf("Error opening transactions file!\n");
        return;
    }

    int count = 0;
    struct Transaction t;
    while (fscanf(pf, "%d %d %s %s %s %lf %lf %d/%d/%d",
                  &t.id, &t.userId, t.username, t.accountNbr,
                  t.type, &t.amount, &t.newBalance,
                  &t.date.month, &t.date.day, &t.date.year) != EOF)
        count++;

    fprintf(pf, "%d %03d %s %s %s %.2lf %.2lf %d/%d/%d\n\n",
            count, u.id, u.name, accountNbr,
            type, amount, newBalance,
            date.month, date.day, date.year);

    fclose(pf);
}

int getAccountFromFile(FILE *ptr, struct Record *r)
{
    return fscanf(ptr, "%d %d %s %s %d/%d/%d %s %s %lf %s",
                  &r->id,
                  &r->userId,
                  r->name,
                  r->accountNbr,
                  &r->deposit.month,
                  &r->deposit.day,
                  &r->deposit.year,
                  r->country,
                  r->phone,
                  &r->amount,
                  r->accountType) != EOF;
}

void saveAccountToFile(FILE *ptr, struct User u, struct Record r)
{
    fprintf(ptr, "%d %03d %s %s %d/%d/%d %s %s %.2lf %s\n\n",
            r.id,
            u.id,
            u.name,
            r.accountNbr,
            r.deposit.month,
            r.deposit.day,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
}

// ── Utility ────────────────────────────────────────────────────────────────

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf(BRED "\n  ✖ Record not found!\n" RESET);
    invalid:
        printf(YELLOW "\n  Enter 0 to try again, 1 for main menu, 2 to exit: " RESET);
        scanf("%d", &option);
        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(u);
        else if (option == 2)
            exit(0);
        else
        {
            printf(RED "  Invalid option.\n" RESET);
            goto invalid;
        }
    }
    else
    {
        printf(YELLOW "\n  Enter 1 for main menu, 0 to exit: " RESET);
        scanf("%d", &option);
    }
    if (option == 1)
    {
        system("clear");
        mainMenu(u);
    }
    else
    {
        system("clear");
        exit(1);
    }
}

void success(struct User u)
{
    int option;
    printf(BGREEN "\n  ✔ Success!\n" RESET);
invalid:
    printf(YELLOW "\n  Enter 1 for main menu, 0 to exit: " RESET);
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf(RED "  Invalid option.\n" RESET);
        goto invalid;
    }
}

// ── Account management ─────────────────────────────────────────────────────

void createNewAcc(struct User u)
{
    struct Record r;
    struct Record records[100];
    int count = 0;
    int maxId = -1;

    system("clear");
    printf("\n" BCYAN "  ===== New Account =====\n\n" RESET);
    printf(YELLOW "  Enter today's date (mm/dd/yyyy): " RESET);
    scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);

    // Load all existing records to check uniqueness and find max id
    FILE *pf = fopen(RECORDS, "a+");
    if (pf == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    rewind(pf);
    struct Record tmp;
    while (getAccountFromFile(pf, &tmp))
    {
        records[count++] = tmp;
        if (tmp.id > maxId)
            maxId = tmp.id;
    }

    // Account number: exactly 12 digits, globally unique
    char inputBuf[50];
    while (1)
    {
        printf("\nEnter the account number (12 digits): ");
        scanf("%s", inputBuf);

        if (!isValidAccountNbr(inputBuf))
        {
            printf("✖ Account number must be exactly 12 digits.\n");
            continue;
        }

        int duplicate = 0;
        for (int i = 0; i < count; i++)
        {
            if (strcmp(records[i].accountNbr, inputBuf) == 0)
            {
                duplicate = 1;
                break;
            }
        }
        if (duplicate)
        {
            printf("✖ This account number already exists. Choose another.\n");
            continue;
        }
        strncpy(r.accountNbr, inputBuf, 12);
        r.accountNbr[12] = '\0';
        break;
    }

    printf("\nEnter the country: ");
    scanf("%s", r.country);

    // Phone: digits only, max 15 digits, leading zeros preserved
    while (1)
    {
        printf("\nEnter the phone number (digits only, up to 15, include country prefix e.g. 0044...): ");
        scanf("%s", inputBuf);
        if (!isValidPhone(inputBuf))
        {
            printf("✖ Phone must contain digits only and be at most 15 digits.\n");
            continue;
        }
        strncpy(r.phone, inputBuf, 15);
        r.phone[15] = '\0';
        break;
    }

    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice: ");
    scanf("%s", r.accountType);

    r.id = maxId + 1;
    r.userId = u.id;

    saveAccountToFile(pf, u, r);
    fclose(pf);
    success(u);
}

void updateAccount(struct User u)
{
    struct Record r;
    struct Record records[100];
    int count = 0;
    char accountNbr[50];
    int found = 0;
    int foundIdx = -1;

    system("clear");
    printf("\n" BCYAN "  ===== Update Account Information =====\n\n" RESET);
    printf(YELLOW "  Enter the account number: " RESET);
    scanf("%s", accountNbr);

    FILE *pf = fopen(RECORDS, "r");
    if (pf == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    while (getAccountFromFile(pf, &r))
    {
        records[count] = r;
        if (strcmp(r.name, u.name) == 0 && strcmp(r.accountNbr, accountNbr) == 0)
        {
            found = 1;
            foundIdx = count;
        }
        count++;
    }
    fclose(pf);

    if (!found)
    {
        stayOrReturn(0, updateAccount, u);
        return;
    }

    int choice;
    printf("\nWhat would you like to update?\n");
    printf("[1]- Phone number\n");
    printf("[2]- Country\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    char inputBuf[50];
    if (choice == 1)
    {
        while (1)
        {
            printf("Enter new phone number (digits only, up to 15): ");
            scanf("%s", inputBuf);
            if (!isValidPhone(inputBuf))
            {
                printf("✖ Phone must contain digits only and be at most 15 digits.\n");
                continue;
            }
            strncpy(records[foundIdx].phone, inputBuf, 15);
            records[foundIdx].phone[15] = '\0';
            break;
        }
    }
    else if (choice == 2)
    {
        printf("Enter new country: ");
        scanf("%s", records[foundIdx].country);
    }
    else
    {
        printf("Invalid choice!\n");
        stayOrReturn(1, updateAccount, u);
        return;
    }

    FILE *pfw = fopen(RECORDS, "w");
    if (pfw == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        struct User ru;
        ru.id = records[i].userId;
        strcpy(ru.name, records[i].name);
        saveAccountToFile(pfw, ru, records[i]);
    }
    fclose(pfw);

    success(u);
}

void transferOwnership(struct User u)
{
    struct Record r;
    struct Record records[100];
    int count = 0;
    char accountNbr[50];
    int found = 0;
    int foundIdx = -1;

    system("clear");
    printf("\n" BCYAN "  ===== Transfer Account Ownership =====\n\n" RESET);
    printf(YELLOW "  Enter the account number to transfer: " RESET);
    scanf("%s", accountNbr);

    FILE *pf = fopen(RECORDS, "r");
    if (pf == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    while (getAccountFromFile(pf, &r))
    {
        records[count] = r;
        if (strcmp(r.name, u.name) == 0 && strcmp(r.accountNbr, accountNbr) == 0)
        {
            found = 1;
            foundIdx = count;
        }
        count++;
    }
    fclose(pf);

    if (!found)
    {
        stayOrReturn(0, transferOwnership, u);
        return;
    }

    char targetName[50];
    printf("Enter the username to transfer ownership to (case-sensitive): ");
    scanf("%s", targetName);

    struct User target;
    int targetFound = 0;
    FILE *uf = fopen("./data/users.txt", "r");
    if (uf == NULL)
    {
        printf("Error opening users file!\n");
        return;
    }
    while (fscanf(uf, "%d %s %s", &target.id, target.name, target.password) != EOF)
    {
        if (strcmp(target.name, targetName) == 0)
        {
            targetFound = 1;
            break;
        }
    }
    fclose(uf);

    if (!targetFound)
    {
        printf("\n✖ User \"%s\" not found!\n", targetName);
        stayOrReturn(0, transferOwnership, u);
        return;
    }

    records[foundIdx].userId = target.id;
    strcpy(records[foundIdx].name, target.name);

    FILE *pfw = fopen(RECORDS, "w");
    if (pfw == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        struct User ru;
        ru.id = records[i].userId;
        strcpy(ru.name, records[i].name);
        saveAccountToFile(pfw, ru, records[i]);
    }
    fclose(pfw);

    sendNotification(target.name, u.name, accountNbr);

    success(u);
}

void removeAccount(struct User u)
{
    struct Record r;
    struct Record records[100];
    int count = 0;
    char accountNbr[50];
    int found = 0;
    int foundIdx = -1;

    system("clear");
    printf("\n" BCYAN "  ===== Remove Account =====\n\n" RESET);
    printf(YELLOW "  Enter the account number: " RESET);
    scanf("%s", accountNbr);

    FILE *pf = fopen(RECORDS, "r");
    if (pf == NULL)
    {
        printf(RED "  Error opening file!\n" RESET);
        return;
    }

    while (getAccountFromFile(pf, &r))
    {
        records[count] = r;
        if (strcmp(r.name, u.name) == 0 && strcmp(r.accountNbr, accountNbr) == 0)
        {
            found = 1;
            foundIdx = count;
        }
        count++;
    }
    fclose(pf);

    if (!found)
    {
        stayOrReturn(0, removeAccount, u);
        return;
    }

    FILE *pfw = fopen(RECORDS, "w");
    if (pfw == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        if (i == foundIdx)
            continue;
        struct User ru;
        ru.id = records[i].userId;
        strcpy(ru.name, records[i].name);
        saveAccountToFile(pfw, ru, records[i]);
    }
    fclose(pfw);

    success(u);
}

void makeTransaction(struct User u)
{
    struct Record r;
    struct Record records[100];
    int count = 0;
    char accountNbr[50];
    int found = 0;
    int foundIdx = -1;

    system("clear");
    printf("\n" BCYAN "  ===== Make Transaction =====\n\n" RESET);
    printf(YELLOW "  Enter the account number: " RESET);
    scanf("%s", accountNbr);

    FILE *pf = fopen(RECORDS, "r");
    if (pf == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    while (getAccountFromFile(pf, &r))
    {
        records[count] = r;
        if (strcmp(r.name, u.name) == 0 && strcmp(r.accountNbr, accountNbr) == 0)
        {
            found = 1;
            foundIdx = count;
        }
        count++;
    }
    fclose(pf);

    if (!found)
    {
        stayOrReturn(0, makeTransaction, u);
        return;
    }

    char *type = records[foundIdx].accountType;
    if (strcmp(type, "fixed01") == 0 ||
        strcmp(type, "fixed02") == 0 ||
        strcmp(type, "fixed03") == 0)
    {
        printf("\n✖ Transactions are not allowed for fixed accounts.\n");
        stayOrReturn(1, makeTransaction, u);
        return;
    }

    printf(GREEN "\n  Current balance: $%.2f\n" RESET, records[foundIdx].amount);
    printf(YELLOW "\n  What would you like to do?\n" RESET);
    printf("   [1]  Deposit\n");
    printf("   [2]  Withdraw\n");
    printf(YELLOW "  ▷ Enter your choice: " RESET);

    int choice;
    scanf("%d", &choice);

    double amount;
    char transactionType[10];
    if (choice == 1)
    {
        printf("Enter amount to deposit: $");
        scanf("%lf", &amount);
        if (amount <= 0)
        {
            printf("\n✖ Amount must be greater than zero.\n");
            stayOrReturn(1, makeTransaction, u);
            return;
        }
        records[foundIdx].amount += amount;
        strcpy(transactionType, "deposit");
    }
    else if (choice == 2)
    {
        printf("Enter amount to withdraw: $");
        scanf("%lf", &amount);
        if (amount <= 0)
        {
            printf("\n✖ Amount must be greater than zero.\n");
            stayOrReturn(1, makeTransaction, u);
            return;
        }
        if (amount > records[foundIdx].amount)
        {
            printf("\n✖ Insufficient balance. Available: $%.2f\n", records[foundIdx].amount);
            stayOrReturn(1, makeTransaction, u);
            return;
        }
        records[foundIdx].amount -= amount;
        strcpy(transactionType, "withdraw");
    }
    else
    {
        printf("Invalid choice!\n");
        stayOrReturn(1, makeTransaction, u);
        return;
    }

    printf(BGREEN "\n  New balance: $%.2f\n" RESET, records[foundIdx].amount);

    FILE *pfw = fopen(RECORDS, "w");
    if (pfw == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        struct User ru;
        ru.id = records[i].userId;
        strcpy(ru.name, records[i].name);
        saveAccountToFile(pfw, ru, records[i]);
    }
    fclose(pfw);

    saveTransactionToFile(u, accountNbr, transactionType, amount,
                          records[foundIdx].amount, records[foundIdx].deposit);

    success(u);
}

void checkAccount(struct User u)
{
    struct Record r;
    char accountNbr[50];
    int found = 0;

    system("clear");
    printf("\n" BCYAN "  ===== Check Account Details =====\n\n" RESET);
    printf(YELLOW "  Enter the account number: " RESET);
    scanf("%s", accountNbr);

    FILE *pf = fopen(RECORDS, "r");
    if (pf == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    while (getAccountFromFile(pf, &r))
    {
        if (strcmp(r.name, u.name) == 0 && strcmp(r.accountNbr, accountNbr) == 0)
        {
            found = 1;
            break;
        }
    }
    fclose(pf);

    if (!found)
    {
        stayOrReturn(0, checkAccount, u);
        return;
    }

    // ── Account details box ──────────────────────────────────────────────
    // Label width = 14, separator = ": ", value = 30, total inner = 46
    char dateStr[20];
    snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d",
             r.deposit.month, r.deposit.day, r.deposit.year);

    printf("\n");
    printf(CYAN "  ┌──────────────────────────────────────────────┐\n" RESET);
    printf(CYAN "  │" RESET BCYAN "               Account Details                " CYAN "│\n" RESET);
    printf(CYAN "  ├──────────────────────────────────────────────┤\n" RESET);
    printf(CYAN "  │" RESET YELLOW "  Account No  " RESET ": %-30s" CYAN "│\n" RESET, r.accountNbr);
    printf(CYAN "  │" RESET YELLOW "  Opened on   " RESET ": %-30s" CYAN "│\n" RESET, dateStr);
    printf(CYAN "  │" RESET YELLOW "  Country     " RESET ": %-30s" CYAN "│\n" RESET, r.country);
    printf(CYAN "  │" RESET YELLOW "  Phone       " RESET ": %-30s" CYAN "│\n" RESET, r.phone);
    printf(CYAN "  │" RESET YELLOW "  Balance     " RESET ": $%-29.2f" CYAN "│\n" RESET, r.amount);
    printf(CYAN "  │" RESET YELLOW "  Type        " RESET ": %-30s" CYAN "│\n" RESET, r.accountType);
    printf(CYAN "  └──────────────────────────────────────────────┘\n\n" RESET);

    double interest;
    if (strcmp(r.accountType, "saving") == 0)
    {
        interest = r.amount * 0.07 / 12;
        printf(GREEN "  💰 You will get $%.2f as interest on day %d of every month\n" RESET,
               interest, r.deposit.day);
    }
    else if (strcmp(r.accountType, "fixed01") == 0)
    {
        interest = r.amount * 0.04;
        printf(GREEN "  💰 You will get $%.2f as interest on %02d/%02d/%04d\n" RESET,
               interest, r.deposit.month, r.deposit.day, r.deposit.year + 1);
    }
    else if (strcmp(r.accountType, "fixed02") == 0)
    {
        interest = r.amount * 0.10;
        printf(GREEN "  💰 You will get $%.2f as interest on %02d/%02d/%04d\n" RESET,
               interest, r.deposit.month, r.deposit.day, r.deposit.year + 2);
    }
    else if (strcmp(r.accountType, "fixed03") == 0)
    {
        interest = r.amount * 0.24;
        printf(GREEN "  💰 You will get $%.2f as interest on %02d/%02d/%04d\n" RESET,
               interest, r.deposit.month, r.deposit.day, r.deposit.year + 3);
    }
    else
    {
        printf(YELLOW "  ℹ You will not get interest — account type is current.\n" RESET);
    }

    success(u);
}

void checkAllAccounts(struct User u)
{
    struct Record r;

    FILE *pf = fopen(RECORDS, "r");

    system("clear");
    int cardCount = 0;
    printf("\n" BCYAN "  Accounts owned by %s\n" RESET, u.name);
    printf(CYAN "  ══════════════════════════════════════════════\n\n" RESET);
    while (getAccountFromFile(pf, &r))
    {
        if (strcmp(r.name, u.name) == 0)
        {
            cardCount++;
            char dateStr[20];
            snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d",
                     r.deposit.day, r.deposit.month, r.deposit.year);

            printf(CYAN "  ┌──────────────────────────────────────────────┐\n" RESET);
            printf(CYAN "  │" RESET YELLOW "  Account No  " RESET ": %-30s" CYAN "│\n" RESET, r.accountNbr);
            printf(CYAN "  │" RESET YELLOW "  Opened on   " RESET ": %-30s" CYAN "│\n" RESET, dateStr);
            printf(CYAN "  │" RESET YELLOW "  Country     " RESET ": %-30s" CYAN "│\n" RESET, r.country);
            printf(CYAN "  │" RESET YELLOW "  Phone       " RESET ": %-30s" CYAN "│\n" RESET, r.phone);
            printf(CYAN "  │" RESET YELLOW "  Balance     " RESET ": $%-29.2f" CYAN "│\n" RESET, r.amount);
            printf(CYAN "  │" RESET YELLOW "  Type        " RESET ": %-30s" CYAN "│\n" RESET, r.accountType);
            printf(CYAN "  └──────────────────────────────────────────────┘\n\n" RESET);
        }
    }
    if (cardCount == 0)
        printf(YELLOW "  No accounts found.\n" RESET);
    fclose(pf);
    success(u);
}
