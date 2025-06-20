#ifndef PLAYER_HPP
#define PLAYER_HPP

// No need for _CRT_SECURE_NO_WARNINGS if using _s functions
#include <iostream> // For basic input/output like std::cout
#include <cstring>  // For string manipulation functions like strncpy_s, strcmp
#include <string>   // For std::string usage, if preferred over char arrays for some parts

// Define the Player structure (Node for the linked list)
struct Player {
    int playerID;
    char name[50];
    char registrationTime[20]; // YYYY-MM-DD HH:MM:SS format
    char status[20];          // e.g., "Registered", "Playing", "Eliminated", "Advanced", "Winner"
    int priority;             // For initial seeding/ranking
    int groupID;              // To assign players to specific groups for group stage
    int wins;                 // Tracks wins in group stage (or overall)
    int losses;               // Tracks losses in group stage (or overall)
    Player* next;             // Pointer to the next player in the list

    // Constructor for easy initialization
    Player(int id = 0, const char* n = "", const char* regTime = "", const char* s = "", int p = 0, int g = 0)
        : playerID(id), priority(p), groupID(g), wins(0), losses(0), next(nullptr) {
        // Use strncpy_s instead of strncpy
        strncpy_s(name, sizeof(name), n, sizeof(name) - 1);
        strncpy_s(registrationTime, sizeof(registrationTime), regTime, sizeof(registrationTime) - 1);
        strncpy_s(status, sizeof(status), s, sizeof(status) - 1);
    }
};

// Define the custom Singly Linked List for Players
class PlayerList {
public:
    Player* head; // Pointer to the first player in the list
    int playerCount; // Number of players in the list

    // Constructor
    PlayerList() : head(nullptr), playerCount(0) {}

    // Destructor to free dynamically allocated Player nodes
    ~PlayerList() {
        Player* current = head;
        while (current != nullptr) {
            Player* nextPlayer = current->next;
            delete current;
            current = nextPlayer;
        }
        head = nullptr; // Ensure head is null after deletion
    }

    // Adds a new Player node to the end of the list
    void addPlayer(Player* newPlayer) {
        if (newPlayer == nullptr) return;

        if (head == nullptr) {
            head = newPlayer;
        }
        else {
            Player* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newPlayer;
        }
        playerCount++;
    }

    // Retrieves a Player by their ID
    Player* getPlayerByID(int id) {
        Player* current = head;
        while (current != nullptr) {
            if (current->playerID == id) {
                return current;
            }
            current = current->next;
        }
        return nullptr; // Player not found
    }

    // Updates the status of a Player
    void updatePlayerStatus(int id, const char* newStatus) {
        Player* player = getPlayerByID(id);
        if (player != nullptr) {
            // Use strncpy_s instead of strncpy
            strncpy_s(player->status, sizeof(player->status), newStatus, sizeof(player->status) - 1);
        }
        else {
            std::cerr << "Warning: Player with ID " << id << " not found for status update." << std::endl;
        }
    }

    // Returns the total number of players
    int getPlayerCount() const {
        return playerCount;
    }

    // Simple Bubble Sort implementation to sort players by their group stage performance
    // (e.g., by wins, then by losses, then by priority for tie-breaking)
    void sortPlayersByRanking() {
        if (head == nullptr || head->next == nullptr) return; // 0 or 1 player, already sorted

        bool swapped;
        Player* current;
        Player* last = nullptr; // Tracks the last sorted element

        do {
            swapped = false;
            current = head;
            while (current->next != last) {
                // Example sorting criteria: higher wins, then lower losses
                bool shouldSwap = false;
                if (current->wins < current->next->wins) {
                    shouldSwap = true;
                }
                else if (current->wins == current->next->wins && current->losses > current->next->losses) {
                    shouldSwap = true;
                }
                else if (current->wins == current->next->wins && current->losses == current->next->losses && current->priority < current->next->priority) {
                    shouldSwap = true; // Use priority as tie-breaker
                }

                if (shouldSwap) {
                    // Swap data of current and current->next nodes
                    // Use strcpy_s instead of strcpy for character arrays
                    int tempID = current->playerID; current->playerID = current->next->playerID; current->next->playerID = tempID;
                    char tempName[50]; strcpy_s(tempName, sizeof(tempName), current->name); strcpy_s(current->name, sizeof(current->name), current->next->name); strcpy_s(current->next->name, sizeof(current->next->name), tempName);
                    char tempRegTime[20]; strcpy_s(tempRegTime, sizeof(tempRegTime), current->registrationTime); strcpy_s(current->registrationTime, sizeof(current->registrationTime), current->next->registrationTime); strcpy_s(current->next->registrationTime, sizeof(current->next->registrationTime), tempRegTime);
                    char tempStatus[20]; strcpy_s(tempStatus, sizeof(tempStatus), current->status); strcpy_s(current->status, sizeof(current->status), current->next->status); strcpy_s(current->next->status, sizeof(current->next->status), tempStatus);
                    int tempPriority = current->priority; current->priority = current->next->priority; current->next->priority = tempPriority;
                    int tempGroup = current->groupID; current->groupID = current->next->groupID; current->next->groupID = tempGroup;
                    int tempWins = current->wins; current->wins = current->next->wins; current->next->wins = tempWins;
                    int tempLosses = current->losses; current->losses = current->next->losses; current->next->losses = tempLosses;

                    swapped = true;
                }
                current = current->next;
            }
            last = current; // After each pass, the last element is sorted
        } while (swapped);
    }

    // Prints details of all players (for debugging/display)
    void printAllPlayers() {
        if (head == nullptr) {
            std::cout << "Player List is empty." << std::endl;
            return;
        }
        std::cout << "--- Player List (" << playerCount << " players) ---" << std::endl;
        Player* current = head;
        while (current != nullptr) {
            std::cout << "ID: " << current->playerID
                << ", Name: " << current->name
                << ", RegTime: " << current->registrationTime
                << ", Status: " << current->status
                << ", Priority: " << current->priority
                << ", Group: " << current->groupID
                << ", W/L: " << current->wins << "/" << current->losses << std::endl;
            current = current->next;
        }
        std::cout << "------------------------------------" << std::endl;
    }
};

#endif // PLAYER_HPP