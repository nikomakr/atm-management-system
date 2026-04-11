#include "header.h"
#include <ctype.h>

// ── Validation helpers ─────────────────────────────────────────────────────

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

static int isValidAccountNbr(const char *s)
{
    int len = (int)strlen(s);
    if (len < 1 || len > 20)
        return 0;
    for (int i = 0; i < len; i++)
        if (!isdigit((unsigned char)s[i]))
            return 0;
    return 1;
}

// ── SQLite helpers ─────────────────────────────────────────────────────────

#define RECORD_FIELDS \
    "id, user_id, name, account_nbr, deposit_month, deposit_day, deposit_year," \
    " country, phone, amount, account_type"

static void rowToRecord(sqlite3_stmt *stmt, struct Record *r)
{
    memset(r, 0, sizeof(*r));
    r->id            = sqlite3_column_int(stmt, 0);
    r->userId        = sqlite3_column_int(stmt, 1);
    const char *col;
    col = (const char *)sqlite3_column_text(stmt, 2);
    if (col) strncpy(r->name,        col, sizeof(r->name)        - 1);
    col = (const char *)sqlite3_column_text(stmt, 3);
    if (col) strncpy(r->accountNbr,  col, sizeof(r->accountNbr)  - 1);
    r->deposit.month = sqlite3_column_int(stmt, 4);
    r->deposit.day   = sqlite3_column_int(stmt, 5);
    r->deposit.year  = sqlite3_column_int(stmt, 6);
    col = (const char *)sqlite3_column_text(stmt, 7);
    if (col) strncpy(r->country,     col, sizeof(r->country)     - 1);
    col = (const char *)sqlite3_column_text(stmt, 8);
    if (col) strncpy(r->phone,       col, sizeof(r->phone)       - 1);
    r->amount        = sqlite3_column_double(stmt, 9);
    col = (const char *)sqlite3_column_text(stmt, 10);
    if (col) strncpy(r->accountType, col, sizeof(r->accountType) - 1);
}

