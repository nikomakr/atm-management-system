#include <termios.h> // for terminal control (hiding password input)
#include "header.h"

char *USERS = "./data/users.txt";

void loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;

    system("clear");
    printf("\n" BCYAN "  ATM Management System — Login\n" RESET);
    printf(CYAN "  ──────────────────────────────\n" RESET);
    printf(YELLOW "  Username: " RESET);
    scanf("%s", a);

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
    printf(YELLOW "  Password: " RESET);
    scanf("%s", pass);

    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
};

void loadUser(struct User *u)
{
    FILE *fp = fopen(USERS, "r");
    if (fp == NULL)
        return;

    struct User tmp;
    while (fscanf(fp, "%d %s %s", &tmp.id, tmp.name, tmp.password) != EOF)
    {
        if (strcmp(tmp.name, u->name) == 0)
        {
            u->id = tmp.id;
            fclose(fp);
            return;
        }
    }
    fclose(fp);
}

const char *getPassword(struct User u)
{
    FILE *fp;
    struct User userChecker;

    if ((fp = fopen("./data/users.txt", "r")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    while (fscanf(fp, "%d %s %s", &userChecker.id, userChecker.name, userChecker.password) != EOF)
    {
        if (strcmp(userChecker.name, u.name) == 0)
        {
            fclose(fp);
            char *buff = userChecker.password;
            return buff;
        }
    }

    fclose(fp);
    return "no user found";
}

void registerMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;
    struct User u;
    FILE *fp;
    int maxId = -1;

    system("clear");
    printf("\n" BCYAN "  ATM Management System — Register\n" RESET);
    printf(CYAN "  ──────────────────────────────────\n" RESET);
    printf(YELLOW "  Choose a username: " RESET);
    scanf("%s", a);

    // check for duplicate username
    if ((fp = fopen(USERS, "r")) != NULL)
    {
        while (fscanf(fp, "%d %s %s", &u.id, u.name, u.password) != EOF)
        {
            if (strcmp(u.name, a) == 0)
            {
                fclose(fp);
                printf(BRED "\n  ✖ Username already exists. Please choose another.\n" RESET);
                exit(1);
            }
            if (u.id > maxId)
                maxId = u.id;
        }
        fclose(fp);
    }

    // hidden password input
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        exit(1);
    }
    printf(YELLOW "  Choose a password: " RESET);
    scanf("%s", pass);

    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        exit(1);
    }

    // save new user
    if ((fp = fopen(USERS, "a")) == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(fp, "%03d %s %s\n", maxId + 1, a, pass);
    fclose(fp);

    printf(BGREEN "\n  ✔ Account created successfully! Welcome, %s.\n" RESET, a);
}