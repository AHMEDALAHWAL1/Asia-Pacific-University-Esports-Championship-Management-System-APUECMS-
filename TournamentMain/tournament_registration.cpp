#include "tournament_registration.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>
#include <ctime>
#include <iomanip>
#include "task_entry_points.hpp"

using namespace std;

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

bool isAlphaString(const string& s) {
    for (char c : s) {
        if (!isalpha(c) && c != ' ') return false;
    }
    return !s.empty();
}

bool isValidRegType(const string& s) {
    return s == "earlybird" || s == "wildcard" || s == "normal";
}

string getCurrentDateTime() {
    time_t now = time(nullptr);
    tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    int hour = localTime.tm_hour;
    string am_pm = "AM";

    if (hour == 0) hour = 12;
    else if (hour == 12) am_pm = "PM";
    else if (hour > 12) {
        hour -= 12;
        am_pm = "PM";
    }

    stringstream ss;
    ss << 1900 + localTime.tm_year << "-"
        << setw(2) << setfill('0') << 1 + localTime.tm_mon << "-"
        << setw(2) << setfill('0') << localTime.tm_mday << " "
        << setw(2) << setfill('0') << hour << ":"
        << setw(2) << setfill('0') << localTime.tm_min << ":"
        << setw(2) << setfill('0') << localTime.tm_sec << " "
        << am_pm;

    return ss.str();
}

Player::Player() {
    id = 0;
    name = "";
    registrationTime = "";
    status = "";
    priority = 0;
    regType = "";
}

Player::Player(int id, string name, string regTime, string status, int priority, string regType)
    : id(id), name(name), registrationTime(regTime), status(status), priority(priority), regType(regType) {
}

PriorityQueue::PriorityQueue() {
    size = 0;
    capacity = 10;
    queue = new Player[capacity];
}

PriorityQueue::~PriorityQueue() {
    delete[] queue;
}

void PriorityQueue::resize() {
    capacity *= 2;
    Player* newQueue = new Player[capacity];
    for (int i = 0; i < size; i++) {
        newQueue[i] = queue[i];
    }
    delete[] queue;
    queue = newQueue;
}

void PriorityQueue::insert(Player player) {
    if (size == capacity) {
        resize();
    }
    int i = size - 1;
    while (i >= 0 && queue[i].priority > player.priority) {
        queue[i + 1] = queue[i];
        i--;
    }
    queue[i + 1] = player;
    size++;
}

Player PriorityQueue::remove() {
    if (isEmpty()) {
        throw runtime_error("Queue is empty");
    }
    return queue[--size];
}

bool PriorityQueue::isEmpty() {
    return size == 0;
}

void loadPlayersFromFile(PriorityQueue& pq) {
    ifstream inputFile("players.txt");
    if (!inputFile.is_open()) {
        cout << "Error: Could not open players.txt file.\n";
        return;
    }

    string line;
    if (inputFile.peek() != EOF) {
        //getline(inputFile, line); // skip header if present
    }

    while (getline(inputFile, line)) {
        stringstream ss(line);
        string id_str, name, regTime, status, regType, priority_str;

        getline(ss, id_str, ',');
        getline(ss, name, ',');
        getline(ss, regTime, ',');
        getline(ss, status, ',');
        getline(ss, regType, ',');
        getline(ss, priority_str, ',');

        id_str = trim(id_str);
        name = trim(name);
        regTime = trim(regTime);
        status = trim(status);
        regType = trim(regType);
        priority_str = trim(priority_str);

        if (id_str.empty() || name.empty() || status.empty() || regType.empty() || priority_str.empty())
            continue;

        int id = stoi(id_str);
        int priority = stoi(priority_str);

        // Insert player into the priority queue if they are registered
        if (status == "Registered") {
            pq.insert(Player(id, name, regTime, status, priority, regType));
        }
    }
    inputFile.close();
}
void registerPlayer(PriorityQueue& pq, int id) {
   ;
    
    string firstName, lastName, regType, name;

    
    while (true) {
        cout << "Enter First Name (letters only): ";
        getline(cin, firstName);
        firstName = trim(firstName);
        if (!isAlphaString(firstName)) {
            cout << "Invalid input! First Name must contain letters only.\n";
            continue;
        }
        break;
    }

    while (true) {
        cout << "Enter Last Name (letters only): ";
        getline(cin, lastName);
        lastName = trim(lastName);
        if (!isAlphaString(lastName)) {
            cout << "Invalid input! Last Name must contain letters only.\n";
            continue;
        }
        break;
    }

    while (true) {
        cout << "Enter Registration Type (wildcard, earlybird, normal): ";
        getline(cin, regType);
        regType = trim(regType);
        if (!isValidRegType(regType)) {
            cout << "Invalid Registration Type! Must be 'wildcard', 'earlybird', or 'normal'.\n";
            continue;
        }
        break;
    }

    name = firstName + " " + lastName;

    int priority = 3; // default normal
    if (regType == "wildcard") priority = 1;
    else if (regType == "earlybird") priority = 2;

    string currentTime = getCurrentDateTime();

    Player newPlayer(id, name, currentTime, "Registered", priority, regType);
    pq.insert(newPlayer);

    ofstream playersFile("players.txt", ios::app);
    if (!playersFile) {
        cout << "Error opening players.txt for writing!\n";
    }
    else {
        playersFile << newPlayer.id << "," << newPlayer.name << "," << newPlayer.registrationTime << "," << newPlayer.status << "," << newPlayer.regType << "," << newPlayer.priority << "\n";
        if (playersFile.fail()) {
            cout << "Error writing to players.txt!\n";
        }
        else {
            cout << "Player " << name << " registered successfully and has been saved to the players list.\n";
        }
        playersFile.close();
    }

    cout << "Press Enter to return to the main menu...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard leftover input
    cin.get(); // wait for Enter key
    clearScreen();
    
}

