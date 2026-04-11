# Functional

**Note: Confirm the data storage method used in the project (e.g., text files or a relational database like SQLite).**

**SQLite relational database.** All data is stored in `./data/atm.db`, which is created and seeded automatically on first launch. Three tables are used:
- `users` — registered user credentials (passwords stored as SHA-256 hashes)
- `records` — all bank account records
- `transactions` — log of every deposit and withdrawal

You can inspect the database at any time by passing a query directly — it runs and returns to your shell immediately:
```bash
sqlite3 atm-system/data/atm.db "SELECT name FROM users;"
```
All verification commands throughout this file follow that same pattern. If you open the interactive shell by mistake (`sqlite3 atm-system/data/atm.db` with no query), type `.quit` to exit.

> **Setup note:** Alice's four accounts (`834213`, `320421`, `3214`, `3212`) and users Alice, Michel, Bob, Jade, and Niko are pre-seeded into the database on first run via `INSERT OR IGNORE`. No manual setup is required.

> **Working directory:** Two different directories are used:
> - `./atm` and `make` commands must be run from inside `atm-system/`
> - `sqlite3 atm-system/data/atm.db "..."` commands must be run from the `atm-management-system/` root
>
> To avoid confusion, open two terminal tabs — one in each directory.

---

**Open the application and register a new user with the name "Marcus" and the password "q1w2e3r4t5y6".**
## Is this user saved in the data storage (text file "./data/users.txt" or database), and if so, are all credentials correct (name and password)?

**Yes.**

How to test:
1. Run `./atm` → choose `[2]- Register`
2. Enter username: `Marcus`, password: `q1w2e3r4t5y6`
3. Verify is in the database by opening another terminal and run the command below:
```bash
sqlite3 atm-system/data/atm.db "SELECT id, name, password FROM users WHERE name = 'Marcus';"
```
Expected output:
```
5|Marcus|40f598cc76550d29043b4ce234a5e4a0f2e5951a113baec2831f91d520f3db4a 
```
The ID is auto-incremented by SQLite, the username is saved correctly, and the password is stored as its SHA-256 hash [bonus feature], so if you will see the pasword slightly different is due to encryption. Also, the number 5 due to the autoincrement, that can be different later on subject to next available id number.

---

**Open the application and re-register the user "Alice".**
## Did the application display an error message stating that this user already exists?

**Yes.**

How to test:
1. Run `./atm` → choose `[2]- Register`
2. Enter username: `Alice`
3. The application immediately prints:
```
✖ Username already exists. Please choose another.
```
and exits. The `users` table is not modified (enforced by a `SELECT COUNT(*)` check before any `INSERT`).

---

**Open the data storage (text file "./data/users.txt" or database).**
## Are all the user names unique? (ex: no repetition on the name Alice)

**Yes.**

How to verify:
```bash
sqlite3 atm-system/data/atm.db "SELECT name FROM users;"
```
Every row has a distinct username. The `name` column is defined as `TEXT UNIQUE NOT NULL`, so the database itself enforces uniqueness in addition to the application-level duplicate check in `registerMenu()`.

---

**Try and login as "Alice".**
## Was Alice able to enter the main menu?

**Yes.**

How to test:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
3. Terminal prints `✔ Password match! Logging in...` and the main menu appears
4. Behind the scenes, the entered password is SHA-256 hashed and compared against the stored digest; `loadUser()` queries the `users` table to correctly set Alice's internal ID (`0`)

---

**Try to create two accounts with the user Alice, then select the option "Update information of account" and select an account number that does not exist for Alice.**
## Did the application display some kind of error message stating that this account does not exist?

**Yes.**

How to test:
1. Log in as Alice → `[2]- Update account information`
2. Enter a number that does not exist for Alice (e.g. `000000000000`)
3. The application prints:
```
✖ Record not found!
Enter 0 to try again, 1 for main menu, 2 to exit:
```

