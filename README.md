# ATM Management System

> A terminal-based ATM management system written in C, featuring user authentication, account management, deposits, withdrawals, ownership transfers with real-time notifications, SHA-256 password encryption, and a SQLite relational database backend. Built as part of a systems programming exercise.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [File Structure](#file-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Database Schema](#database-schema)
- [Account Types & Interest Rates](#account-types--interest-rates)
- [Input Validation](#input-validation)
- [Real-Time Notifications](#real-time-notifications)

---

## Overview

This project is a terminal-based ATM management system written in **C**. It allows multiple users to register, log in, and manage their bank accounts entirely from the command line. All data is persisted in a **SQLite relational database** (`data/atm.db`), which is created and seeded automatically on first launch. Passwords are hashed with **SHA-256** before storage — plain-text passwords never touch disk.

---

## Features

| Feature | Status |
|---|---|
| User registration (unique usernames, auto-incremented IDs) | ✔ |
| User login (hidden password input, SHA-256 comparison) | ✔ |
| Create a new account (validated account number and phone) | ✔ |
| Check details of a single account (with interest calculation) | ✔ |
| List all owned accounts | ✔ |
| Update account information (country / phone) | ✔ |
| Make a transaction — deposit or withdraw (logged to `transactions` table) | ✔ |
| Remove an existing account | ✔ |
| Transfer account ownership to another user | ✔ |
| Real-time notification on ownership transfer (named pipes + fork) | ✔ |
| SHA-256 password encryption (macOS CommonCrypto) | ✔ |
| SQLite relational database backend | ✔ |
| Styled TUI (ANSI colours, Unicode box-drawing) | ✔ |

---

## File Structure

```
atm-management-system/
│
├── AUDIT.md                   # Functional audit Q&A with test instructions
├── README.md
│
└── atm-system/
    │
    ├── data/
    │   ├── atm.db             # SQLite database (auto-created on first run)
    │   └── legacy/            # Former plain-text storage (no longer in use)
    │       ├── users.txt
    │       ├── records.txt
    │       └── transactions.txt
    │
    ├── src/
    │   ├── main.c             # Entry point, menu logic, DB lifecycle
    │   ├── auth.c             # Login, registration, password hashing
    │   ├── system.c           # Account management, transactions, validation
    │   ├── db.c               # SQLite open/close, schema creation, seed data
    │   ├── notification.c     # Real-time IPC notifications (named pipes + fork)
    │   └── header.h           # Shared structs, macros & function declarations
    │
    └── Makefile               # Build configuration
```

---

## Getting Started

### Prerequisites

- A C compiler (`cc` / `clang`)
- macOS — required for `termios.h`, `CommonCrypto`, and `unistd.h`
- SQLite3 (`sqlite3` is bundled with macOS)

### Building

```bash
git clone <repo-url>
cd atm-management-system/atm-system
make
```

To force a full recompile:

```bash
make clean && make
```

To build and launch in one step:

```bash
make run
```

### Running

```bash
cd atm-management-system/atm-system
./atm
```

The database `data/atm.db` is created and seeded automatically on first launch. No manual setup is needed.

---

## Usage

On launch, the initial menu appears:

```
  ╔══════════════════════════════════════════════╗
  ║           ATM Management System              ║
  ╠══════════════════════════════════════════════╣
  ║                                              ║
  ║   [1]  Login                                 ║
  ║   [2]  Register                              ║
  ║   [3]  Exit                                  ║
  ║                                              ║
  ╚══════════════════════════════════════════════╝
```

After logging in, the main menu greets the user by name:

```
  ╔══════════════════════════════════════════════╗
  ║              Welcome back, Alice!            ║
  ╠══════════════════════════════════════════════╣
  ║                                              ║
  ║   [1]  Create a new account                  ║
  ║   [2]  Update account information            ║
  ║   [3]  Check accounts                        ║
  ║   [4]  List all owned accounts               ║
  ║   [5]  Make Transaction                      ║
  ║   [6]  Remove existing account               ║
  ║   [7]  Transfer ownership                    ║
  ║   [8]  Exit                                  ║
  ║                                              ║
  ╚══════════════════════════════════════════════╝
```

---

## Database Schema

All data is stored in `data/atm.db`. You can inspect it from the `atm-management-system/` root:

```bash
sqlite3 atm-system/data/atm.db "SELECT name FROM users;"
sqlite3 atm-system/data/atm.db ".schema"
```

### `users`

| Column | Type | Notes |
|---|---|---|
| `id` | INTEGER PRIMARY KEY | Auto-assigned |
| `name` | TEXT UNIQUE NOT NULL | Case-sensitive |
| `password` | TEXT NOT NULL | SHA-256 hex digest (64 chars) |

### `records`

| Column | Type | Notes |
|---|---|---|
| `id` | INTEGER PRIMARY KEY AUTOINCREMENT | |
| `user_id` | INTEGER | Foreign key → `users.id` |
| `name` | TEXT | Owning user's name |
| `account_nbr` | TEXT UNIQUE | Digits only, globally unique |
| `deposit_month/day/year` | INTEGER | Account opening date |
| `country` | TEXT | |
| `phone` | TEXT | Up to 15 digits, leading zeros preserved |
| `amount` | REAL | Current balance |
| `account_type` | TEXT | `saving`, `current`, `fixed01`, `fixed02`, `fixed03` |

### `transactions`

| Column | Type | Notes |
|---|---|---|
| `id` | INTEGER PRIMARY KEY AUTOINCREMENT | |
| `user_id` | INTEGER | |
| `username` | TEXT | |
| `account_nbr` | TEXT | |
| `type` | TEXT | `deposit` or `withdraw` |
| `amount` | REAL | Amount transacted |
| `new_balance` | REAL | Balance after the transaction |
| `t_month/day/year` | INTEGER | Transaction date |

### Seed data

The following users and accounts are pre-loaded on first launch:

| ID | Username | Password |
|---|---|---|
| 0 | Alice | `q1w2e3r4t5y6` |
| 1 | Michel | `q1w2e3r4t5y6` |
| 2 | Bob | `123456789qwertyuio!` |
| 3 | Jade | `123Sa*` |
| 4 | Niko | `123!` |

| Account | Owner | Type | Balance |
|---|---|---|---|
| 834213 | Alice | saving | $1001.20 |
| 320421 | Alice | fixed01 | $1001.20 |
| 3214 | Alice | fixed02 | $1001.20 |
| 3212 | Alice | fixed03 | $1001.20 |

---

## Account Types & Interest Rates

| Account Type | Description | Interest | Transactions |
|---|---|---|---|
| `current` | Standard current account | None | Allowed |
| `saving` | Savings account | 7% p.a., paid monthly on the deposit day | Allowed |
| `fixed01` | Fixed term — 1 year | 4% on maturity | Not allowed |
| `fixed02` | Fixed term — 2 years | 10% on maturity | Not allowed |
| `fixed03` | Fixed term — 3 years | 24% on maturity | Not allowed |

> Interest is calculated and displayed when checking a single account (`[3]- Check accounts`). Fixed accounts cannot make deposits or withdrawals.

---

## Input Validation

| Field | Rule |
|---|---|
| Account number | Digits only, 1–20 characters, globally unique |
| Phone number | Digits only, 1–15 characters, leading zeros preserved |
| Username | Unique at registration; case-sensitive at login and all lookups |
| Withdrawal amount | Greater than zero and must not exceed available balance |
| Deposit amount | Greater than zero |

---

## Real-Time Notifications

When a user transfers ownership of an account, the recipient is **instantly notified** — even if they are mid-session in a separate terminal.

### How it works

1. On login, a named FIFO is created at `/tmp/atm_notify_<username>`.
2. A **child process** is forked. It blocks on `read()`, waiting for incoming data.
3. When a transfer is made, the sender writes to the recipient's FIFO via `sendNotification()`.
4. The child process wakes up and prints the notification immediately.
5. On exit, the child is terminated and the FIFO is cleaned up via `atexit`.

If the recipient is not logged in, the write is non-blocking and silently skipped.

### Demo

```
Terminal A — Alice                        Terminal B — Bob
─────────────────────────────────         ──────────────────────────────
[7]- Transfer ownership
Enter account: 834213
Transfer to: Bob
✔ Success!
                                           ========================================
                                                    *** NOTIFICATION ***
                                             'Alice' transferred account
                                             #834213 to you!
                                           ========================================
```