static void saveTransaction(struct User u, const char *accountNbr, const char *type,
                            double amount, double newBalance, struct Date date)
{
    const char *sql =
        "INSERT INTO transactions"
        " (user_id, username, account_nbr, type, amount, new_balance, t_month, t_day, t_year)"
        " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt,    1, u.id);
    sqlite3_bind_text(stmt,   2, u.name,      -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,   3, accountNbr,  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,   4, type,         -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, amount);
    sqlite3_bind_double(stmt, 6, newBalance);
    sqlite3_bind_int(stmt,    7, date.month);
    sqlite3_bind_int(stmt,    8, date.day);
    sqlite3_bind_int(stmt,    9, date.year);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
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
        mainMenu(u);
    else if (option == 0)
        exit(1);
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
    memset(&r, 0, sizeof(r));

    system("clear");
    printf("\n" BCYAN "  ===== New Account =====\n\n" RESET);
    printf(YELLOW "  Enter today's date (mm/dd/yyyy): " RESET);
    scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);

    char inputBuf[50];

    // Account number: digits only, globally unique
    while (1)
    {
        printf("\nEnter the account number: ");
        scanf("%s", inputBuf);

        if (!isValidAccountNbr(inputBuf))
        {
            printf("✖ Account number must contain digits only (max 20).\n");
            continue;
        }

        sqlite3_stmt *chk;
        sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM records WHERE account_nbr = ?", -1, &chk, NULL);
        sqlite3_bind_text(chk, 1, inputBuf, -1, SQLITE_STATIC);
        sqlite3_step(chk);
        int exists = sqlite3_column_int(chk, 0);
        sqlite3_finalize(chk);

        if (exists)
        {
            printf("✖ This account number already exists. Choose another.\n");
            continue;
        }
        strncpy(r.accountNbr, inputBuf, sizeof(r.accountNbr) - 1);
        break;
    }

    printf("\nEnter the country: ");
    scanf("%s", r.country);

    // Phone: digits only, max 15
    while (1)
    {
        printf("\nEnter the phone number (digits only, up to 15, include country prefix): ");
        scanf("%s", inputBuf);
        if (!isValidPhone(inputBuf))
        {
            printf("✖ Phone must contain digits only and be at most 15 digits.\n");
            continue;
        }
        strncpy(r.phone, inputBuf, sizeof(r.phone) - 1);
        break;
    }

    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n"
           "\t-> saving\n\t-> current\n"
           "\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n"
           "\n\tEnter your choice: ");
    scanf("%s", r.accountType);

    const char *sql =
        "INSERT INTO records"
        " (user_id, name, account_nbr, deposit_month, deposit_day, deposit_year,"
        "  country, phone, amount, account_type)"
        " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt,    1, u.id);
    sqlite3_bind_text(stmt,   2, u.name,          -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,   3, r.accountNbr,    -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt,    4, r.deposit.month);
    sqlite3_bind_int(stmt,    5, r.deposit.day);
    sqlite3_bind_int(stmt,    6, r.deposit.year);
    sqlite3_bind_text(stmt,   7, r.country,        -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,   8, r.phone,           -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 9, r.amount);
    sqlite3_bind_text(stmt,  10, r.accountType,    -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
        fprintf(stderr, "Insert error: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);

    success(u);
}

void updateAccount(struct User u)
{
    char accountNbr[50];
    system("clear");
    printf("\n" BCYAN "  ===== Update Account Information =====\n\n" RESET);
    printf(YELLOW "  Enter the account number: " RESET);
    scanf("%s", accountNbr);

    // Find account belonging to this user
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT " RECORD_FIELDS " FROM records WHERE name = ? AND account_nbr = ?",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, u.name,      -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, accountNbr,  -1, SQLITE_STATIC);

    struct Record r;
    int found = (sqlite3_step(stmt) == SQLITE_ROW);
    if (found) rowToRecord(stmt, &r);
    sqlite3_finalize(stmt);

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
            break;
        }
        sqlite3_prepare_v2(db,
            "UPDATE records SET phone = ? WHERE account_nbr = ? AND name = ?",
            -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, inputBuf,    -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, accountNbr,  -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, u.name,      -1, SQLITE_STATIC);
    }
    else if (choice == 2)
    {
        printf("Enter new country: ");
        scanf("%s", inputBuf);
        sqlite3_prepare_v2(db,
            "UPDATE records SET country = ? WHERE account_nbr = ? AND name = ?",
            -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, inputBuf,    -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, accountNbr,  -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, u.name,      -1, SQLITE_STATIC);
    }
    else
    {
        printf("Invalid choice!\n");
        stayOrReturn(1, updateAccount, u);
        return;
    }

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    success(u);
}

void checkAccount(struct User u)
{
    char accountNbr[50];
    system("clear");
    printf("\n" BCYAN "  ===== Check Account Details =====\n\n" RESET);
    printf(YELLOW "  Enter the account number: " RESET);
    scanf("%s", accountNbr);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT " RECORD_FIELDS " FROM records WHERE name = ? AND account_nbr = ?",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, u.name,     -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, accountNbr, -1, SQLITE_STATIC);

    struct Record r;
    int found = (sqlite3_step(stmt) == SQLITE_ROW);
    if (found) rowToRecord(stmt, &r);
    sqlite3_finalize(stmt);

    if (!found)
    {
        stayOrReturn(0, checkAccount, u);
        return;
    }

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
    system("clear");
    printf("\n" BCYAN "  Accounts owned by %s\n" RESET, u.name);
    printf(CYAN "  ══════════════════════════════════════════════\n\n" RESET);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT " RECORD_FIELDS " FROM records WHERE name = ?",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);

    int cardCount = 0;
    struct Record r;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        rowToRecord(stmt, &r);
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
    sqlite3_finalize(stmt);

    if (cardCount == 0)
        printf(YELLOW "  No accounts found.\n" RESET);

    success(u);
}

