#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <direct.h>
using namespace std;

// Forward declarations
char getYesNoInput();
string getNonEmptyStringInput();

struct SpectatorNode {
    string spectatorId;
    string name;
    string userType;
    int priority;
    string status;
    string checkInTime;
    SpectatorNode* next;

    SpectatorNode(string id, string n, string type, int p, string stat, string time)
        : spectatorId(id), name(n), userType(type), priority(p), status(stat), checkInTime(time), next(nullptr) {}
};

struct ViewingSlot {
    string slotId;
    string spectatorId;
    string status;
    ViewingSlot* next;

    ViewingSlot(string id, string spId, string stat) : slotId(id), spectatorId(spId), status(stat), next(nullptr) {}
};

class SpectatorQueueManager {
private:
    SpectatorNode* front;
    SpectatorNode* rear;
    int size;
    int maxCapacity;

    ViewingSlot* slotFront;
    int slotCount;

    const string filepath = "LIVESTREAMING.txt";

public:
    SpectatorQueueManager(int capacity = 50) : front(nullptr), rear(nullptr), size(0), maxCapacity(capacity),
        slotFront(nullptr), slotCount(0) {}

    ~SpectatorQueueManager() {
        // 1) Delete all SpectatorNode instances without saving to file
        if (front) {
            SpectatorNode* curr = front;
            SpectatorNode* start = front;
            do {
                SpectatorNode* temp = curr;
                curr = curr->next;
                delete temp;
            } while (curr != start);
        }
        front = rear = nullptr;
        size = 0;

        // 2) Delete all ViewingSlot instances exactly as before
        removeAllSlots();
    }

    void removeAllSlots() {
        if (!slotFront) return;
        ViewingSlot* curr = slotFront->next;
        while (curr != slotFront) {
            ViewingSlot* temp = curr;
            curr = curr->next;
            delete temp;
        }
        delete slotFront;
        slotFront = nullptr;
        slotCount = 0;
    }

