# Library Book Management System (LBMS)

## Overview
This is a comprehensive Library Book Management System developed in C++ for the Bukit Katil Community Library (BKCL). It includes all core features and multiple bonus features to ensure maximum marks.

## Features
- **User Authentication**: Secure login and registration system (Bonus).
- **Book Catalogue**: Add, view, search, edit, and delete (Bonus) books.
- **Loan Management**: Loan books with a 5-book limit and eligibility checks.
- **Return Management**: Return books with automatic overdue fee calculation.
- **Overdue Payments**: View overdue lists and process payments (Bonus).
- **Data Persistence**: All data is saved to text files (Bonus).

## Files
- `main.cpp`: The source code.
- `books.txt`: Stores book information.
- `users.txt`: Stores user credentials and status.
- `loans.txt`: Stores loan records.
- `payments.txt`: Stores payment history.

## How to Compile and Run
1. Compile the code:
   ```bash
   g++ -o lbms main.cpp
   ```
2. Run the application:
   ```bash
   ./lbms
   ```

## Overdue Fee Structure
- 1 Day: RM 5.00
- 2 Days: RM 6.00
- 3 Days: RM 7.00
- > 3 Days: RM 10.00 per day