void makeTransaction(struct User u)
{
    char accountNbr[50];
    system("clear");
    printf("\n" BCYAN "  ===== Make Transaction =====\n\n" RESET);
    printf(YELLOW "  Enter the account number: " RESET);
    scanf("%s", accountNbr);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT " RECORD_FIELDS " FROM records WHERE name = ? AND account_nbr = ?",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, u.name,     -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, accountNbr, -1, SQLITE_STATIC);

    struct Record r;
    int found = (sqlite3_step(stmt) == SQLITE_ROW);
    if (found) rowToRecord(stmt, &r);
    sqlite3_finalize(stmt);

    if (!found)
    {
        stayOrReturn(0, makeTransaction, u);
        return;
    }

    if (strcmp(r.accountType, "fixed01") == 0 ||
        strcmp(r.accountType, "fixed02") == 0 ||
        strcmp(r.accountType, "fixed03") == 0)
    {
        printf("\n✖ Transactions are not allowed for fixed accounts.\n");
        stayOrReturn(1, makeTransaction, u);
        return;
    }

    printf(GREEN "\n  Current balance: $%.2f\n" RESET, r.amount);
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
        r.amount += amount;
        strncpy(transactionType, "deposit", sizeof(transactionType) - 1);
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
        if (amount > r.amount)
        {
            printf("\n✖ Insufficient balance. Available: $%.2f\n", r.amount);
            stayOrReturn(1, makeTransaction, u);
            return;
        }
        r.amount -= amount;
        strncpy(transactionType, "withdraw", sizeof(transactionType) - 1);
    }
    else
    {
        printf("Invalid choice!\n");
        stayOrReturn(1, makeTransaction, u);
        return;
    }

    printf(BGREEN "\n  New balance: $%.2f\n" RESET, r.amount);

    sqlite3_prepare_v2(db,
        "UPDATE records SET amount = ? WHERE account_nbr = ? AND name = ?",
        -1, &stmt, NULL);
    sqlite3_bind_double(stmt, 1, r.amount);
    sqlite3_bind_text(stmt,  2, accountNbr, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,  3, u.name,     -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    saveTransaction(u, accountNbr, transactionType, amount, r.amount, r.deposit);
    success(u);
}

void removeAccount(struct User u)
{
    char accountNbr[50];
    system("clear");
    printf("\n" BCYAN "  ===== Remove Account =====\n\n" RESET);
    printf(YELLOW "  Enter the account number: " RESET);
    scanf("%s", accountNbr);

    // Verify the account exists and belongs to this user
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM records WHERE name = ? AND account_nbr = ?",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, u.name,     -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, accountNbr, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int found = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (!found)
    {
        stayOrReturn(0, removeAccount, u);
        return;
    }

    sqlite3_prepare_v2(db,
        "DELETE FROM records WHERE account_nbr = ? AND name = ?",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, accountNbr, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, u.name,     -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    success(u);
}

void transferOwnership(struct User u)
{
    char accountNbr[50];
    system("clear");
    printf("\n" BCYAN "  ===== Transfer Account Ownership =====\n\n" RESET);
    printf(YELLOW "  Enter the account number to transfer: " RESET);
    scanf("%s", accountNbr);

    // Verify account belongs to current user
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM records WHERE name = ? AND account_nbr = ?",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, u.name,     -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, accountNbr, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int found = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (!found)
    {
        stayOrReturn(0, transferOwnership, u);
        return;
    }

    char targetName[50];
    printf("Enter the username to transfer ownership to (case-sensitive): ");
    scanf("%s", targetName);

    // Look up target user in the database
    struct User target = {0};
    sqlite3_prepare_v2(db,
        "SELECT id, name FROM users WHERE name = ?",
        -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, targetName, -1, SQLITE_STATIC);
    int targetFound = (sqlite3_step(stmt) == SQLITE_ROW);
    if (targetFound)
    {
        target.id = sqlite3_column_int(stmt, 0);
        strncpy(target.name,
                (const char *)sqlite3_column_text(stmt, 1),
                sizeof(target.name) - 1);
    }
    sqlite3_finalize(stmt);

    if (!targetFound)
    {
        printf("\n✖ User \"%s\" not found!\n", targetName);
        stayOrReturn(0, transferOwnership, u);
        return;
    }

    sqlite3_prepare_v2(db,
        "UPDATE records SET user_id = ?, name = ? WHERE account_nbr = ? AND name = ?",
        -1, &stmt, NULL);
    sqlite3_bind_int(stmt,  1, target.id);
    sqlite3_bind_text(stmt, 2, target.name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, accountNbr,  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, u.name,      -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sendNotification(target.name, u.name, accountNbr);
    success(u);
}