    void loadSpectators() {
        ifstream file(filepath);
        if (!file.is_open()) {
            cout << "Could not open " << filepath << " to load spectators.\n";
            return;
        }

        string line;
        bool inSpectatorSection = false;

        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            if (line.empty()) continue;

            if (line.find("# Spectators Data") != string::npos) {
                inSpectatorSection = true;
                continue;
            }
            if (line.find("# Viewing Slots") != string::npos) break;

            if (inSpectatorSection) {
                if (line.find(',') == string::npos) continue;

                stringstream ss(line);
                string id, name, type, priorityStr, status, checkInTime;
                getline(ss, id, ',');
                getline(ss, name, ',');
                getline(ss, type, ',');
                getline(ss, priorityStr, ',');
                getline(ss, status, ',');
                getline(ss, checkInTime, ',');

                auto trim = [](string& s) {
                    s.erase(0, s.find_first_not_of(" \t"));
                    s.erase(s.find_last_not_of(" \t") + 1);
                    };

                trim(id); trim(name); trim(type); trim(priorityStr); trim(status); trim(checkInTime);

                if (findSpectatorById(id) != nullptr) {
                    continue; // skip duplicates
                }

                int priority = stoi(priorityStr);

                SpectatorNode* newNode = new SpectatorNode(id, name, type, priority, status, checkInTime);

                if (!front) {
                    front = rear = newNode;
                    newNode->next = front;
                }
                else {
                    rear->next = newNode;
                    newNode->next = front;
                    rear = newNode;
                }
                size++;
            }
        }
        file.close();
    }

    void loadViewingSlots() {
        ifstream file(filepath.c_str());
        if (!file.is_open()) {
            cout << "Could not open " << filepath << " to load viewing slots.\n";
            return;
        }
        string line;
        bool inSlotSection = false;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            if (line.empty()) continue;
            if (line.find("# Viewing Slots") != string::npos || line.find("Viewing Slots") != string::npos) {
                inSlotSection = true;
                continue;
            }
            if (!inSlotSection) continue;

            if (line.find(',') == string::npos) continue;

            stringstream ss(line);
            string slotId, spId, status;
            getline(ss, slotId, ',');
            getline(ss, spId, ',');
            getline(ss, status, ',');

            auto trim = [](string& s) { s.erase(0, s.find_first_not_of(" \t")); s.erase(s.find_last_not_of(" \t") + 1); };
            trim(slotId); trim(spId); trim(status);

            addViewingSlot(slotId, spId, status);
        }
        file.close();
    }

    void addViewingSlot(const string& id, const string& spId, const string& stat) {
        ViewingSlot* newSlot = new ViewingSlot(id, spId, stat);
        if (!slotFront) {
            slotFront = newSlot;
            newSlot->next = slotFront;
        }
        else {
            ViewingSlot* tail = slotFront;
            while (tail->next != slotFront) tail = tail->next;
            tail->next = newSlot;
            newSlot->next = slotFront;
        }
        slotCount++;
    }

    bool isValidSpectatorId(const string& id) {
        if (id.size() < 3) return false;
        if (id[0] != 'S' || id[1] != 'P') return false;
        for (size_t i = 2; i < id.size(); ++i)
            if (!isdigit(id[i])) return false;
        return true;
    }

    bool isValidTimeFormat(const string& time) {
        if (time.size() != 5 || time[2] != ':') return false;
        string h = time.substr(0, 2);
        string m = time.substr(3, 2);
        if (!isdigit(h[0]) || !isdigit(h[1]) || !isdigit(m[0]) || !isdigit(m[1])) return false;
        int hour = stoi(h);
        int minute = stoi(m);
        return hour >= 0 && hour < 24 && minute >= 0 && minute < 60;
    }

    bool isNonEmptyString(const string& s) {
        return s.find_first_not_of(' ') != string::npos;
    }

    void addSpectator(string id, string name, string userType, string time) {
        if (size >= maxCapacity) {
            cout << "Queue is full. Cannot add more spectators.\n";
            return;
        }

        if (!isValidSpectatorId(id)) {
            cout << "Invalid Spectator ID format. Must start with 'SP' followed by digits (e.g., SP001).\n";
            return;
        }
        if (findSpectatorById(id)) {
            cout << "Spectator ID already exists in queue. Please enter a unique ID.\n";
            return;
        }

        if (!isNonEmptyString(name)) {
            cout << "Name cannot be empty or just spaces.\n";
            return;
        }

        if (!isValidTimeFormat(time)) {
            cout << "Invalid time format. Please enter in HH:MM 24-hour format.\n";
            return;
        }

        int priority = (userType == "VIP") ? 3 :
            (userType == "Influencer") ? 2 : 1;

        SpectatorNode* newNode = new SpectatorNode(id, name, userType, priority, "Waiting", time);

        if (!front) {
            front = rear = newNode;
            newNode->next = front;
        }
        else {
            rear->next = newNode;
            newNode->next = front;
            rear = newNode;
        }
        size++;

        cout << userType << " " << name << " (ID: " << id << ") added to queue.\n";

        saveSpectatorsToFile();  // <-- Changed here: replaced appendSpectatorToFile() with full save
    }

    void removeSpectator(bool prompt = true) {
        if (!front) {
            cout << "Queue is empty.\n";
            return;
        }
        if (prompt) {
            cout << "Are you sure you want to remove the next spectator from queue? (Y/N): ";
            char confirm = getYesNoInput();
            if (confirm != 'Y') {
                cout << "Remove cancelled.\n";
                return;
            }
        }
        SpectatorNode* temp = front;
        if (front == rear) {
            front = rear = nullptr;
        }
        else {
            front = front->next;
            rear->next = front;
        }
        cout << "Removed spectator: " << temp->name << " (ID: " << temp->spectatorId << ")\n";
        delete temp;
        size--;

        saveSpectatorsToFile();
    }

    void saveSpectatorsToFile() {
        ifstream inFile(filepath);
        if (!inFile.is_open()) {
            cout << "Error opening file for reading: " << filepath << "\n";
            return;
        }

        ofstream outFile("LIVESTREAMING_temp.txt");
        if (!outFile.is_open()) {
            cout << "Error opening temp file for writing.\n";
            inFile.close();
            return;
        }

        string line;
        bool inSpectatorSection = false;

        while (getline(inFile, line)) {
            if (line.find("# Spectators Data") != string::npos) {
                outFile << line << "\n";
                inSpectatorSection = true;

                if (front) {
                    SpectatorNode* curr = front;
                    do {
                        outFile << curr->spectatorId << ", " << curr->name << ", "
                            << curr->userType << ", " << curr->priority << ", "
                            << curr->status << ", " << curr->checkInTime << "\n";
                        curr = curr->next;
                    } while (curr != front);
                }

                while (getline(inFile, line)) {
                    size_t pos = line.find_first_not_of(" \t");
                    if (line.empty() || pos == string::npos || line[pos] == '#') {
                        outFile << line << "\n";
                        break;
                    }
                }

                while (getline(inFile, line)) {
                    outFile << line << "\n";
                }

                inSpectatorSection = false;
                break;
            }
            else {
                outFile << line << "\n";
            }
        }

        inFile.close();
        outFile.close();

        if (remove(filepath.c_str()) != 0) {
            cout << "Error deleting original file.\n";
            return;
        }
        if (rename("LIVESTREAMING_temp.txt", filepath.c_str()) != 0) {
            cout << "Error renaming temp file.\n";
            return;
        }

        cout << "Spectators saved to file successfully.\n";
    }


    void displayQueue() {
        if (!front) {
            cout << "Queue is empty.\n";
            return;
        }
        SpectatorNode* curr = front;
        int pos = 1;
        bool foundWaiting = false;
        cout << "\n=== Current Spectator Queue (Waiting Only) ===\n";
        cout << left << setw(4) << "No" << setw(10) << "ID"
            << setw(15) << "Name" << setw(12) << "Type"
            << setw(10) << "Priority" << setw(10) << "Status"
            << "Check-in Time\n";
        cout << string(70, '-') << "\n";

        do {
            if (curr->status == "Waiting") {
                cout << left << setw(4) << pos++
                    << setw(10) << curr->spectatorId
                    << setw(15) << curr->name
                    << setw(12) << curr->userType
                    << setw(10) << curr->priority
                    << setw(10) << curr->status
                    << curr->checkInTime << "\n";
                foundWaiting = true;
            }
            curr = curr->next;
        } while (curr != front);

        if (!foundWaiting) {
            cout << "No spectators currently waiting in the queue.\n";
        }
    }

    void displayAllUsers() {
        ifstream file(filepath.c_str());
        if (!file.is_open()) {
            cout << "Could not open " << filepath << "\n";
            char cwd[1024];
            if (_getcwd(cwd, sizeof(cwd)))
                cout << "Working directory: " << cwd << "\n";
            return;
        }
        string line;
        int count = 0;
        cout << "\n=== Registered Spectators From File ===\n";
        cout << left << setw(10) << "ID" << setw(15) << "Name"
            << setw(12) << "Type" << setw(10) << "Priority"
            << setw(10) << "Status" << "Check-in Time\n";
        cout << string(70, '-') << "\n";

        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            if (line.empty() || line[0] == '#' || line.find(',') == string::npos) continue;
            if (line.find("Viewing") != string::npos) break;

            int commas = 0;
            for (char c : line) if (c == ',') commas++;
            if (commas != 5) continue;

            stringstream ss(line);
            string id, name, type, priority, status, time;
            getline(ss, id, ','); getline(ss, name, ','); getline(ss, type, ',');
            getline(ss, priority, ','); getline(ss, status, ','); getline(ss, time, ',');

            auto trim = [](string& s) { s.erase(0, s.find_first_not_of(" \t")); s.erase(s.find_last_not_of(" \t") + 1); };
            trim(id); trim(name); trim(type); trim(priority); trim(status); trim(time);

            cout << left << setw(10) << id << setw(15) << name
                << setw(12) << type << setw(10) << priority
                << setw(10) << status << time << "\n";
            count++;
        }
        cout << string(70, '-') << "\n";
        cout << "Total spectators: " << count << "\n";
    }

    SpectatorNode* findSpectatorById(const string& id) {
        if (!front) return nullptr;
        SpectatorNode* curr = front;
        do {
            if (curr->spectatorId == id) return curr;
            curr = curr->next;
        } while (curr != front);
        return nullptr;
    }

    ViewingSlot* findSlotById(const string& id) {
        if (!slotFront) return nullptr;
        ViewingSlot* curr = slotFront;
        do {
            if (curr->slotId == id) return curr;
            curr = curr->next;
        } while (curr != slotFront);
        return nullptr;
    }

    void assignSeating() {
        if (!front) {
            cout << "Queue is empty. No spectators to assign seating.\n";
            return;
        }
        cout << "\n=== Assign Seating for VIPs and Influencers ===\n";

        SpectatorNode* curr = front;
        bool foundAny = false;
        int pos = 1;
        do {
            if ((curr->userType == "VIP" || curr->userType == "Influencer") && curr->status == "Waiting") {
                foundAny = true;
                cout << pos << ". " << curr->name << " (ID: " << curr->spectatorId << "), Status: " << curr->status << "\n";
            }
            curr = curr->next;
            pos++;
        } while (curr != front);

        if (!foundAny) {
            cout << "No VIP or Influencer spectators waiting to be seated.\n";
            return;
        }

        cout << "\nAvailable Viewing Slots:\n";
        ViewingSlot* slot = slotFront;
        int slotIndex = 1;
        bool anyAvailable = false;
        do {
            if (slot->status == "Available") {
                cout << slotIndex << ". " << slot->slotId << " (Available)\n";
                anyAvailable = true;
            }
            slot = slot->next;
            slotIndex++;
        } while (slot != slotFront);

        if (!anyAvailable) {
            cout << "No available viewing slots.\n";
            return;
        }

        cout << "Enter Spectator ID to assign seating: ";
        string spId = getNonEmptyStringInput();

        SpectatorNode* spectator = findSpectatorById(spId);
        if (!spectator) {
            cout << "Spectator with ID " << spId << " not found or not eligible.\n";
            return;
        }
        if (!(spectator->userType == "VIP" || spectator->userType == "Influencer")) {
            cout << "Spectator is not VIP or Influencer.\n";
            return;
        }
        if (spectator->status != "Waiting") {
            cout << "Spectator is not waiting for seating.\n";
            return;
        }

        cout << "Enter Viewing Slot ID to assign (e.g., SL01): ";
        string slotId = getNonEmptyStringInput();

        ViewingSlot* selectedSlot = findSlotById(slotId);
        if (!selectedSlot) {
            cout << "Invalid slot ID.\n";
            return;
        }
        if (selectedSlot->status == "Occupied") {
            cout << "Slot already occupied.\n";
            return;
        }

        // Assign seat and update statuses
        selectedSlot->spectatorId = spectator->spectatorId;
        selectedSlot->status = "Occupied";
        spectator->status = "Seated";

        cout << "Assigned spectator " << spectator->name << " to slot " << selectedSlot->slotId << "\n";

        saveSpectatorsToFile();
        saveViewingSlotsToFile();
    }

    void manageOverflow() {
        if (!front) {
            cout << "Queue is empty.\n";
            return;
        }
        cout << "\n=== Manage Overflow Audience ===\n";

        SpectatorNode* curr = front;
        int count = 0;
        do {
            if (curr->status == "Overflow") {
                cout << "- " << curr->name << " (ID: " << curr->spectatorId << ")\n";
                count++;
            }
            curr = curr->next;
        } while (curr != front);

        if (count == 0) {
            cout << "No spectators in overflow.\n";
            return;
        }

        cout << "\nEnter Spectator ID to move from Overflow to Waiting (or type 'none' to cancel): ";
        string spId;
        while (true) {
            getline(cin, spId);
            if (spId == "none") return;
            if (!isValidSpectatorId(spId)) {
                cout << "Invalid format. Please enter valid Spectator ID (e.g., SP001) or 'none': ";
                continue;
            }
            SpectatorNode* spectator = findSpectatorById(spId);
            if (!spectator || spectator->status != "Overflow") {
                cout << "Spectator ID not found in overflow or invalid. Try again or type 'none': ";
                continue;
            }
            if (size >= maxCapacity) {
                cout << "Queue is full. Cannot move spectator to waiting.\n";
                return;
            }
            spectator->status = "Waiting";
            cout << "Moved " << spectator->name << " from Overflow to Waiting.\n";
            break;
        }
        saveSpectatorsToFile();
    }

    void organizeViewingSlots() {
        cout << "\n=== Organize Viewing Slots ===\n";
        cout << "Current Viewing Slots:\n";
        displayViewingSlots();

        cout << "\nTo assign or free slots, please use 'Assign Seating' option.\n";
    }

    void viewViewingSlots() {
        cout << "\n=== Viewing Slots ===\n";
        displayViewingSlots();
    }

    void displayViewingSlots() {
        if (!slotFront) {
            cout << "No viewing slots loaded.\n";
            return;
        }
        ViewingSlot* curr = slotFront;
        int pos = 1;
        cout << left << setw(6) << "No" << setw(8) << "SlotID" << setw(12) << "SpectatorID" << "Status\n";
        cout << string(40, '-') << "\n";
        do {
            cout << left << setw(6) << pos++
                << setw(8) << curr->slotId
                << setw(12) << curr->spectatorId
                << curr->status << "\n";
            curr = curr->next;
        } while (curr != slotFront);
    }

    void saveViewingSlotsToFile() {
        ifstream inFile(filepath.c_str());
        if (!inFile.is_open()) {
            cout << "Cannot open " << filepath << " to save viewing slots.\n";
            return;
        }

        ofstream outFile("LIVESTREAMING_temp.txt");
        if (!outFile.is_open()) {
            cout << "Cannot create temporary file to save viewing slots.\n";
            inFile.close();
            return;
        }

        string line;
        bool inViewingSlotsSection = false;

        while (getline(inFile, line)) {
            if (line.find("# Viewing Slots") != string::npos) {
                outFile << line << "\n";
                inViewingSlotsSection = true;

                if (slotFront) {
                    ViewingSlot* curr = slotFront;
                    do {
                        outFile << curr->slotId << ", " << curr->spectatorId << ", " << curr->status << "\n";
                        curr = curr->next;
                    } while (curr != slotFront);
                }

                while (getline(inFile, line)) {
                    if (line.empty() || line[0] == '#') {
                        outFile << line << "\n";
                        break;
                    }
                }

                while (getline(inFile, line)) {
                    outFile << line << "\n";
                }

                inViewingSlotsSection = false;
                break;
            }
            else {
                outFile << line << "\n";
            }
        }

        inFile.close();
        outFile.close();

        if (remove(filepath.c_str()) != 0) {
            cout << "Error deleting original file.\n";
            return;
        }
        if (rename("LIVESTREAMING_temp.txt", filepath.c_str()) != 0) {
            cout << "Error renaming temp file.\n";
            return;
        }

        cout << "Viewing slots saved to file successfully.\n";
    }
};

