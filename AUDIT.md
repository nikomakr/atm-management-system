# Functional

**Note: Confirm the data storage method used in the project (e.g., text files or a relational database like SQLite).**

**Plain text files.** Three files are used:
- `./data/users.txt` — registered user credentials
- `./data/records.txt` — all bank account records
- `./data/transactions.txt` — log of every deposit and withdrawal [Added it as a bonus feature]

All files use whitespace-delimited fields, one record per line.

> **Testing note:** Account numbers are now validated to be **exactly 12 digits**. [Added it as a bonus feature] When the questions below reference short account numbers (e.g. `834213`), use the 12-digit equivalent instead (e.g. `000000834213`).

---

**Open the application and register a new user with the name "Marcus" and the password "q1w2e3r4t5y6".**
## Is this user saved in the data storage (text file "./data/users.txt" or database), and if so, are all credentials correct (name and password)?

**Yes.**
cd atm-system
How to test:
1. Run `./atm` → choose `[2]- Register`
2. Enter username: `Marcus`, password: `q1w2e3r4t5y6`
3. Open `data/users.txt` — a new line appears:
```
003 Marcus q1w2e3r4t5y6
```
The ID is auto-incremented (3-digit zero-padded) [Added it as a bonus feature], username and password are saved correctly.

---

**Open the application and re-register the user "Alice".**
## Did the application display an error message stating that this user already exists?

**Yes.**
cd atm-system
How to test:
1. Run `./atm` → choose `[2]- Register`
2. Enter username: `Alice`
3. The application immediately prints:
```
Username already exists. Please choose another.
```
and exits. `users.txt` is not modified.

---

**Open the data storage (text file "./data/users.txt" or database).**
## Are all the user names unique? (ex: no repetition on the name Alice)

**Yes.**

How to verify:
- Open `data/users.txt` — every line has a distinct username
- The duplicate check in `registerMenu()` scans the entire file before writing, so a second "Alice" can never be saved

---

**Try and login as "Alice".**
## Was Alice able to enter the main menu?

**Yes.**
cd atm-system
How to test:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
3. Terminal prints `Password Match!` and the main menu appears
4. Behind the scenes, `loadUser()` also reads `users.txt` to correctly set Alice's internal ID (`000`)

---

**Try to create two accounts with the user Alice, then select the option "Update information of account" and select an account number that does not exist for Alice.**
## Did the application display some kind of error message stating that this account does not exist?

**Yes.**

How to test:
1. Log in as Alice → `[1]- Create a new account` twice (e.g. account numbers `000000834213` and `000000320421`)
2. Choose `[2]- Update account information`
3. Enter a number that does not exist (e.g. `000000000000`)
4. The application prints:
```
✖ Record not found!!
Enter 0 to try again, 1 to return to main menu and 2 to exit:
```

---

**Resorting to the user Alice, try and select the option "Update information of account" and select one of the accounts you created for Alice.**
## Did the application prompt a choice of updating the phone number or the country?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[2]- Update account information`
2. Enter a valid account number (e.g. `000000834213`)
3. The application presents:
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
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[2]- Update account information` → enter `000000834213`
2. Choose `[1]- Phone number` → enter `004491234567` (digits only, max 15)
3. The application confirms success
4. Open `data/records.txt` — the phone field for that account is now `004491234567`

> The phone field accepts up to 15 digits, preserves leading zeros (e.g. country prefix `0044`), and rejects any non-digit characters. [Added it as a bonus feature]

---

**Resorting to the user Alice, try and select the option "Update information of account" and select one of the accounts you created for Alice. Then update the country of that account.**
## Was the country of that account updated in the application and the data storage (text file "records.txt" or database)?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[2]- Update account information` → enter `000000834213`
2. Choose `[2]- Country` → enter `Germany`
3. Open `data/records.txt` — the country field for that account is now `Germany`

---

**Resorting to the user Alice, try to create a new account with: date "10/10/2012" account number "834213", country "UK", phone number "291231392", deposit amount $"1001.20", type of account "saving". Then select "Check accounts" choose the account you just created.**
## Did the application display the account information and the gain of $5.84 of interest on day 10 of every month?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[1]- Create a new account`
   - Date: `10/10/2012`
   - Account number: `000000834213` *(must be 12 digits)*
   - Country: `UK`
   - Phone: `291231392`
   - Amount: `1001.20`
   - Type: `saving`
