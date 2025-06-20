
#include <iostream>
#include <fstream>
#include <cstring>
#include "game_result.hpp"
using namespace std;

void addMatchResult(MatchResult results[], int &count) {
    cout << "\nEnter Match ID: ";
    cin >> results[count].matchID;
    cout << "Enter Player 1: ";
    cin >> results[count].player1;
    cout << "Enter Player 2: ";
    cin >> results[count].player2;
    cout << "Enter Score for " << results[count].player1 << ": ";
    cin >> results[count].score1;
    cout << "Enter Score for " << results[count].player2 << ": ";
    cin >> results[count].score2;

    if (results[count].score1 > results[count].score2)
        strcpy_s(results[count].winner, results[count].player1);
    else
        strcpy_s(results[count].winner, results[count].player2);

    count++;
    cout << "Match recorded successfully.\n";
}

void saveResultsToFile(MatchResult results[], int count, const char* filename) {
    ofstream fout(filename);
    for (int i = 0; i < count; i++) {
        fout << results[i].matchID << ","
             << results[i].player1 << ","
             << results[i].player2 << ","
             << results[i].score1 << ","
             << results[i].score2 << ","
             << results[i].winner << "\n";
    }
    fout.close();
}

void loadResultsFromFile(MatchResult results[], int &count, const char* filename) {
    ifstream fin(filename);
    count = 0;
    while (fin.getline(results[count].matchID, 10, ',')) {
        fin.getline(results[count].player1, 30, ',');
        fin.getline(results[count].player2, 30, ',');
        fin >> results[count].score1;
        fin.ignore(); // skip comma
        fin >> results[count].score2;
        fin.ignore(); // skip comma
        fin.getline(results[count].winner, 30);
        count++;
    }
    fin.close();
}

void displayResults(MatchResult results[], int count) {
    cout << "\n--- MATCH RESULTS ---\n";
    for (int i = 0; i < count; i++) {
        cout << results[i].matchID << ": "
             << results[i].player1 << " vs " << results[i].player2
             << " | " << results[i].score1 << "-" << results[i].score2
             << " | Winner: " << results[i].winner << "\n";
    }
}

void searchPlayerHistory(MatchResult results[], int count, const char* playerName) {
    cout << "\n--- MATCH HISTORY FOR PLAYER: " << playerName << " ---\n";
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (strcmp(results[i].player1, playerName) == 0 ||
            strcmp(results[i].player2, playerName) == 0) {
            cout << results[i].matchID << ": "
                 << results[i].player1 << " vs " << results[i].player2
                 << " | " << results[i].score1 << "-" << results[i].score2
                 << " | Winner: " << results[i].winner << "\n";
            found = true;
        }
    }
    if (!found) {
        cout << "No matches found for player.\n";
    }
}