int getValidatedMenuChoice(int minOption, int maxOption) {
    int choice;
    while (true) {
        string input;
        getline(cin, input);
        try {
            choice = stoi(input);
            if (choice >= minOption && choice <= maxOption)
                break;
            else
                cout << "Please enter a number between " << minOption << " and " << maxOption << ": ";
        }
        catch (...) {
            cout << "Invalid input. Please enter a number: ";
        }
    }
    return choice;
}

void manageSpectatorsMenu(SpectatorQueueManager& manager) {
    int choice;
    do {
        cout << "\n--- Manage Spectators ---\n\n";
        cout << "1. Add New Spectator\n";
        cout << "2. Assign Seating (VIP / Influencer)\n";
        cout << "3. Manage Overflow Audience\n";
        cout << "4. Back to Main Menu\n";
        cout << "\nEnter your choice (1-4): ";

        choice = getValidatedMenuChoice(1, 4);

        switch (choice) {
        case 1: {
            cout << "\n--- Add New Spectator ---\n";
            cout << "Select Type:\n1. VIP\n2. Influencer\n3. General\n";
            cout << "Enter your choice (1-3): ";
            int typeChoice = getValidatedMenuChoice(1, 3);

            string type;
            if (typeChoice == 1) type = "VIP";
            else if (typeChoice == 2) type = "Influencer";
            else type = "General";

            cout << "Enter Spectator ID (e.g., SP001): ";
            string id = getNonEmptyStringInput();
            cout << "Enter Name: ";
            string name = getNonEmptyStringInput();
            cout << "Enter Check-in Time (e.g., 09:30): ";
            string time = getNonEmptyStringInput();

            manager.addSpectator(id, name, type, time);
            break;
        }
        case 2:
            manager.assignSeating();
            break;
        case 3:
            manager.manageOverflow();
            break;
        case 4:
            cout << "Returning to Main Menu...\n";
            break;
        }
    } while (choice != 4);
}

