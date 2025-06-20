#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include "tournament_registration.hpp"

using namespace std;

bool isIdTaken(int id, PriorityQueue& pq) {
    for (int i = 0; i < pq.size; ++i) {
        if (pq.queue[i].id == id) return true;
    }
    return false;
}

bool isIdInWithdrawals(int id) {
    ifstream withdrawFile("withdrawals.txt");
    if (!withdrawFile.is_open()) {
        cout << "Error opening withdrawals.txt\n";
        return false;
    }
    string line;
    while (getline(withdrawFile, line)) {
        stringstream ss(line);
        string id_str;
        getline(ss, id_str, ',');
        int fileId = stoi(id_str);
        if (fileId == id) {
            withdrawFile.close();
            return true;
        }
    }
    withdrawFile.close();
    return false;
}

bool isIdInCheckedIn(int id) {
    ifstream checkedInFile("checked_in.txt");
    if (!checkedInFile.is_open()) {
        cout << "Error opening checked_in.txt\n";
        return false;
    }
    string line;
    while (getline(checkedInFile, line)) {
        stringstream ss(line);
        string id_str;
        getline(ss, id_str, ',');
        int fileId = stoi(id_str);
        if (fileId == id) {
            checkedInFile.close();
            return true;
        }
    }
    checkedInFile.close();
    return false;
}

int getValidatedIdOrCancel(const string& action, PriorityQueue& pq) {
    int id;
    string input;
    while (true) {
        cout << "Please enter the player ID to " << action << " or 'c' to cancel: ";

        // Clear any leftover input before reading new line
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        getline(cin, input);
        if (input == "c" || input == "C") return -1;
        try {
            id = stoi(input);
            if (action == "register") {
                if (isIdTaken(id, pq) || isIdInWithdrawals(id) || isIdInCheckedIn(id)) {
                    cout << "ID already used. Enter a different ID.\n";
                    continue;
                }
            }
            else if (action == "check in") {
                if (isIdInCheckedIn(id)) {
                    cout << "Player already checked in. Enter different ID.\n";
                    continue;
                }
            }
            else if (action == "withdraw") {
                if (isIdInWithdrawals(id)) {
                    cout << "Player already withdrawn. Enter different ID.\n";
                    continue;
                }
            }
            return id;
        }
        catch (...) {
            cout << "Invalid input! Enter a numeric value or 'c' to cancel.\n";
        }
    }
}


void Task2Main() {
    PriorityQueue playerQueue;

    // Print header with date & time
    time_t now = time(nullptr);
    tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    char timeBuffer[100];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %I:%M:%S %p", &localTime);

    cout << "========================================\n";
    cout << "      TOURNAMENT REGISTRATION &         \n";
    cout << "           PLAYER QUEUEING               \n";
    cout << "   Date and Time: " << timeBuffer << "\n";
    cout << "========================================\n\n";

    loadPlayersFromFile(playerQueue);

    int choice;
    do {
        cout << "\nMain Menu\n";
        cout << "1. Register Player\n";
        cout << "2. Check-in Player\n";
        cout << "3. Withdraw Player\n";
        cout << "4. Replace Player\n";
        cout << "5. Display Queue\n";
        cout << "6. Exit\n";
        cout << "(Choose from option 1 to 6): ";

        while (!(cin >>choice)) {
            cout << "Enter your choice: ";
            if (!(cin >> choice)) {
                cout << "Invalid input! Enter number 1-6.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            if (choice < 1 || choice > 6) {
                cout << "Please enter a valid option 1-6.\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore();
            break;
        }

        if (choice == 1) {
            int id = getValidatedIdOrCancel("register", playerQueue);
            if (id == -1) {
                cout << "Action canceled. Returning to menu.\n";
                continue;
            }
            registerPlayer(playerQueue, id);
        }
        else if (choice == 2) {
            int id = getValidatedIdOrCancel("check in", playerQueue);
            if (id == -1) {
                cout << "Action canceled. Returning to menu.\n";
                continue;
            }
            checkInPlayer(playerQueue, id);
        }
        else if (choice == 3) {
            int id = getValidatedIdOrCancel("withdraw", playerQueue);
            if (id == -1) {
                cout << "Action canceled. Returning to menu.\n";
                continue;
            }
            withdrawPlayer(playerQueue, id);
        }
        else if (choice == 4) {
            replacePlayer(playerQueue, 0, "", "");
        }
        else if (choice == 5) {
            displayPlayers(playerQueue);
        }
        else if (choice == 6) {
            cout << "Thank you for using the program. Goodbye!\n";
            cout << "Press Enter to exit...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    } while (choice != 6);

    
}
