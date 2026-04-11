#include "header.h"

sqlite3 *db = NULL;

static void initSchema(void)
{
    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "  id       INTEGER PRIMARY KEY,"
        "  name     TEXT UNIQUE NOT NULL,"
        "  password TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS records ("
        "  id            INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id       INTEGER NOT NULL,"
        "  name          TEXT NOT NULL,"
        "  account_nbr   TEXT UNIQUE NOT NULL,"
        "  deposit_month INTEGER NOT NULL,"
        "  deposit_day   INTEGER NOT NULL,"
        "  deposit_year  INTEGER NOT NULL,"
        "  country       TEXT NOT NULL,"
        "  phone         TEXT NOT NULL,"
        "  amount        REAL NOT NULL,"
        "  account_type  TEXT NOT NULL,"
        "  FOREIGN KEY(user_id) REFERENCES users(id)"
        ");"
        "CREATE TABLE IF NOT EXISTS transactions ("
        "  id          INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id     INTEGER NOT NULL,"
        "  username    TEXT NOT NULL,"
        "  account_nbr TEXT NOT NULL,"
        "  type        TEXT NOT NULL,"
        "  amount      REAL NOT NULL,"
        "  new_balance REAL NOT NULL,"
        "  t_month     INTEGER NOT NULL,"
        "  t_day       INTEGER NOT NULL,"
        "  t_year      INTEGER NOT NULL"
        ");";

    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK)
    {
        fprintf(stderr, "Schema error: %s\n", err);
        sqlite3_free(err);
        exit(1);
    }
}

static void seedData(void)
{
    // SHA-256 hashes:
    //   q1w2e3r4t5y6          → 40f598cc...
    //   123456789qwertyuio!   → e7bc988c...
    //   123Sa*                → bbb7da5e...
    //   123!                  → 58cc4805...
    const char *sql =
        "INSERT OR IGNORE INTO users (id, name, password) VALUES"
        " (0, 'Alice',  '40f598cc76550d29043b4ce234a5e4a0f2e5951a113baec2831f91d520f3db4a'),"
        " (1, 'Michel', '40f598cc76550d29043b4ce234a5e4a0f2e5951a113baec2831f91d520f3db4a'),"
        " (2, 'Bob',    'e7bc988cbec9752c8cb1017cc44abb9b2fb031a9e15a89afd9f3c3b8b697ff0f'),"
        " (3, 'Jade',   'bbb7da5e3bae6a646cc40d64871d4d671e69bfda81d456ca1550ae62462ad36c'),"
        " (4, 'Niko',   '58cc480580f302b31ac8c42d470df5c3cc7abec35d99098288a5a3ac3b56a449');"

        "INSERT OR IGNORE INTO records"
        " (id, user_id, name, account_nbr, deposit_month, deposit_day, deposit_year,"
        "  country, phone, amount, account_type) VALUES"
        " (0, 0, 'Alice', '834213', 10, 10, 2012, 'UK', '291231392', 1001.20, 'saving'),"
        " (1, 0, 'Alice', '320421', 10, 10, 2012, 'UK', '291231392', 1001.20, 'fixed01'),"
        " (2, 0, 'Alice', '3214',   10, 10, 2012, 'UK', '291231392', 1001.20, 'fixed02'),"
        " (3, 0, 'Alice', '3212',   10, 10, 2012, 'UK', '291231392', 1001.20, 'fixed03');";

    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK)
    {
        fprintf(stderr, "Seed error: %s\n", err);
        sqlite3_free(err);
        exit(1);
    }
}

void openDB(void)
{
    if (sqlite3_open("./data/atm.db", &db) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    initSchema();
    seedData();
}

void closeDB(void)
{
    if (db)
        sqlite3_close(db);
}