2. Choose `[3]- Check accounts` → enter `000000834213`
3. The application displays:
```
Account number: 000000834213
...
You will get $5.84 as interest on day 10 of every month
```
Interest formula: `1001.20 × 0.07 / 12 = $5.84`

---

**Resorting to the user Alice create again an account but with account number "320421" and type of account "fixed01" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $40.05 of interest on 10/10/2013?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[1]- Create a new account`
   - Date: `10/10/2012`, Account number: `000000320421`, Country: `UK`, Phone: `291231392`, Amount: `1001.20`, Type: `fixed01`
2. Choose `[3]- Check accounts` → enter `000000320421`
3. The application displays:
```
You will get $40.05 as interest on 10/10/2013
```
Interest formula: `1001.20 × 0.04 = $40.05`

---

**Resorting to the user Alice create again an account but with account number "3214" and type of account "fixed02" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $100.12 of interest on 10/10/2014?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[1]- Create a new account`
   - Account number: `000000003214`, Type: `fixed02` (rest same as above)
2. Choose `[3]- Check accounts` → enter `000000003214`
3. The application displays:
```
You will get $100.12 as interest on 10/10/2014
```
Interest formula: `1001.20 × 0.10 = $100.12`

---

**Resorting to the user Alice create again an account but with account number "3212" and type of account "fixed03" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $240.29 of interest on 10/10/2015?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[1]- Create a new account`
   - Account number: `000000003212`, Type: `fixed03` (rest same as above)
2. Choose `[3]- Check accounts` → enter `000000003212`
3. The application displays:
```
You will get $240.29 as interest on 10/10/2015
```
Interest formula: `1001.20 × 0.24 = $240.29`

---

**Resorting to the user Alice select the option "Make transaction". Then choose the account with the id "3212"**
## Was an error message displayed stating it is not possible to withdraw or deposit for "fixed" accounts?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[5]- Make Transaction`
2. Enter account number: `000000003212` (fixed03 account)
3. The application immediately prints:
```
✖ Transactions are not allowed for fixed accounts.
```

---

**Resorting to the user Alice select the option "Make transaction", choose the account with the id "834213". Then try to withdraw money.**
## Are you able to withdraw money?
## And if so, was the withdrawal updated in the data storage (text file "records.txt" or database)?
## Does the system forbid to withdraw an amount superior to your available balance?

**Yes to all three.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[5]- Make Transaction` → enter `000000834213`
2. Current balance is shown (e.g. `$1001.20`)
3. Choose `[2]- Withdraw` → enter `200`
4. Application shows: `New balance: $801.20` → `✔ Success!`
5. Open `data/records.txt` — balance for `000000834213` is now `801.20` ✓
6. Open `data/transactions.txt` — a new entry is appended:
```
0 000 Alice 000000834213 withdraw 200.00 801.20 10/10/2012
```

To test the balance guard: choose `[2]- Withdraw` → enter an amount larger than the balance (e.g. `99999`)
```
✖ Insufficient balance. Available: $801.20
```

---

**Try to deposit money into the account "834213".**
## Were you able to deposit money into this account?
## And if so did it update the data storage (text file "records.txt" or database)?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[5]- Make Transaction` → enter `000000834213`
2. Choose `[1]- Deposit` → enter `500`
3. Application shows: `New balance: $1301.20` → `✔ Success!`
4. Open `data/records.txt` — balance is updated ✓
5. Open `data/transactions.txt` — a new deposit entry is appended ✓

---

**Resorting to the user Alice try to select the option "Remove existing account" and remove the accounts "834213", "320421" and "3214".**
## Can you confirm that those account were deleted, both in the application and data storage (text file "records.txt" or database)?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[6]- Remove existing account` → enter `000000834213` → `✔ Success!`
2. Repeat for `000000320421` and `000000003214`
3. Open `data/records.txt` — those three entries are gone ✓
4. Choosing `[3]- Check accounts` and entering any of those numbers now returns `✖ Record not found!!`

