#include "player.hpp"
#include "match.hpp"
#include "bracket.hpp"
#include "match_scheduling.hpp"
#include "task_entry_points.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <limits>   // Required for numeric_limits
#include <cstdlib>  // Required for system() function

// Function to clear the console screen


// Function to create a dummy players.txt file for initial testing
void createDummyPlayersFile(const char* filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "1,Alice,2025-01-01 10:00:00,Registered,10\n";
        file << "2,Bob,2025-01-01 10:01:00,Registered,9\n";
        file << "3,Charlie,2025-01-01 10:02:00,Registered,8\n";
        file << "4,David,2025-01-01 10:03:00,Registered,7\n";
        file << "5,Eve,2025-01-01 10:04:00,Registered,6\n";
        file << "6,Frank,2025-01-01 10:05:00,Registered,5\n";
        file << "7,Grace,2025-01-01 10:06:00,Registered,4\n";
        file << "8,Heidi,2025-01-01 10:07:00,Registered,3\n";
        file.close();
        std::cout << "Created dummy players.txt" << std::endl;
    } else {
        std::cerr << "Error creating dummy players.txt" << std::endl;
    }
}

// Function to append a dummy match result to results.txt
void appendDummyResultFile(const char* filename, int matchID, int winnerID) {
    std::ofstream file(filename, std::ios_base::app); // Open in append mode
    if (file.is_open()) {
        // Find a loser that is not the winner for the given matchID
        int loserID = 0;
        if (matchID == 1 && winnerID == 1) loserID = 2;
        else if (matchID == 2 && winnerID == 3) loserID = 4;
        else if (matchID == 3 && winnerID == 5) loserID = 6;
        else if (matchID == 4 && winnerID == 7) loserID = 8;
        // Add more specific loser logic if needed, or make it truly random
        else { // Fallback for other matches if IDs are unknown, just pick a dummy loser
             loserID = winnerID == 1 ? 2 : 1; // Example fallback
        }

        file << matchID << "," << winnerID << "," << loserID << "\n";
        file.close();
        std::cout << "Appended result to results.txt: Match " << matchID << ", Winner " << winnerID << ", Loser " << loserID << std::endl;
    } else {
        std::cerr << "Error appending to results.txt" << std::endl;
    }
}

// Function to clear results.txt
void clearResultsFile(const char* filename) {
    std::ofstream file(filename, std::ios_base::trunc); // Open in truncate mode to clear
    if (file.is_open()) {
        file.close();
        std::cout << "Cleared " << filename << std::endl;
    } else {
        std::cerr << "Error clearing " << filename << std::endl;
    }
}


void displayMenu() {
    std::cout << "\n--- Tournament Management System Menu ---" << std::endl;
    std::cout << "1. Create Dummy Players File (players.txt)" << std::endl;
    std::cout << "2. Load Players from File" << std::endl;
    std::cout << "3. Display All Players" << std::endl;
    std::cout << "4. Generate Group Stage Matches" << std::endl;
    std::cout << "5. Display Group Stage Matches Queue" << std::endl;
    std::cout << "6. Simulate Group Stage Progress (Process Individual Results)" << std::endl;
    std::cout << "7. Run Full Group Stage Simulation" << std::endl;
    std::cout << "8. Generate Knockout Stage Bracket" << std::endl;
    std::cout << "9. Display Knockout Bracket" << std::endl;
    std::cout << "10. Run Knockout Stage Simulation" << std::endl;
    std::cout << "11. Output Current Standings to File" << std::endl;
    std::cout << "12. Output Scheduled Matches to File" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

void Task1Main(){
    MatchScheduler scheduler;
    int choice;
    const char* playersFile = "players.txt";
    const char* resultsFile = "results.txt";
    const char* standingsFile = "current_standings.txt";
    const char* scheduledMatchesFile = "scheduled_matches.txt";

    // Ensure results file is clear at the start
    clearResultsFile(resultsFile);
    clearScreen(); // Clear screen at program start

    do {
        displayMenu();
        std::cin >> choice;

        // Input validation
        while (std::cin.fail()) {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin >> choice;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character

        switch (choice) {
            case 1:
                createDummyPlayersFile(playersFile);
                break;
            case 2:
                scheduler.loadPlayersFromFile(playersFile);
                break;
            case 3:
                scheduler.displayAllPlayers();
                break;
            case 4:
                scheduler.generateGroupStageMatches();
                break;
            case 5:
                scheduler.displayGroupStageQueue();
                break;
            case 6: {
                int matchID, winnerID;
                std::cout << "Enter Match ID to process: ";
                std::cin >> matchID;
                std::cout << "Enter Winner ID for Match " << matchID << ": ";
                std::cin >> winnerID;
                // Clear the results file first to only process the specific new entry
                clearResultsFile(resultsFile);
                appendDummyResultFile(resultsFile, matchID, winnerID);
                scheduler.processMatchResultFile(resultsFile);
                break;
            }
            case 7:
                clearResultsFile(resultsFile); // Clear previous results to ensure full simulation processes unplayed matches
                scheduler.runGroupStageSimulation();
                break;
            case 8:
                scheduler.generateKnockoutStageBracket();
                break;
            case 9:
                scheduler.displayKnockoutBracket();
                break;
            case 10:
                scheduler.runKnockoutStageSimulation();
                break;
            case 11:
                scheduler.outputCurrentStandings(standingsFile);
                break;
            case 12:
                scheduler.outputScheduledMatches(scheduledMatchesFile);
                break;
            case 0:
                std::cout << "Exiting Tournament Management System. Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        
        // Pause briefly to allow user to read output before clearing, then clear screen
        if (choice != 0) { // Don't pause/clear if exiting
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume remaining newline if any
            std::cin.get(); // Wait for user to press Enter
            clearScreen();
        }

    } while (choice != 0);

   
}