---

**Resorting to the user Alice, try and select the option "Update information of account" and select one of the accounts you created for Alice.**
## Did the application prompt a choice of updating the phone number or the country?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[2]- Update account information`
3. Enter a valid account number (e.g. `834213`)
4. The application presents:
```
What would you like to update?
[1]- Phone number
[2]- Country
Enter your choice:
```

---

**Resorting to the user Alice, try and select the option "Update information of account" and select one of the accounts you created for Alice. Then update the phone number of that account.**
## Was the phone number of that account updated in the application and the data storage (text file "records.txt" or database)?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[2]- Update account information` → enter `834213`
3. Choose `[1]- Phone number` → enter `004491234567`
4. Verify in the database:
```bash
sqlite3 atm-system/data/atm.db "SELECT phone FROM records WHERE account_nbr = '834213';"
```
Expected output: `004491234567`

> The phone field accepts up to 15 digits, preserves leading zeros (e.g. country prefix `0044`), and rejects any non-digit characters. [bonus feature]

---

**Resorting to the user Alice, try and select the option "Update information of account" and select one of the accounts you created for Alice. Then update the country of that account.**
## Was the country of that account updated in the application and the data storage (text file "records.txt" or database)?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[2]- Update account information` → enter `834213`
3. Choose `[2]- Country` → enter `Germany`
4. Verify in the database:
```bash
sqlite3 atm-system/data/atm.db "SELECT country FROM records WHERE account_nbr = '834213';"
```
Expected output: `Germany`

---

**Resorting to the user Alice, try to create a new account with: date "10/10/2012" account number "834213", country "UK", phone number "291231392", deposit amount $"1001.20", type of account "saving". Then select "Check accounts" choose the account you just created.**
## Did the application display the account information and the gain of $5.84 of interest on day 10 of every month?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[3]- Check accounts` → enter `834213`
3. The application displays:
```
Account No   : 834213
...
💰 You will get $5.84 as interest on day 10 of every month
```
Interest formula: `1001.20 × 0.07 / 12 = $5.84`

> Account `834213` is pre-seeded in the database. To test the creation flow, first delete it:
> ```bash
> sqlite3 atm-system/data/atm.db "DELETE FROM records WHERE account_nbr = '834213';"
> ```
> Then use `[1]- Create a new account` with the values above.

---

**Resorting to the user Alice create again an account but with account number "320421" and type of account "fixed01" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $40.05 of interest on 10/10/2013?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[3]- Check accounts` → enter `320421`
3. The application displays:
```
💰 You will get $40.05 as interest on 10/10/2013
```
Interest formula: `1001.20 × 0.04 = $40.05`

> Account `320421` (fixed01, 1-year term) is pre-seeded in the database.

---

**Resorting to the user Alice create again an account but with account number "3214" and type of account "fixed02" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $100.12 of interest on 10/10/2014?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[3]- Check accounts` → enter `3214`
3. The application displays:
```
💰 You will get $100.12 as interest on 10/10/2014
```
Interest formula: `1001.20 × 0.10 = $100.12`

> Account `3214` (fixed02, 2-year term) is pre-seeded in the database.

---

**Resorting to the user Alice create again an account but with account number "3212" and type of account "fixed03" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $240.29 of interest on 10/10/2015?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[3]- Check accounts` → enter `3212`
3. The application displays:
```
💰 You will get $240.29 as interest on 10/10/2015
```
Interest formula: `1001.20 × 0.24 = $240.29`

> Account `3212` (fixed03, 3-year term) is pre-seeded in the database.

---

**Resorting to the user Alice select the option "Make transaction". Then choose the account with the id "3212"**
## Was an error message displayed stating it is not possible to withdraw or deposit for "fixed" accounts?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[5]- Make Transaction`
3. Enter account number: `3212` (fixed03 account)
4. The application immediately prints:
```
✖ Transactions are not allowed for fixed accounts.
```

