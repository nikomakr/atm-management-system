#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ── ANSI color codes ───────────────────────────────────────────────────────
#define RESET    "\033[0m"
#define BOLD     "\033[1m"
#define RED      "\033[0;31m"
#define GREEN    "\033[0;32m"
#define YELLOW   "\033[0;33m"
#define CYAN     "\033[0;36m"
#define BRED     "\033[1;31m"
#define BGREEN   "\033[1;32m"
#define BYELLOW  "\033[1;33m"
#define BCYAN    "\033[1;36m"

// ── Box drawing ────────────────────────────────────────────────────────────
// Inner content width = 46 characters
#define BOX_LINE "══════════════════════════════════════════════"
#define BOX_TOP  CYAN "  ╔" BOX_LINE "╗\n" RESET
#define BOX_DIV  CYAN "  ╠" BOX_LINE "╣\n" RESET
#define BOX_BOT  CYAN "  ╚" BOX_LINE "╝\n" RESET
#define BOX_ROW(content) CYAN "  ║" RESET content CYAN "║\n" RESET

struct Date
{
    int month, day, year;
};

// all fields for each record of an account
struct Record
{
    int id;
    int userId;
    char name[100];
    char country[100];
    char phone[16];      // up to 15 digits + null (includes country prefix)
    char accountType[10];
    char accountNbr[13]; // exactly 12 digits + null
    double amount;
    struct Date deposit;
    struct Date withdraw;
};

struct User
{
    int id;
    char name[50];
    char password[50];
};

struct Transaction
{
    int id;
    int userId;
    char username[50];
    char accountNbr[13];
    char type[10];   // "deposit" or "withdraw"
    double amount;
    double newBalance;
    struct Date date;
};

// authentication functions
void loginMenu(char a[50], char pass[50]);
void registerMenu(char a[50], char pass[50]);
const char *getPassword(struct User u);
void loadUser(struct User *u);

// notification functions
void startNotificationListener(struct User u);
void sendNotification(const char *toUser, const char *fromUser, const char *accountNbr);

// system function
void createNewAcc(struct User u);
void makeTransaction(struct User u);
void mainMenu(struct User u);
void checkAccount(struct User u);
void checkAllAccounts(struct User u);
void updateAccount(struct User u);
void removeAccount(struct User u);
void transferOwnership(struct User u);
