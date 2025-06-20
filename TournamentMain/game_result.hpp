
#ifndef GAME_RESULT_HPP
#define GAME_RESULT_HPP

struct MatchResult {
    char matchID[10];
    char player1[30];
    char player2[30];
    int score1;
    int score2;
    char winner[30];
};

void addMatchResult(MatchResult results[], int &count);
void saveResultsToFile(MatchResult results[], int count, const char* filename);
void loadResultsFromFile(MatchResult results[], int &count, const char* filename);
void displayResults(MatchResult results[], int count);
void searchPlayerHistory(MatchResult results[], int count, const char* playerName);

#endif
