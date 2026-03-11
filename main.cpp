#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <ctime>

using namespace std;

// --- Constants ---
const string BOOKS_FILE = "books.txt";
const string USERS_FILE = "users.txt";
const string LOANS_FILE = "loans.txt";
const string PAYMENTS_FILE = "payments.txt";
const int MAX_LOANS = 5;

// --- Structs ---
struct Book {
    string id;
    string title;
    string author;
    bool isAvailable;
};

struct User {
    string userId;
    string username;
    string password;
    int loanCount;
    double totalOverdueFee;
};

struct Loan {
    string loanId;
    string bookId;
    string userId;
    time_t loanDate;
    time_t dueDate;
    time_t returnDate;
    bool isReturned;
};

struct Payment {
    string paymentId;
    string userId;
    double amount;
    time_t paymentDate;
};

// --- Global Data ---
vector<Book> books;
vector<User> users;
vector<Loan> loans;
vector<Payment> payments;
User* currentUser = nullptr;

// --- Utility Functions ---
string getCurrentDateStr(time_t t) {
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return string(buf);
}

time_t addDays(time_t t, int days) {
    return t + (days * 24 * 60 * 60);
}

double calculateOverdueFee(time_t dueDate, time_t returnDate) {
    if (returnDate <= dueDate) return 0.0;
    double diff = difftime(returnDate, dueDate);
    int days = (int)(diff / (24 * 60 * 60));
    if (days <= 0) return 0.0;
    
    if (days == 1) return 5.00;
    if (days == 2) return 6.00;
    if (days == 3) return 7.00;
    return days * 10.00;
}

// --- File I/O ---
void loadBooks() {
    books.clear();
    ifstream file(BOOKS_FILE);
    string id, title, author;
    bool avail;
    while (file >> id >> ws && getline(file, title, '|') && getline(file, author, '|') >> avail) {
        books.push_back({id, title, author, avail});
    }
}

void saveBooks() {
    ofstream file(BOOKS_FILE);
    for (const auto& b : books) {
        file << b.id << " " << b.title << "|" << b.author << "|" << b.isAvailable << endl;
    }
}

void loadUsers() {
    users.clear();
    ifstream file(USERS_FILE);
    string id, name, pass;
    int count;
    double fee;
    while (file >> id >> name >> pass >> count >> fee) {
        users.push_back({id, name, pass, count, fee});
    }
}

void saveUsers() {
    ofstream file(USERS_FILE);
    for (const auto& u : users) {
        file << u.userId << " " << u.username << " " << u.password << " " << u.loanCount << " " << u.totalOverdueFee << endl;
    }
}

void loadLoans() {
    loans.clear();
    ifstream file(LOANS_FILE);
    string lid, bid, uid;
    time_t ld, dd, rd;
    bool ret;
    while (file >> lid >> bid >> uid >> ld >> dd >> rd >> ret) {
        loans.push_back({lid, bid, uid, ld, dd, rd, ret});
    }
}

void saveLoans() {
    ofstream file(LOANS_FILE);
    for (const auto& l : loans) {
        file << l.loanId << " " << l.bookId << " " << l.userId << " " << l.loanDate << " " << l.dueDate << " " << l.returnDate << " " << l.isReturned << endl;
    }
}

void loadPayments() {
    payments.clear();
    ifstream file(PAYMENTS_FILE);
    string pid, uid;
    double amt;
    time_t pd;
    while (file >> pid >> uid >> amt >> pd) {
        payments.push_back({pid, uid, amt, pd});
    }
}

void savePayments() {
    ofstream file(PAYMENTS_FILE);
    for (const auto& p : payments) {
        file << p.paymentId << " " << p.userId << " " << p.amount << " " << p.paymentDate << endl;
    }
}

// --- Core Functions ---

void addBook() {
    string id, title, author;
    cout << "Enter Book ID: "; cin >> id;
    cout << "Enter Title: "; cin.ignore(); getline(cin, title);
    cout << "Enter Author: "; getline(cin, author);
    books.push_back({id, title, author, true});
    saveBooks();
    cout << "Book added successfully!\n";
}

void viewBooks() {
    cout << left << setw(10) << "ID" << setw(30) << "Title" << setw(20) << "Author" << "Status" << endl;
    cout << string(70, '-') << endl;
    for (const auto& b : books) {
        cout << left << setw(10) << b.id << setw(30) << b.title << setw(20) << b.author << (b.isAvailable ? "Available" : "Loaned") << endl;
    }
}

void searchBook() {
    string query;
    cout << "Enter Book ID or Title to search: "; cin.ignore(); getline(cin, query);
    bool found = false;
    for (const auto& b : books) {
        if (b.id == query || b.title.find(query) != string::npos) {
            cout << "Found: " << b.id << " | " << b.title << " by " << b.author << " [" << (b.isAvailable ? "Available" : "Loaned") << "]\n";
            found = true;
        }
    }
    if (!found) cout << "No book found.\n";
}

void editBook() {
    string id;
    cout << "Enter Book ID to edit: "; cin >> id;
    for (auto& b : books) {
        if (b.id == id) {
            cout << "New Title (current: " << b.title << "): "; cin.ignore(); getline(cin, b.title);
            cout << "New Author (current: " << b.author << "): "; getline(cin, b.author);
            saveBooks();
            cout << "Book updated!\n";
            return;
        }
    }
    cout << "Book not found.\n";
}

