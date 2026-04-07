# ATM Management System

> A terminal-based ATM management system written in C, featuring user authentication, account management, ownership transfers with real-time notifications, and persistent file-based storage. Built as part of a systems programming exercise to practise low-level memory and file manipulation.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [File Structure](#file-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Data Format](#data-format)
- [Account Types & Interest Rates](#account-types--interest-rates)
- [Real-Time Notifications](#real-time-notifications)
- [Contributing](#contributing)

---

## Overview

This project is a terminal-based ATM management system written in **C**. It allows multiple users to register, log in, and manage their bank accounts entirely from the command line. All data is persisted to plain text files, making it lightweight and dependency-free.

---

## Features

| Feature | Status |
|---|---|
| User Registration | Done |
| User Login (with hidden password input) | Done |
| Create a new account | Done |
| Check details of a single account | Done |
| Check list of all owned accounts | Done |
| Update account information (country / phone) | Done |
| Remove an existing account | Done |
| Transfer account ownership to another user | Done |
| Real-time notification on ownership transfer | Done |
| Make a transaction (deposit / withdraw) | TODO |

---

## File Structure

```
atm-system/
│
├── data/
│   ├── users.txt        # Stores registered user credentials
│   └── records.txt      # Stores all account records
│
├── src/
│   ├── main.c           # Entry point & menu logic
│   ├── auth.c           # Login, registration & user lookup
│   ├── system.c         # Core account management functions
│   ├── notification.c   # Real-time IPC notifications (named pipes + fork)
│   └── header.h         # Shared structs & function declarations
│
└── Makefile             # Build configuration
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
[5]- Make Transaction         (not yet implemented)
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

**Example:**
```
0 Alice securepassword
1 Michel anotherpassword
2 Bob yetanotherpassword
```

> Usernames must be unique. Duplicate names are rejected at registration. IDs are assigned automatically in incrementing order.

---

### `data/records.txt`

Each entry represents a bank account record:

```
<id> <user_id> <username> <account_number> <mm/dd/yyyy> <country> <phone> <balance> <account_type>
```

| Column | Field | Description |
|---|---|---|
| 1 | `id` | Record index (order it was created) |
| 2 | `user_id` | ID of the owning user from `users.txt` |
| 3 | `username` | Name of the owning user |
| 4 | `account_number` | Account number chosen at creation |
| 5 | `deposit date` | Date the account was opened (mm/dd/yyyy) |
| 6 | `country` | Country entered by the user |
| 7 | `phone` | Phone number |
| 8 | `balance` | Current balance in dollars |
| 9 | `account_type` | One of: `saving`, `current`, `fixed01`, `fixed02`, `fixed03` |

**Example:**
```
0 0 Alice 0 10/10/2012 Germany 291321234 22432.52 saving
1 1 Michel 2 02/05/2001 Portugal 123543455 10023.23 fixed01
```

---

## Account Types & Interest Rates

When creating an account, the following types are available:

| Account Type | Description | Interest |
|---|---|---|
| `current` | Standard current account | None |
| `saving` | Savings account | 7% per year, paid monthly (~0.583%) on the deposit day each month |
| `fixed01` | Fixed term — 1 year | 4% paid on maturity date |
| `fixed02` | Fixed term — 2 years | 5% paid on maturity date |
| `fixed03` | Fixed term — 3 years | 8% paid on maturity date |

> Interest is calculated and displayed when checking a single account. Actual transaction processing is not yet implemented.

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
Terminal A — Alice                   Terminal B — Bob (idle at menu)
──────────────────────────────       ──────────────────────────────
[7]- Transfer ownership
Enter account: 99
Transfer to: Bob
✔ Success!
                                      ========================================
                                               *** NOTIFICATION ***
                                        'Alice' transferred account #99 to you!
                                      ========================================
```

---

## Contributing

This project was completed as part of a systems programming exercise. Feel free to fork it and extend it further — some ideas:

- Password encryption
- SQLite database backend instead of flat files
- Improved TUI using `ncurses`
- Transaction history log per account

---

*Built with C as part of a low-level systems programming module.*