bool parseDateTime(const string& datetimeStr, tm& outTm) {
    istringstream ss(datetimeStr);
    ss >> get_time(&outTm, "%Y-%m-%d %I:%M:%S %p");
    return !ss.fail();
}

void checkInPlayer(PriorityQueue& pq, int id) {
    bool found = false;
    Player checkedInPlayer;
    int removeIndex = -1;

    for (int i = 0; i < pq.size; ++i) {
        if (pq.queue[i].id == id) {
            found = true;
            checkedInPlayer = pq.queue[i];
            removeIndex = i;
            break;
        }
    }

    if (!found) {
        cout << "Player with ID " << id << " not found.\n";
        return;
    }

    tm regTm{};
    if (!parseDateTime(checkedInPlayer.registrationTime, regTm)) {
        cout << "Error parsing registration time.\n";
        return;
    }

    time_t reg_time_t = mktime(&regTm);
    time_t now = time(nullptr);
    double diffMinutes = difftime(now, reg_time_t) / 60.0;

    if (diffMinutes >= 30) {
        cout << "Check-in has been rejected – late by " << (int)diffMinutes << " minutes. Must check in within 30 minutes.\n";
        return;
    }

    if (diffMinutes > 10) {
        cout << "Warning: Player " << checkedInPlayer.name << " is checking in late (" << (int)diffMinutes << " minutes).\n";
    }

    checkedInPlayer.status = "CheckedIn";
    checkedInPlayer.registrationTime = getCurrentDateTime();

    ofstream checkedInFile("checked_in.txt", ios::app);
    if (!checkedInFile) {
        cout << "Failed to open checked_in.txt\n";
        return;
    }
    checkedInFile << checkedInPlayer.id << "," << checkedInPlayer.name << "," << checkedInPlayer.registrationTime << ","
        << checkedInPlayer.status << "," << checkedInPlayer.regType << "," << checkedInPlayer.priority << "\n";
    checkedInFile.close();

    for (int i = removeIndex; i < pq.size - 1; ++i) {
        pq.queue[i] = pq.queue[i + 1];
    }
    pq.size--;

    ofstream playersFile("players.txt");
    if (!playersFile) {
        cout << "Failed to open players.txt\n";
        return;
    }
    for (int i = 0; i < pq.size; ++i) {
        Player& p = pq.queue[i];
        playersFile << p.id << "," << p.name << "," << p.registrationTime << "," << p.status << "," << p.regType << "," << p.priority << "\n";
    }
    playersFile.close();

    cout << "Player " << checkedInPlayer.name << " checked in successfully at " << checkedInPlayer.registrationTime << ".\n";
    cout << "Press Enter to return to the main menu...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard leftover input
    cin.get(); // wait for Enter key
    clearScreen();
}