---

**Resorting to the user Alice select the option "Make transaction", choose the account with the id "834213". Then try to withdraw money.**
## Are you able to withdraw money?
## And if so, was the withdrawal updated in the data storage (text file "records.txt" or database)?
## Does the system forbid to withdraw an amount superior to your available balance?

**Yes to all three.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[5]- Make Transaction` → enter `834213`
3. Current balance is shown (e.g. `$1001.20`)
4. Choose `[2]- Withdraw` → enter `200`
5. Application shows: `New balance: $801.20` → `✔ Success!`
6. Verify the balance update:
```bash
sqlite3 atm-system/data/atm.db "SELECT amount FROM records WHERE account_nbr = '834213';"
```
Expected output: `801.2`

7. Verify the transaction log:
```bash
sqlite3 atm-system/data/atm.db "SELECT * FROM transactions;"
```
Expected row: `1|0|Alice|834213|withdraw|200.0|801.2|10|10|2012`

To test the balance guard: choose `[2]- Withdraw` → enter an amount larger than the balance (e.g. `99999`)
```
✖ Insufficient balance. Available: $801.20
```

---

**Try to deposit money into the account "834213".**
## Were you able to deposit money into this account?
## And if so did it update the data storage (text file "records.txt" or database)?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[5]- Make Transaction` → enter `834213`
3. Choose `[1]- Deposit` → enter `500`
4. Application shows: `New balance: $1501.20` → `✔ Success!`
5. Verify:
```bash
sqlite3 atm-system/data/atm.db "SELECT amount FROM records WHERE account_nbr = '834213';"
sqlite3 atm-system/data/atm.db "SELECT type, amount, new_balance FROM transactions ORDER BY id DESC LIMIT 1;"
```

---

**Resorting to the user Alice try to select the option "Remove existing account" and remove the accounts "834213", "320421" and "3214".**
## Can you confirm that those account were deleted, both in the application and data storage (text file "records.txt" or database)?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[6]- Remove existing account` → enter `834213` → `✔ Success!`
3. Repeat for `320421` and `3214`
4. Verify the records are gone:
```bash
sqlite3 atm-system/data/atm.db "SELECT account_nbr FROM records WHERE name = 'Alice';"
```
Expected output: only `3212` remains.

---

**Resorting to the user Alice select the option "Remove existing account" and try to remove an account that does not exist.**
## Did the application prompt some type of error saying that the account does not exist?

**Yes.**

How to test:
1. Run `./atm` → login as Alice
2. Choose `[6]- Remove existing account`
3. Enter a number that does not exist (e.g. `000000000000`)
4. The application prints:
```
✖ Record not found!
Enter 0 to try again, 1 for main menu, 2 to exit:
```

---

**Create another user named "Michel". Then by using Alice select the option "transfer owner" and try to transfer ownership of the account "3212" to Michel.**
## Were you able to transfer the ownership of this account to Michel? And if so did it update both application and data storage (text file "records.txt" or database)?

**Yes.**

How to test:
1. Michel is pre-seeded in the database; no registration step needed
2. Run `./atm` → login as Alice
3. Choose `[7]- Transfer ownership`
4. Enter account number: `3212`
5. Enter target username: `Michel` *(case-sensitive)*
6. Application confirms `✔ Success!`
7. Verify the ownership change:
```bash
sqlite3 atm-system/data/atm.db "SELECT user_id, name FROM records WHERE account_nbr = '3212';"
```
Expected output: `1|Michel`

---

# Bonus

**Open two terminals and login with two different users. Then transfer ownership of an account to the other user.**
## +Was the user who received the account notified instantly?

**Yes.**