void viewQueueSeatsMenu(SpectatorQueueManager& manager) {
    int choice;
    do {
        cout << "\n--- View Queue & Seats ---\n\n";
        cout << "1. Display Current Queue\n";
        cout << "2. Remove Next Spectator from Queue\n";
        cout << "3. Display Registered Spectators (from file)\n";
        cout << "4. Back to Main Menu\n";
        cout << "\nEnter your choice (1-4): ";

        choice = getValidatedMenuChoice(1, 4);

        switch (choice) {
        case 1:
            manager.displayQueue();
            break;
        case 2:
            manager.removeSpectator();
            break;
        case 3:
            manager.displayAllUsers();
            break;
        case 4:
            cout << "Returning to Main Menu...\n";
            break;
        }
    } while (choice != 4);
}

void viewingSlotsMenu(SpectatorQueueManager& manager) {
    int choice;
    do {
        cout << "\n--- Viewing Slots ---\n\n";
        cout << "1. Organize Viewing Slots\n";
        cout << "2. View Viewing Slots\n";
        cout << "3. Back to Main Menu\n";
        cout << "\nEnter your choice (1-3): ";

        choice = getValidatedMenuChoice(1, 3);

        switch (choice) {
        case 1:
            manager.organizeViewingSlots();
            break;
        case 2:
            manager.viewViewingSlots();
            break;
        case 3:
            cout << "Returning to Main Menu...\n";
            break;
        }
    } while (choice != 3);
}

