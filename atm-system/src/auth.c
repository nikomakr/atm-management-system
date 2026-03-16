#include <termios.h>
#include "header.h"

char *USERS = "./data/users.txt";

void loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
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
    printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
    scanf("%s", pass);

    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
};

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
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t Register:");
    scanf("%s", a);

    // check for duplicate username
    if ((fp = fopen(USERS, "r")) != NULL)
    {
        while (fscanf(fp, "%d %s %s", &u.id, u.name, u.password) != EOF)
        {
            if (strcmp(u.name, a) == 0)
            {
                fclose(fp);
                printf("\n\t\t\tUsername already exists. Please choose another.\n");
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
    printf("\n\n\n\n\n\t\t\t\tEnter a password:");
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
    fprintf(fp, "%d %s %s\n", maxId + 1, a, pass);
    fclose(fp);

    printf("\n\t\t\tAccount created successfully! Welcome, %s.\n", a);
}