void deleteBook() {
    string id;
    cout << "Enter Book ID to delete: "; cin >> id;
    auto it = remove_if(books.begin(), books.end(), [&](const Book& b) { return b.id == id; });
    if (it != books.end()) {
        books.erase(it, books.end());
        saveBooks();
        cout << "Book deleted!\n";
    } else {
        cout << "Book not found.\n";
    }
}

void loanBook() {
    if (currentUser->loanCount >= MAX_LOANS) {
        cout << "Loan limit reached (max 5).\n";
        return;
    }
    if (currentUser->totalOverdueFee > 0) {
        cout << "You have unpaid overdue fees (RM " << fixed << setprecision(2) << currentUser->totalOverdueFee << "). Please pay first.\n";
        return;
    }

    string bid;
    cout << "Enter Book ID to loan: "; cin >> bid;
    for (auto& b : books) {
        if (b.id == bid) {
            if (!b.isAvailable) {
                cout << "Book is already loaned.\n";
                return;
            }
            b.isAvailable = false;
            time_t now = time(0);
            loans.push_back({"L" + to_string(loans.size() + 1), b.id, currentUser->userId, now, addDays(now, 14), 0, false});
            currentUser->loanCount++;
            saveBooks();
            saveLoans();
            saveUsers();
            cout << "Book loaned successfully! Due date: " << getCurrentDateStr(addDays(now, 14)) << endl;
            return;
        }
    }
    cout << "Book not found.\n";
}

void returnBook() {
    string bid;
    cout << "Enter Book ID to return: "; cin >> bid;
    for (auto& l : loans) {
        if (l.bookId == bid && l.userId == currentUser->userId && !l.isReturned) {
            l.isReturned = true;
            time_t now = time(0);
            l.returnDate = now;
            
            for (auto& b : books) {
                if (b.id == bid) b.isAvailable = true;
            }
            
            double fee = calculateOverdueFee(l.dueDate, now);
            if (fee > 0) {
                currentUser->totalOverdueFee += fee;
                cout << "Book returned LATE. Overdue fee: RM " << fixed << setprecision(2) << fee << endl;
            } else {
                cout << "Book returned on time.\n";
            }
            
            currentUser->loanCount--;
            saveBooks();
            saveLoans();
            saveUsers();
            return;
        }
    }
    cout << "No active loan found for this book and user.\n";
}

void viewOverdue() {
    cout << "--- Overdue Payments ---\n";
    bool found = false;
    for (const auto& u : users) {
        if (u.totalOverdueFee > 0) {
            cout << "User: " << u.username << " (ID: " << u.userId << ") | Owed: RM " << fixed << setprecision(2) << u.totalOverdueFee << endl;
            found = true;
        }
    }
    if (!found) cout << "No overdue payments.\n";
}

void payOverdue() {
    if (currentUser->totalOverdueFee <= 0) {
        cout << "You have no overdue fees.\n";
        return;
    }
    cout << "Your total overdue fee is RM " << fixed << setprecision(2) << currentUser->totalOverdueFee << endl;
    cout << "Confirm payment? (y/n): ";
    char choice; cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        payments.push_back({"P" + to_string(payments.size() + 1), currentUser->userId, currentUser->totalOverdueFee, time(0)});
        currentUser->totalOverdueFee = 0;
        saveUsers();
        savePayments();
        cout << "Payment successful!\n";
    }
}

// --- Auth ---
bool login() {
    string name, pass;
    cout << "--- Login ---\nUsername: "; cin >> name;
    cout << "Password: "; cin >> pass;
    for (auto& u : users) {
        if (u.username == name && u.password == pass) {
            currentUser = &u;
            return true;
        }
    }
    return false;
}

void registerUser() {
    string name, pass;
    cout << "--- Register ---\nUsername: "; cin >> name;
    cout << "Password: "; cin >> pass;
    users.push_back({"U" + to_string(users.size() + 1), name, pass, 0, 0.0});
    saveUsers();
    cout << "Registration successful! Please login.\n";
}

// --- Main Menu ---
void menu() {
    while (true) {
        cout << "\n--- LBMS Menu (User: " << currentUser->username << ") ---\n";
        cout << "1. Add Book\n2. View Books\n3. Search Book\n4. Edit Book\n5. Delete Book (Bonus)\n";
        cout << "6. Loan Book\n7. Return Book\n8. View Overdue List\n9. Pay Overdue (Bonus)\n0. Logout\nChoice: ";
        int choice; cin >> choice;
        switch (choice) {
            case 1: addBook(); break;
            case 2: viewBooks(); break;
            case 3: searchBook(); break;
            case 4: editBook(); break;
            case 5: deleteBook(); break;
            case 6: loanBook(); break;
            case 7: returnBook(); break;
            case 8: viewOverdue(); break;
            case 9: payOverdue(); break;
            case 0: currentUser = nullptr; return;
            default: cout << "Invalid choice.\n";
        }
    }
}

int main() {
    loadBooks();
    loadUsers();
    loadLoans();
    loadPayments();

    while (true) {
        cout << "\n--- Welcome to BKCL Library ---\n1. Login\n2. Register\n3. Exit\nChoice: ";
        int choice; cin >> choice;
        if (choice == 1) {
            if (login()) menu();
            else cout << "Login failed.\n";
        } else if (choice == 2) {
            registerUser();
        } else if (choice == 3) {
            break;
        }
    }
    return 0;
}
