// main.cpp (The Central Hub)
#include <iostream>
#include <limits> // Required for numeric_limits
#include <cstdlib> // Required for system() for clearScreen()

// Include headers that declare your task-specific main functions
// You will need to create these headers if they don't exist,
// or ensure your existing task headers declare these functions.
#include "task_entry_points.hpp" // This header will declare Task1Main(), Task2Main(), etc.

// Function to clear the console screen (cross-platform)
void clearScreen() {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For macOS and Linux
#endif
}

// Function to display the main application menu
void displayOverallMainMenu() {
    std::cout << "\n--- APUEC Championship Management System ---" << std::endl;
    std::cout << "1. Task 1: Match Scheduling & Player Progression" << std::endl;
    std::cout << "2. Task 2: Tournament Registration & Player Queueing" << std::endl;
    std::cout << "3. Task 3: Live Stream & Spectator Queue Management" << std::endl;
    std::cout << "4. Task 4: Game Result Logging & Performance History" << std::endl;
    std::cout << "0. Exit Application" << std::endl;
    std::cout << "Enter your choice: ";
}

int main() {
    clearScreen(); // Clear screen at program start

    int choice;
    do {
        displayOverallMainMenu();
        std::cin >> choice;

        // Input validation
        while (std::cin.fail()) {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cin >> choice;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume the newline character left by std::cin >> choice

        switch (choice) {
        case 1:
            clearScreen();
            std::cout << "Entering Task 1: Match Scheduling & Player Progression...\n";
            Task1Main(); // Call the function for Task 1
            std::cout << "\nExiting Task 1. Returning to main menu.\n";
            break;
        case 2:
            clearScreen();
            std::cout << "Entering Task 2: Tournament Registration & Player Queueing...\n";
            Task2Main(); // Call the function for Task 2
            std::cout << "\nExiting Task 2. Returning to main menu.\n";
            break;
        case 3:
            clearScreen();
            std::cout << "Entering Task 3: Live Stream & Spectator Queue Management...\n";
            Task3Main(); // Call the function for Task 3
            std::cout << "\nExiting Task 3. Returning to main menu.\n";
            break;
        case 4:
            clearScreen();
            std::cout << "Entering Task 4: Game Result Logging & Performance History...\n";
            Task4Main(); // Call the function for Task 4
            std::cout << "\nExiting Task 4. Returning to main menu.\n";
            break;
        case 0:
            std::cout << "Exiting APUEC Championship Management System. Goodbye!" << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            break;
        }

        // Pause and clear screen before displaying the main menu again, unless exiting
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get(); // Wait for user to press Enter
            clearScreen();
        }

    } while (choice != 0);

    return 0; // The single exit point for the entire application
}