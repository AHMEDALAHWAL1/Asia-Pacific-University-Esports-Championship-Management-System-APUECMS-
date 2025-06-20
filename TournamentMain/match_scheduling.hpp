#ifndef MATCH_SCHEDULING_HPP
#define MATCH_SCHEDULING_HPP

// Include custom data structure headers
#include "player.hpp"
#include "match.hpp"
#include "bracket.hpp"

// For file operations (fstream) and basic string/io (iostream, string)
#include <fstream>
#include <iostream>
#include <string>

class MatchScheduler {
private:
    PlayerList allPlayers;          // Stores all registered players
    MatchQueue groupStageMatches;   // Queue for group stage matches
    TournamentBracket knockoutBracket; // Tree for knockout stage matches
    int nextMatchID;                // Counter for unique match IDs (across stages)

    // Private helper functions for internal logic and file writing
    void updatePlayerStatsAndStatus(int winnerPlayerID, int loserPlayerID); // Updates player wins/losses/status
    void writeScheduledMatchesToFile(const char* filename); // Writes current scheduled matches to a file
    void writeCurrentStandingsToFile(const char* filename); // Writes current player standings to a file

public:
    // Constructor and Destructor
    MatchScheduler();
    ~MatchScheduler();

    // === Public Interface for File-Based Interaction ===

    // Input functions: Read data from external files
    void loadPlayersFromFile(const char* players_filename); // Reads initial player data from players.txt
    void processMatchResultFile(const char* results_filename); // Reads new match outcomes from results.txt

    // Core Task 1 functionalities: Generate matches based on internal state
    void generateGroupStageMatches();      // Creates and schedules group stage matches
    void generateKnockoutStageBracket();   // Builds the knockout bracket based on group stage results

    // Output functions: Write current state to external files
    void outputScheduledMatches(const char* output_filename); // Writes matches currently ready to be played
    void outputCurrentStandings(const char* output_filename); // Writes the latest player standings/status

    // Simulation/Execution functions (if this task also "plays" matches internally for testing)
    // These functions would simulate match outcomes and then call processMatchResult internally
    void runGroupStageSimulation();
    void runKnockoutStageSimulation();

    // Display functions (for debugging and demonstration purposes)
    void displayAllPlayers();
    void displayGroupStageQueue();
    void displayKnockoutBracket();
};

#endif // TASK1_MATCH_SCHEDULING_HPP