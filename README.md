# ATM Management System

> A terminal-based ATM management system written in C, featuring user authentication, account management, deposits, withdrawals, ownership transfers with real-time notifications, and persistent file-based storage. Built as part of a systems programming exercise to practise low-level memory and file manipulation.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [File Structure](#file-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Data Format](#data-format)
- [Account Types & Interest Rates](#account-types--interest-rates)
- [Input Validation](#input-validation)
- [Real-Time Notifications](#real-time-notifications)
- [Contributing](#contributing)

---

## Overview

This project is a terminal-based ATM management system written in **C**. It allows multiple users to register, log in, and manage their bank accounts entirely from the command line. All data is persisted to plain text files, making it lightweight and dependency-free.

---

## Features

| Feature | Status |
|---|---|
| User Registration (unique usernames, auto-incremented IDs) | Done |
| User Login (with hidden password input, case-sensitive) | Done |
| Create a new account (12-digit account number, validated phone) | Done |
| Check details of a single account (with interest calculation) | Done |
| Check list of all owned accounts | Done |
| Update account information (country / phone) | Done |
| Make a transaction — deposit or withdraw (saved to `transactions.txt`) | Done |
| Remove an existing account | Done |
| Transfer account ownership to another user | Done |
| Real-time notification on ownership transfer (named pipes + fork) | Done |

---

## File Structure

```
atm-system/
│
├── data/
│   ├── users.txt          # Registered user credentials
│   ├── records.txt        # All account records
│   └── transactions.txt   # Log of all deposits and withdrawals
│
├── src/
│   ├── main.c             # Entry point & menu logic
│   ├── auth.c             # Login, registration & user lookup
│   ├── system.c           # Core account management & validation
│   ├── notification.c     # Real-time IPC notifications (named pipes + fork)
│   └── header.h           # Shared structs & function declarations
│
└── Makefile               # Build configuration
```

---

## Getting Started

### Prerequisites

- GCC (or any C compiler)
- A Unix-based system (Linux / macOS) — required for `termios.h` and `unistd.h`

### Building the Project

Clone the repository and run:

```bash
git clone https://github.com/nikomakr/atm-management-system.git
cd atm-management-system/atm-system
make
```

To force a full recompile from scratch:

```bash
make clean && make
```

### Running the Application

```bash
./atm
```

---

## Usage

On launch, you will be presented with an initial menu:

```
======= ATM =======

[1]- Login
[2]- Register
[3]- Exit
```

After logging in, the main menu provides the following options:

```
======= ATM =======

[1]- Create a new account
[2]- Update account information
[3]- Check accounts
[4]- Check list of owned accounts
[5]- Make Transaction
[6]- Remove existing account
[7]- Transfer ownership
[8]- Exit
```

---

## Data Format

### `data/users.txt`

Each line represents a registered user:

```
<id> <username> <password>
```

| Field | Description |
|---|---|
| `id` | 3-digit zero-padded, auto-incremented (000, 001, 002…) |
| `username` | Unique. Duplicate names rejected at registration. Case-sensitive. |
| `password` | Stored in plain text |

**Example:**
```
000 Alice securepassword
001 Michel anotherpassword
002 Bob yetanotherpassword
```

---

### `data/records.txt`

Each entry represents a bank account record:

```
<id> <user_id> <username> <account_number> <mm/dd/yyyy> <country> <phone> <balance> <account_type>
```

| Column | Field | Description |
|---|---|---|
| 1 | `id` | Unique record ID — always `max existing id + 1` |
| 2 | `user_id` | 3-digit zero-padded ID of the owning user (e.g. `002`) |
| 3 | `username` | Name of the owning user |
| 4 | `account_number` | Exactly 12 digits, globally unique |
| 5 | `deposit date` | Date the account was opened (mm/dd/yyyy) |
| 6 | `country` | Country entered by the user |
| 7 | `phone` | Up to 15 digits, leading zeros preserved (e.g. `004412345678`) |
| 8 | `balance` | Current balance in dollars |
| 9 | `account_type` | One of: `saving`, `current`, `fixed01`, `fixed02`, `fixed03` |

**Example:**
```
0 000 Alice 100000000001 10/10/2012 Germany 004491821234 22432.52 saving
1 001 Michel 100000000002 02/05/2001 Portugal 00351123543455 10023.23 fixed01
```

---

### `data/transactions.txt`

Each entry represents a single deposit or withdrawal:

```
<id> <user_id> <username> <account_number> <type> <amount> <new_balance> <mm/dd/yyyy>
```

| Column | Field | Description |
|---|---|---|
| 1 | `id` | Auto-incremented transaction ID |
| 2 | `user_id` | 3-digit zero-padded ID of the user who made the transaction |
| 3 | `username` | Name of the user |
| 4 | `account_number` | 12-digit account number |
| 5 | `type` | `deposit` or `withdraw` |
| 6 | `amount` | Amount transacted |
| 7 | `new_balance` | Balance after the transaction |
| 8 | `date` | Date of the transaction (mm/dd/yyyy) |

**Example:**
```
0 000 Alice 100000000001 deposit 500.00 22932.52 10/10/2012
1 000 Alice 100000000001 withdraw 100.00 22832.52 10/10/2012
```

---

## Account Types & Interest Rates

When creating an account, the following types are available:

| Account Type | Description | Interest | Transactions |
|---|---|---|---|
| `current` | Standard current account | None | Allowed |
| `saving` | Savings account | 7% per year, paid monthly (~0.583%) on the deposit day | Allowed |
| `fixed01` | Fixed term — 1 year | 4% paid on maturity date | Not allowed |
| `fixed02` | Fixed term — 2 years | 5% paid on maturity date | Not allowed |
| `fixed03` | Fixed term — 3 years | 8% paid on maturity date | Not allowed |

> Interest is calculated and displayed when checking a single account (`[3]- Check accounts`). Fixed accounts cannot make deposits or withdrawals — an error is shown if attempted.

---

## Input Validation

The following rules are enforced at the point of input, with re-prompting on failure:

| Field | Rule |
|---|---|
| Account number | Exactly 12 digits; globally unique across all records |
| Phone number | Digits only; 1–15 characters; leading zeros preserved (e.g. country prefix `0044...`) |
| User ID | 3-digit zero-padded, auto-assigned — never manually entered |
| Record ID | Auto-assigned as `max existing id + 1` — always unique even after deletions |
| Username | Case-sensitive at login and in all lookups (e.g. `Alice` ≠ `alice`) |
| Withdrawal amount | Must be greater than zero and not exceed the available balance |
| Deposit amount | Must be greater than zero |

---

## Real-Time Notifications

When a user transfers ownership of an account to another user, the recipient is **instantly notified** — even if they are in the middle of using the application in a separate terminal.

### How it works

This is implemented using **named pipes (FIFOs)** and **child processes**:

1. On login, a named FIFO is created at `/tmp/atm_notify_<username>`.
2. A **child process** is forked. It blocks on `read()` from the FIFO, sleeping until data arrives.
3. When a transfer is made, the sender writes a message to the recipient's FIFO.
4. The recipient's child process wakes up and prints the notification to their terminal immediately.
5. On exit, the child is terminated and the FIFO is cleaned up automatically via `atexit`.

If the recipient is not logged in, the notification is silently skipped (the write is non-blocking).

### Demo

```
Terminal A — Alice                        Terminal B — Bob (idle at menu)
─────────────────────────────────         ──────────────────────────────
[7]- Transfer ownership
Enter account: 123456789012
Transfer to: Bob
✔ Success!
                                           ========================================
                                                    *** NOTIFICATION ***
                                             'Alice' transferred account
                                             #123456789012 to you!
                                           ========================================
```

---

## Contributing

This project was completed as part of a systems programming exercise. Feel free to fork it and extend it further — some ideas:

- Password encryption
- SQLite database backend instead of flat files
- Improved TUI using `ncurses`

---

*Built with C as part of a low-level systems programming module.*