void displayMainMenu() {
    cout << "\n=== APUEC Live Stream Menu ===\n\n";
    cout << "1. Manage Spectators\n";
    cout << "2. View Queue & Seats\n";
    cout << "3. Viewing Slots\n";
    cout << "4. Exit\n";
    cout << "\nEnter your choice (1-4): ";
}

void Task3Main() {
    SpectatorQueueManager manager;
    manager.loadSpectators();
    manager.loadViewingSlots();

    int choice;
    do {
        displayMainMenu();
        choice = getValidatedMenuChoice(1, 4);

        switch (choice) {
        case 1:
            manageSpectatorsMenu(manager);
            break;
        case 2:
            viewQueueSeatsMenu(manager);
            break;
        case 3:
            viewingSlotsMenu(manager);
            break;
        case 4:
            cout << "Returning to main menu...\n";
            break;
        default:
            std::cout << "Invalid Choice. Please try again.\n";
        }
    } while (choice!=4);

    
}

char getYesNoInput() {
    while (true) {
        string input;
        getline(cin, input);
        if (input.size() == 1) {
            char c = toupper(input[0]);
            if (c == 'Y' || c == 'N') return c;
        }
        cout << "Please enter Y or N: ";
    }
}

string getNonEmptyStringInput() {
    string input;
    while (true) {
        getline(cin, input);
        if (input.find_first_not_of(' ') != string::npos) {
            return input;
        }
        cout << "Input cannot be empty or spaces only. Please enter again: ";
    }
}