How to test:
1. Terminal 1: `./atm` → log in as `Nick` with password `123!`
2. Terminal 2: `./atm` → log in as `Tom` with password `123!`
3. In Terminal 2 (Tom): `[7]- Transfer ownership` → enter a valid account number → target: `Nick`
4. The moment Tom confirms, Terminal 1 (Nick) instantly shows:
```
========================================
         *** NOTIFICATION ***
  'Tom' transferred account #32012 to you!
========================================
```

This works via a named FIFO at `/tmp/atm_notify_Nick` and a forked child process that blocks on `read()`. You can verify the FIFO exists while Bob is logged in:
```bash
ls /tmp/atm_notify_*
```

## +Did the student update the terminal interface?

**Yes.**

All menus are rendered using ANSI colour codes and Unicode box-drawing characters. Headers are displayed in bold cyan inside bordered panels, prompts in yellow, success messages in bold green, and error messages in bold red. The main menu greets the logged-in user by name in a centred header.

## +Is the password saved in the data storage (text file "users.txt" or database) encrypted?

**Yes.**

Passwords are hashed with SHA-256 using macOS CommonCrypto before being stored. The 64-character hex digest is saved in the `users` table; the plain-text password is never written to disk. On login, the entered password is hashed and compared against the stored digest.

Verify:
```bash
sqlite3 atm-system/data/atm.db "SELECT name, password FROM users WHERE name = 'Alice';"
```
Expected output:
```
Alice|40f598cc76550d29043b4ce234a5e4a0f2e5951a113baec2831f91d520f3db4a
```

## +Did the student create a relational database?

**Yes.**

Storage uses SQLite (`data/atm.db`), created and seeded automatically at startup. Three tables are defined with proper types, `UNIQUE` and `NOT NULL` constraints, and a foreign key from `records.user_id` to `users.id`.

Schema overview:
```bash
sqlite3 atm-system/data/atm.db ".schema"
```

## +Did the student make their own Makefile?

**Yes.**

A `Makefile` is present that compiles `main.c`, `auth.c`, `system.c`, `notification.c`, and `db.c` into separate object files and links them into the `atm` binary. It defines `CC`, `CFLAGS` (`-Wall -Wextra -std=c11`), and `LDFLAGS` (`-lsqlite3` plus CommonCrypto frameworks). A pattern rule ensures any source file recompiles when `header.h` changes. A `run` target builds and launches the binary; `clean` removes all objects and the binary.

To build:
```bash
make clean && make
```
To build and run in one step:
```bash
make run
```

## +Did the student add more features to the project?

**Yes.** The following were added beyond the base requirements:

1. **Real-time transfer notifications** — named pipes (FIFOs) + `fork()`. The recipient is instantly alerted in their terminal when an account is transferred to them, even mid-session.
2. **Transaction log** — every deposit and withdrawal is recorded in the `transactions` table with the user ID, username, account number, type, amount, new balance, and date.
3. **Password encryption** — SHA-256 hashing via CommonCrypto; plain-text passwords never touch disk.
4. **Styled TUI** — ANSI colour codes and Unicode box-drawing characters throughout all menus.
5. **SQLite relational database** — full data layer backed by SQLite, replacing plain text files.

## +Did the student optimize the code already given?

**Yes.** The following optimisations and fixes were applied:

1. **`u.id` garbage value bug** — `struct User u` was uninitialised at declaration. Added `= {0}` at declaration and introduced `loadUser()` to correctly populate `u.id` from the `users` table after every login or registration.
2. **Record ID uniqueness** — IDs are now managed by SQLite `AUTOINCREMENT`, guaranteeing uniqueness even after deletions.
3. **Phone number stored as string** — changed from `long` (which stripped leading zeros and had no format check) to `TEXT` in the database / `char[16]` in the struct. Accepts up to 15 digits, preserves leading zeros (e.g. country prefix `0044...`), and rejects non-digit characters.
4. **User ID zero-padded** — stored as a 3-digit zero-padded integer (`000`, `001`, `002`…) in display contexts; the integer primary key in the database is `0`, `1`, `2`…
