
#include <iostream>
#include "game_result.hpp"
using namespace std;

MatchResult matchLog[100];
int matchCount = 0;

void Task4Main() {
    loadResultsFromFile(matchLog, matchCount, "results.txt");

    int choice;
    do {
        cout << "\n1. Add Match Result\n2. Display All Results\n3. Search Player History\n4. Save & Exit\nChoice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            addMatchResult(matchLog, matchCount);
        } else if (choice == 2) {
            displayResults(matchLog, matchCount);
        } else if (choice == 3) {
            char playerName[30];
            cout << "Enter player name to search: ";
            cin.getline(playerName, 30);
            searchPlayerHistory(matchLog, matchCount, playerName);
        } else if (choice == 4) {
            saveResultsToFile(matchLog, matchCount, "results.txt");
        }
    } while (choice != 4);

    
}
