# Functional

**Note: Confirm the data storage method used in the project (e.g., text files or a relational database like SQLite).**

Plain text files. Credentials are stored in `./data/users.txt` and account records in `./data/records.txt`, both using whitespace-delimited fields.

---

**Open the application and register a new user with the name "Marcus" and the password "q1w2e3r4t5y6".**
## Is this user saved in the data storage (text file "./data/users.txt" or database), and if so, are all credentials correct (name and password)?

**Yes.** `registerMenu()` in `auth.c` appends a new line to `users.txt` in the format `<id> <username> <password>`. The ID is derived by finding the current maximum ID in the file and incrementing it.

---

**Open the application and re-register the user "Alice".**
## Did the application display an error message stating that this user already exists?

**Yes.** `registerMenu()` scans `users.txt` for a matching username before writing. If a match is found it prints `"Sorry this username already exist"` and exits.

---

**Open the data storage (text file "./data/users.txt" or database).**
## Are all the user names unique? (ex: no repetition on the name Alice)

**Yes.** Uniqueness is enforced at registration time — duplicate names are rejected before the new record is written.

---

**Try and login as "Alice".**
## Was Alice able to enter the main menu?

**Yes.** `initMenu()` in `main.c` calls `getPassword()` which looks up the stored password for the given username in `users.txt`. If the entered password matches, control passes to `mainMenu()`.

---

**Try to create two accounts with the user Alice, then select the option "Update information of account" and select an account number that does not exist for Alice.**
## Did the application display some kind of error message stating that this account does not exist?

**Yes.** `updateAccount()` in `system.c` scans all records for one matching both the given account number and the logged-in user's ID. If none is found it calls `stayOrReturn(0, updateAccount, u)` which prints `"Record not found!!"` and offers a retry or return to the main menu.

---

**Resorting to the user Alice, try and select the option "Update information of account" and select one of the accounts you created for Alice.**
## Did the application prompt a choice of updating the phone number or the country?

**Yes.** After locating the account, `updateAccount()` presents:
```
What do you want to update?
[1]- Phone Number
[2]- Country
```

---

**Resorting to the user Alice, try and select the option "Update information of account" and select one of the accounts you created for Alice. Then update the phone number of that account.**
## Was the phone number of that account updated in the application and the data storage (text file "records.txt" or database)?

**Yes.** The updated record is written back to `records.txt` by rewriting the entire file with the modified struct values.

---

**Resorting to the user Alice, try and select the option "Update information of account" and select one of the accounts you created for Alice. Then update the country of that account.**
## Was the country of that account updated in the application and the data storage (text file "records.txt" or database)?

**Yes.** Same mechanism as the phone update above — full file rewrite with the updated country field.

---

**Resorting to the user Alice, try to create a new account with: date "10/10/2012" account number "834213", country "UK", phone number "291231392", deposit amount $"1001.20", type of account "saving". Then select "Check accounts" choose the account you just created.**
## Did the application display the account information and the gain of $5.84 of interest on day 10 of every month?

**No.** `checkAllAccounts()` lists account details (number, date, country, phone, balance, type) but does not calculate or display interest. Interest logic for saving accounts (0.5834% monthly) is not yet implemented.

---

**Resorting to the user Alice create again an account but with account number "320421" and type of account "fixed01" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $40.05 of interest on 10/10/2013?

**No.** Interest calculation for `fixed01` accounts (4.005% at maturity) is not yet implemented.

---

**Resorting to the user Alice create again an account but with account number "3214" and type of account "fixed02" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $100.12 of interest on 10/10/2014?

**No.** Interest calculation for `fixed02` accounts (10.012% at maturity) is not yet implemented.

---

**Resorting to the user Alice create again an account but with account number "3212" and type of account "fixed03" with the rest of the information as in the last account. Then select "Check accounts" and choose the account you just created.**
## Did the application display the account information and the gain of $240.29 of interest on 10/10/2015?

**No.** Interest calculation for `fixed03` accounts (24.029% at maturity) is not yet implemented.

---

**Resorting to the user Alice select the option "Make transaction". Then choose the account with the id "3212"**
## Was an error message displayed stating it is not possible to withdraw or deposit for "fixed" accounts?

**No.** The "Make Transaction" option (menu item 5) is not yet implemented — selecting it does nothing.

---

**Resorting to the user Alice select the option "Make transaction", choose the account with the id "834213". Then try to withdraw money.**
## Are you able to withdraw money?
## And if so, was the withdrawal updated in the data storage (text file "records.txt" or database)?
## Does the system forbid to withdraw an amount superior to your available balance?

**No.** Transactions are not yet implemented.

---

**Try to deposit money into the account "834213".**
## Were you able to deposit money into this account?
## And if so did it update the data storage (text file "records.txt" or database)?

**No.** Transactions are not yet implemented.

---

**Resorting to the user Alice try to select the option "Remove existing account" and remove the accounts "834213", "320421" and "3214".**
## Can you confirm that those account were deleted, both in the application and data storage (text file "records.txt" or database)?

**No.** Account removal (menu item 6) is not yet implemented.

---

**Resorting to the user Alice select the option "Remove existing account" and try to remove an account that does not exist.**
## Did the application prompt some type of error saying that the account does not exist?

**No.** Account removal is not yet implemented.

---

**Create another user named "Michel". Then by using Alice select the option "transfer owner" and try to transfer ownership of the account "3212" to Michel.**
## Were you able to transfer the ownership of this account to Michel? And if so did it update both application and data storage (text file "records.txt" or database)?

**No.** Transfer ownership (menu item 7) is not yet implemented.

---

# Bonus

**Open two terminals and login with two different users. Then transfer ownership of an account to the other user.**
## +Was the user who received the account notified instantly?

**No.** Real-time inter-process notification is not implemented.

## +Did the student update the terminal interface?

**No.** The UI uses the same basic `printf`/`scanf` terminal interface.

## +Is the password saved in the data storage (text file "users.txt" or database) encrypted?

**No.** Passwords are stored in plain text.

## +Did the student create a relational database?

**No.** Storage uses plain text files (`users.txt` and `records.txt`).

## +Did the student make their own Makefile?

**Yes.** A `Makefile` is present that compiles `main.c`, `auth.c`, and `system.c` into separate object files and links them into the `atm` executable. A `clean` target is also provided.

## +Did the student add more features to the project?

**No.** The implemented features match the base requirements only.

## +Did the student optimize the code already given?

**Partially.** Five bugs identified in the original scaffold were fixed (`system.c`, `auth.c`, `header.h`). No broader algorithmic or structural optimisations were applied.
