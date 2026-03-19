# ATM Management System

> An ATM management system written in C, featuring user authentication, account creation, balance enquiries, deposits, withdrawals, account updates, ownership transfers, and persistent file-based storage. Built as part of a systems programming exercise to practise low-level memory and file manipulation.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [File Structure](#file-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Data Format](#data-format)
- [Account Types & Interest Rates](#account-types--interest-rates)
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
| Check details of a single account | TODO |
| Check list of all owned accounts | Done |
| Update account information (country / phone) | Done |
| Make a transaction (deposit / withdraw) | TODO |
| Remove an existing account | TODO |
| Transfer account ownership to another user | TODO |

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
│   ├── main.c           # Entry point & main menu logic
│   ├── auth.c           # Login & registration functions
│   ├── system.c         # Core account management functions
│   └── header.h         # Shared structs & function declarations
│
└── Makefile             # Build configuration
```

---

## Getting Started

### Prerequisites

- GCC (or any C compiler)
- A Unix-based system (Linux / macOS) — required for `termios.h` password masking

### Building the Project

Clone the repository and run:

```bash
git clone https://github.com/nikomakr/atm-management-system.git
cd atm-management-system/atm-system
make
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

Each line represents a registered user in the following format:

```
<id> <username> <password>
```

**Example:**
```
0 Alice securepassword
1 Michel anotherpassword
```

> Note: Usernames must be unique. Duplicate names are rejected at registration.

---

### `data/records.txt`

Each entry represents a bank account record in the following format:

```
<id> <user_id> <username> <account_number> <mm/dd/yyyy> <country> <phone> <balance> <account_type>
```

**Example:**
```
0 0 Alice 0 10/10/2012 Germany 291321234 22432.52 saving
1 1 Michel 2 02/05/2001 Portugal 123543455 10023.23 fixed01
```

---

## Account Types & Interest Rates

When creating an account, the following types are available:

| Account Type | Description | Interest | Deposit/Withdraw |
|---|---|---|---|
| `current` | Standard current account | None | Allowed |
| `saving` | Savings account | 0.5834% on the 10th of each month | Allowed |
| `fixed01` | Fixed term — 1 year | 4.005% paid on maturity date | Not allowed |
| `fixed02` | Fixed term — 2 years | 10.012% paid on maturity date | Not allowed |
| `fixed03` | Fixed term — 3 years | 24.029% paid on maturity date | Not allowed |

> **Note: Interest calculation and transaction restrictions per account type are not yet implemented.**

---

## Contributing

This project was completed as part of a systems programming exercise. Feel free to fork it and extend it further — some ideas:

- Password encryption
- SQLite database backend instead of flat files
- Real-time transfer notifications using pipes & child processes
- Improved TUI using `ncurses`

---

*Built with C as part of a low-level systems programming module.*