void withdrawPlayer(PriorityQueue& pq, int id) {
    bool found = false;

    // Open the withdrawals file for appending
    ofstream withdrawFile("withdrawals.txt", ios::app);
    if (!withdrawFile) {
        cout << "Error opening withdrawals.txt for writing!\n";
        return;
    }

    // Temporary queue to hold the remaining players
    PriorityQueue tempPQ;

    // Iterate over all players in the queue to find the withdrawn player
    for (int i = 0; i < pq.size; ++i) {
        Player& p = pq.queue[i];

        if (p.id == id) {
            // Player found, update their status to "Withdrawn"
            p.status = "Withdrawn";

            // Write the player to the withdrawals file
            withdrawFile << p.id << "," << p.name << "," << p.registrationTime << "," << p.status << "," << p.regType << "," << p.priority << "\n";
            cout << "Player " << p.name << " has been marked as Withdrawn and moved to withdrawals list.\n";
            found = true;  // Mark player as found
        }
        else {
            // If not the player to withdraw, keep the player in the temp queue
            tempPQ.insert(p);
        }
    }

    if (!found) {
        cout << "Player with ID " << id << " not found.\n";
        withdrawFile.close();
        return;
    }

    // Rebuild the players.txt file with the remaining players
    ofstream playersFile("players.txt");
    if (!playersFile) {
        cout << "Error opening players.txt for writing!\n";
        withdrawFile.close();
        return;
    }

    // Write all players from the temp queue back into players.txt
    for (int i = 0; i < tempPQ.size; ++i) {
        Player& p = tempPQ.queue[i];
        playersFile << p.id << "," << p.name << "," << p.registrationTime << "," << p.status << "," << p.regType << "," << p.priority << "\n";
    }

    withdrawFile.close();
    playersFile.close();
    cout << "Press Enter to return to the main menu...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard leftover input
    cin.get(); // wait for Enter key

    clearScreen();
}

void printWithdrawnPlayers() {
    std::ifstream withdrawFile("withdrawals.txt");
    if (!withdrawFile) {
        std::cout << "Error opening withdrawals.txt for reading!\n";
        return;
    }

    std::string line;
    std::cout << "List of withdrawn players:\n";
    while (getline(withdrawFile, line)) {
        std::cout << line << "\n";
    }
    withdrawFile.close();
}

