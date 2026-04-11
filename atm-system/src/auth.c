#include <termios.h>
#include <CommonCrypto/CommonDigest.h>
#include "header.h"

// Compute SHA-256 of `input`, write 64-char hex string + null into `out` (65 bytes).
static void hashPassword(const char *input, char out[65])
{
    unsigned char digest[CC_SHA256_DIGEST_LENGTH];
    CC_SHA256(input, (CC_LONG)strlen(input), digest);
    for (int i = 0; i < CC_SHA256_DIGEST_LENGTH; i++)
        snprintf(out + i * 2, 3, "%02x", digest[i]);
    out[64] = '\0';
}

void loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;

    system("clear");
    printf("\n" BCYAN "  ATM Management System — Login\n" RESET);
    printf(CYAN "  ──────────────────────────────\n" RESET);
    printf(YELLOW "  Username: " RESET);
    scanf("%s", a);

    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) { perror("tcsetattr"); exit(1); }

    char raw[50];
    printf(YELLOW "  Password: " RESET);
    scanf("%s", raw);

    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) { perror("tcsetattr"); exit(1); }

    hashPassword(raw, pass);
}

void loadUser(struct User *u)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT id FROM users WHERE name = ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, u->name, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW)
        u->id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
}

const char *getPassword(struct User u)
{
    static char buf[65];
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT password FROM users WHERE name = ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW)
        strncpy(buf, (const char *)sqlite3_column_text(stmt, 0), 64);
    else
        strncpy(buf, "no user found", 64);
    buf[64] = '\0';
    sqlite3_finalize(stmt);
    return buf;
}

void registerMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;

    system("clear");
    printf("\n" BCYAN "  ATM Management System — Register\n" RESET);
    printf(CYAN "  ──────────────────────────────────\n" RESET);
    printf(YELLOW "  Choose a username: " RESET);
    scanf("%s", a);

    // Check for duplicate username
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM users WHERE name = ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, a, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int exists = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    if (exists)
    {
        printf(BRED "\n  ✖ Username already exists. Please choose another.\n" RESET);
        exit(1);
    }

    // Hidden password input
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) { perror("tcsetattr"); exit(1); }

    char raw[50];
    printf(YELLOW "  Choose a password: " RESET);
    scanf("%s", raw);

    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) { perror("tcsetattr"); exit(1); }

    hashPassword(raw, pass);

    // Insert new user; SQLite assigns the next available id
    sqlite3_prepare_v2(db, "INSERT INTO users (name, password) VALUES (?, ?)", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, a, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
        fprintf(stderr, "Register error: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);

    printf(BGREEN "\n  ✔ Account created successfully! Welcome, %s.\n" RESET, a);
}