---

**Resorting to the user Alice select the option "Remove existing account" and try to remove an account that does not exist.**
## Did the application prompt some type of error saying that the account does not exist?

**Yes.**
cd atm-system
Run application & login:
1. Run `./atm` → choose `[1]- Login`
2. Enter username: `Alice`, password: `q1w2e3r4t5y6`
How to test:
1. Log in as Alice → `[6]- Remove existing account`
2. Enter a number that does not exist (e.g. `000000000000`)
3. The application prints:
```
✖ Record not found!!
Enter 0 to try again, 1 to return to main menu and 2 to exit:
```

---

**Create another user named "Michel". Then by using Alice select the option "transfer owner" and try to transfer ownership of the account "3212" to Michel.**
## Were you able to transfer the ownership of this account to Michel? And if so did it update both application and data storage (text file "records.txt" or database)?

**Yes.**
cd atm-system
Run application:
1. Run `./atm` → choose `[1]- Login`
How to test:
1. Register `Michel` if not already registered
2. Log in as Alice → `[7]- Transfer ownership`
3. Enter account number: `000000003212`
4. Enter target username: `Michel` *(case-sensitive — `michel` will not match)*
5. Application confirms `✔ Success!`
6. Open `data/records.txt` — the entry for `000000003212` now shows `Michel` as the owner ✓

---

# Bonus

**Open two terminals and login with two different users. Then transfer ownership of an account to the other user.**
## +Was the user who received the account notified instantly?

**Yes.**

How to test:
1. Terminal 1: `./atm` → log in as `Bob`
2. Terminal 2: `./atm` → log in as `Alice`
3. In Terminal 2 (Alice): `[7]- Transfer ownership` → enter a valid account number → target: `Bob`
4. The moment Alice confirms, Terminal 1 (Bob) instantly shows:
```
========================================
         *** NOTIFICATION ***
  'Alice' transferred account #000000003212 to you!
========================================
```

This works via a named FIFO at `/tmp/atm_notify_Bob` and a forked child process that blocks on `read()`. You can verify the FIFO exists while Bob is logged in:
```bash
ls /tmp/atm_notify_*
```

## +Did the student update the terminal interface?

**No.** The UI uses the same basic `printf`/`scanf` terminal interface.

## +Is the password saved in the data storage (text file "users.txt" or database) encrypted?

**No.** Passwords are stored in plain text in `users.txt`.

## +Did the student create a relational database?

**No.** Storage uses plain text files. Three files are maintained: `users.txt`, `records.txt`, and `transactions.txt`.

## +Did the student make their own Makefile?

**Yes.** A `Makefile` is present that compiles `main.c`, `auth.c`, `system.c`, and `notification.c` into separate object files and links them into the `atm` executable. A `clean` target is also provided.

To build:
```bash
make clean && make
```

## +Did the student add more features to the project?

**Yes.** The following were added beyond the base requirements:

1. **Real-time transfer notifications** — named pipes (FIFOs) + `fork()`. The recipient is instantly alerted in their terminal when an account is transferred to them, even mid-session.
2. **Transaction log** — every deposit and withdrawal is appended to `data/transactions.txt` with the transaction ID, user, account number, type, amount, new balance, and date.

## +Did the student optimize the code already given?

**Yes.** The following optimisations and fixes were applied:

1. **`u.id` garbage value bug** — `struct User u` was uninitialised at declaration. Added `= {0}` at declaration and introduced `loadUser()` in `auth.c` to correctly populate `u.id` from `users.txt` after every login or registration.
2. **Record ID uniqueness** — changed from `recordCount` (which could repeat after deletions) to `maxId + 1`, guaranteeing a unique ID even if records have been removed.
3. **Account number validation** — enforced to be exactly 12 digits and globally unique across all records. Input is re-prompted until valid.
4. **Phone number stored as string** — changed from `long` (which stripped leading zeros and had no format check) to `char[16]`. Accepts up to 15 digits, preserves leading zeros (e.g. country prefix `0044...`), and rejects any non-digit characters.
5. **User ID zero-padded** — stored as 3-digit zero-padded integer (`000`, `001`, `002`…) in both `users.txt` and `records.txt`.