void replacePlayer(PriorityQueue& pq, int /*unused*/, std::string /*unused*/, std::string /*unused*/) {
    const int MAX_WITHDRAWN = 100;
    int withdrawnIds[MAX_WITHDRAWN];
    int count = 0;

    std::ifstream withdrawFile("withdrawals.txt");
    if (!withdrawFile) {
        std::cout << "Error opening withdrawals.txt for reading!\n";
        return;
    }

    std::string line;
    std::cout << "List of withdrawn players:\n";
    std::cout << std::left
        << std::setw(4) << "ID"
        << " | " << std::setw(20) << "Name"
        << " | " << std::setw(10) << "Status"
        << " | " << std::setw(10) << "Type"
        << " | " << std::setw(8) << "Priority"
        << " | Registered at\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    while (getline(withdrawFile, line) && count < MAX_WITHDRAWN) {
        // Expected format: id,name,registrationTime,status,regType,priority
        size_t pos[6];
        pos[0] = line.find(',');
        pos[1] = line.find(',', pos[0] + 1);
        pos[2] = line.find(',', pos[1] + 1);
        pos[3] = line.find(',', pos[2] + 1);
        pos[4] = line.find(',', pos[3] + 1);

        if (pos[0] == std::string::npos || pos[1] == std::string::npos || pos[2] == std::string::npos ||
            pos[3] == std::string::npos || pos[4] == std::string::npos) {
            // Malformed line, just print raw and continue
            std::cout << line << "\n";
            continue;
        }

        std::string idStr = line.substr(0, pos[0]);
        std::string name = line.substr(pos[0] + 1, pos[1] - pos[0] - 1);
        std::string registrationTime = line.substr(pos[1] + 1, pos[2] - pos[1] - 1);
        std::string status = line.substr(pos[2] + 1, pos[3] - pos[2] - 1);
        std::string regType = line.substr(pos[3] + 1, pos[4] - pos[3] - 1);
        std::string priorityStr = line.substr(pos[4] + 1);

        int id = atoi(idStr.c_str());
        if (id > 0) {
            withdrawnIds[count++] = id;

            std::cout << std::setw(4) << id << " | "
                << std::setw(20) << name << " | "
                << std::setw(10) << status << " | "
                << std::setw(10) << regType << " | "
                << std::setw(8) << priorityStr << " | "
                << registrationTime << "\n";
        }
    }
    withdrawFile.close();

    int selectedId = 0;
    while (true) {
        std::cout << "Which withdrawn player do you want to replace? Enter the Player ID (or 'c' to cancel): ";
        std::string input;
        getline(std::cin, input);

        if (input == "c" || input == "C") {
            std::cout << "Replacement cancelled. Returning to main menu.\n";
            return; // Exit replacePlayer early
        }

        selectedId = atoi(input.c_str());

        bool found = false;
        for (int i = 0; i < count; ++i) {
            if (withdrawnIds[i] == selectedId) {
                found = true;
                break;
            }
        }

        if (found) {
            std::cout << "Selected player ID " << selectedId << " found.\n";
            break;
        }
        else {
            std::cout << "Player ID " << selectedId << " not found in withdrawn players. Please try again.\n";
        }
    }

    // Remove the selected player from withdrawals.txt (rewrite file excluding that player)
    std::ifstream inFile("withdrawals.txt");
    if (!inFile) {
        std::cout << "Error opening withdrawals.txt for reading!\n";
        return;
    }
    std::ofstream tempFile("withdrawals_temp.txt");
    if (!tempFile) {
        std::cout << "Error opening temporary file for writing!\n";
        inFile.close();
        return;
    }

    while (getline(inFile, line)) {
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            int id = atoi(line.substr(0, pos).c_str());
            if (id != selectedId) {
                tempFile << line << "\n";
            }
            // else skip line to remove it
        }
        else {
            tempFile << line << "\n";  // keep malformed lines
        }
    }

    inFile.close();
    tempFile.close();

    if (remove("withdrawals.txt") != 0) {
        std::cout << "Error deleting original withdrawals.txt file!\n";
        return;
    }
    if (rename("withdrawals_temp.txt", "withdrawals.txt") != 0) {
        std::cout << "Error renaming temporary file to withdrawals.txt!\n";
        return;
    }

    // Continue to get replacement player info
    std::string firstName, lastName, regType;

    while (true) {
        std::cout << "Enter First Name for replacement player (letters only): ";
        getline(std::cin, firstName);
        firstName = trim(firstName);
        if (!isAlphaString(firstName)) {
            std::cout << "Invalid input! First Name must contain letters only.\n";
            continue;
        }
        break;
    }

    while (true) {
        std::cout << "Enter Last Name for replacement player (letters only): ";
        getline(std::cin, lastName);
        lastName = trim(lastName);
        if (!isAlphaString(lastName)) {
            std::cout << "Invalid input! Last Name must contain letters only.\n";
            continue;
        }
        break;
    }

    while (true) {
        std::cout << "Enter Registration Type for replacement player (wildcard, earlybird, normal): ";
        getline(std::cin, regType);
        regType = trim(regType);
        if (!isValidRegType(regType)) {
            std::cout << "Invalid Registration Type! Must be 'wildcard', 'earlybird', or 'normal'.\n";
            continue;
        }
        break;
    }

    std::string fullName = firstName + " " + lastName;
    int priority = 3;
    if (regType == "wildcard") priority = 1;
    else if (regType == "earlybird") priority = 2;

    Player newPlayer(selectedId, fullName, getCurrentDateTime(), "Registered", priority, regType);

    pq.insert(newPlayer);

    std::ofstream playersFile("players.txt", std::ios::app);
    if (!playersFile) {
        std::cout << "Error opening players.txt for writing!\n";
    }
    else {
        playersFile << newPlayer.id << "," << newPlayer.name << "," << newPlayer.registrationTime << ","
            << newPlayer.status << "," << newPlayer.regType << "," << newPlayer.priority << "\n";
        if (playersFile.fail()) {
            std::cout << "Error writing to players.txt!\n";
        }
        else {
            std::cout << "Player " << fullName << " has been replaced the withdrawn player successfully and saved to the players list.\n";
        }
        playersFile.close();
        cout << "Press Enter to return to the main menu...";
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard leftover input
        cin.get(); // wait for Enter key

        clearScreen();
    }
    
}

void displayPlayers(const PriorityQueue& pq) {
    cout << "\nCurrent Players in Queue (Priority order):\n";
    cout << "ID | Name | Status | Type | Priority | Registered at\n";
    cout << "----------------------------------------------------\n";
    for (int i = 0; i < pq.size; i++) {
        const Player& p = pq.queue[i];
        cout << p.id << " | " << p.name << " | " << p.status << " | " << p.regType << " | " << p.priority << " | " << p.registrationTime << "\n";
    }
    cout << endl;
    cout << "Press Enter to return to the main menu...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard leftover input
    cin.get(); // wait for Enter key

    clearScreen();
}



