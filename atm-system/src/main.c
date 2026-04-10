#include "header.h"

// Print a centered line inside the box (inner width = 46)
static void printBoxCentered(const char *text)
{
    int len = (int)strlen(text);
    int pad_left  = (46 - len) / 2;
    int pad_right = 46 - len - pad_left;
    printf(CYAN "  ║" RESET BCYAN "%*s%s%*s" RESET CYAN "║\n" RESET,
           pad_left, "", text, pad_right, "");
}

// Print a left-aligned row inside the box, padded to inner width 46
#define BOX_ITEM(str) \
    printf(CYAN "  ║" RESET "%-46s" CYAN "║\n" RESET, str)

void mainMenu(struct User u)
{
    int option;
    system("clear");

    char title[60];
    snprintf(title, sizeof(title), "Welcome back, %s!", u.name);

    printf("\n");
    printf(BOX_TOP);
    printBoxCentered(title);
    printf(BOX_DIV);
    BOX_ITEM("");
    BOX_ITEM("   [1]  Create a new account");
    BOX_ITEM("   [2]  Update account information");
    BOX_ITEM("   [3]  Check accounts");
    BOX_ITEM("   [4]  List all owned accounts");
    BOX_ITEM("   [5]  Make Transaction");
    BOX_ITEM("   [6]  Remove existing account");
    BOX_ITEM("   [7]  Transfer ownership");
    BOX_ITEM("   [8]  Exit");
    BOX_ITEM("");
    printf(BOX_BOT);
    printf(YELLOW "  ▷ Enter your choice: " RESET);
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        createNewAcc(u);
        break;
    case 2:
        updateAccount(u);
        break;
    case 3:
        checkAccount(u);
        break;
    case 4:
        checkAllAccounts(u);
        break;
    case 5:
        makeTransaction(u);
        break;
    case 6:
        removeAccount(u);
        break;
    case 7:
        transferOwnership(u);
        break;
    case 8:
        exit(1);
        break;
    default:
        printf(RED "  Invalid option. Please try again.\n" RESET);
        mainMenu(u);
    }
}

void initMenu(struct User *u)
{
    int r = 0;
    int option;
    system("clear");

    printf("\n");
    printf(BOX_TOP);
    printBoxCentered("ATM Management System");
    printf(BOX_DIV);
    BOX_ITEM("");
    BOX_ITEM("   [1]  Login");
    BOX_ITEM("   [2]  Register");
    BOX_ITEM("   [3]  Exit");
    BOX_ITEM("");
    printf(BOX_BOT);

    while (!r)
    {
        printf(YELLOW "  ▷ Enter your choice: " RESET);
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            loginMenu(u->name, u->password);
            if (strcmp(u->password, getPassword(*u)) == 0)
            {
                printf(BGREEN "\n  ✔ Password match! Logging in...\n" RESET);
            }
            else
            {
                printf(BRED "\n  ✖ Wrong username or password.\n" RESET);
                exit(1);
            }
            r = 1;
            break;
        case 2:
            registerMenu(u->name, u->password);
            r = 1;
            break;
        case 3:
            exit(1);
            break;
        default:
            printf(RED "  Invalid option. Please try again.\n" RESET);
        }
    }
}

int main()
{
    struct User u = {0};

    initMenu(&u);
    loadUser(&u);
    startNotificationListener(u);
    mainMenu(u);
    return 0;
}
