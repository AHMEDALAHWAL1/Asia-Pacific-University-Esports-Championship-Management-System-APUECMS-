#include "match_scheduling.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream> // For stringstream to parse lines
#include <vector>  // Temporarily use for processedMatchIDs

// --- Helper functions for parsing lines ---
// Simple parsing for players.txt: PlayerID, Name, RegistrationTime, Status, Priority
bool parsePlayerLine(const std::string& line, int& id, char* name, char* regTime, char* status, int& priority) {
    std::stringstream ss(line);
    std::string segment;
    int current_segment = 0;

    // Use getline with ',' as delimiter
    while (std::getline(ss, segment, ',')) {
        switch (current_segment) {
        case 0: id = std::stoi(segment); break;
        case 1: strncpy_s(name, 50, segment.c_str(), _TRUNCATE); break; // Use _TRUNCATE for automatic truncation
        case 2: strncpy_s(regTime, 20, segment.c_str(), _TRUNCATE); break;
        case 3: strncpy_s(status, 20, segment.c_str(), _TRUNCATE); break;
        case 4: priority = std::stoi(segment); break;
        default: return false; // Too many segments
        }
        current_segment++;
    }
    return current_segment == 5; // Ensure all 5 segments were parsed
}

// Simple parsing for results.txt: MatchID, WinnerID, LoserID
bool parseResultLine(const std::string& line, int& matchID, int& winnerID, int& loserID) {
    std::stringstream ss(line);
    std::string segment;
    int current_segment = 0;

    while (std::getline(ss, segment, ',')) {
        switch (current_segment) {
        case 0: matchID = std::stoi(segment); break;
        case 1: winnerID = std::stoi(segment); break;
        case 2: loserID = std::stoi(segment); break;
        default: return false;
        }
        current_segment++;
    }
    return current_segment == 3;
}


// --- MatchScheduler Class Implementation ---

// Constructor
MatchScheduler::MatchScheduler() : nextMatchID(1) {
    // Constructor initializes internal data structures (done by their constructors)
    // nextMatchID ensures unique match IDs across all stages
}

// Destructor
MatchScheduler::~MatchScheduler() {
    // Destructors for allPlayers, groupStageMatches, knockoutBracket handle memory cleanup
}

// === Public Interface Implementations ===

// Reads player data from the specified file and populates the internal PlayerList.
void MatchScheduler::loadPlayersFromFile(const char* players_filename) {
    std::ifstream file(players_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open players file: " << players_filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        int id, priority;
        char name[50], regTime[20], status[20];
        if (parsePlayerLine(line, id, name, regTime, status, priority)) {
            Player* newPlayer = new Player(id, name, regTime, status, priority, 0); // Group 0 for now
            allPlayers.addPlayer(newPlayer);
        }
        else {
            std::cerr << "Warning: Failed to parse player line: " << line << std::endl;
        }
    }
    file.close();
    std::cout << "Loaded " << allPlayers.getPlayerCount() << " players from " << players_filename << std::endl;
}

// Processes new match results from the specified file.
// This function should be called periodically by an external loop.
// It assumes the results file might contain new entries since the last read.
void MatchScheduler::processMatchResultFile(const char* results_filename) {
    // Use a static vector to store processed match IDs to avoid reprocessing
    // This is a simple approach; for large files, a more efficient method
    // like reading only new lines or using a hash set might be needed.
    static std::vector<int> processedMatchIDs;

    std::ifstream file(results_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open results file: " << results_filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        int matchID, winnerID, loserID;
        if (parseResultLine(line, matchID, winnerID, loserID)) {
            // Check if this matchID has already been processed
            bool alreadyProcessed = false;
            for (size_t i = 0; i < processedMatchIDs.size(); ++i) {
                if (processedMatchIDs[i] == matchID) {
                    alreadyProcessed = true;
                    break;
                }
            }

            if (!alreadyProcessed) {
                // Find the match and update its winner/played status
                Match* completedMatch = groupStageMatches.getMatchByID(matchID);
                if (completedMatch) {
                    completedMatch->winnerID = winnerID;
                    completedMatch->played = true;
                    std::cout << "Processed Group Stage Match: " << matchID << ", Winner: " << winnerID << std::endl;
                }
                else {
                    // Check knockout stage if not found in group stage
                    knockoutBracket.updateBracketWinner(matchID, winnerID);
                    std::cout << "Processed Knockout Match: " << matchID << ", Winner: " << winnerID << std::endl;
                }

                // Update player statuses and wins/losses
                updatePlayerStatsAndStatus(winnerID, loserID);

                // Add to processed list
                processedMatchIDs.push_back(matchID);
            }
        }
        else {
            std::cerr << "Warning: Failed to parse result line: " << line << std::endl;
        }
    }
    file.close();

    // After processing results, update output files
    outputCurrentStandings("current_standings.txt");
    // You might also regenerate scheduled_matches.txt if new matches become available
    // (e.g., in knockout stage as players advance)
}

// Generates group stage matches using a round-robin approach.
void MatchScheduler::generateGroupStageMatches() {
    // Clear any existing matches in the queue if regenerating
    while (!groupStageMatches.isEmpty()) {
        delete groupStageMatches.dequeue(); // Free memory
    }

    // Assign players to a default group if not already assigned
    Player* current = allPlayers.head;
    while (current != nullptr) {
        if (current->groupID == 0) { // If group not set, assign to default group 1
            current->groupID = 1;
        }
        current = current->next;
    }

    // Simple round-robin for all players in group 1 (for demonstration)
    if (allPlayers.getPlayerCount() < 2) {
        std::cout << "Not enough players for group stage." << std::endl;
        return;
    }

    Player* p1 = allPlayers.head;
    while (p1 != nullptr) {
        Player* p2 = p1->next; // Start from the next player to avoid self-play and duplicate pairs
        while (p2 != nullptr) {
            // Only create matches if both players are in the same group (if groups were more complex)
            if (p1->groupID == p2->groupID) {
                Match* newMatch = new Match(nextMatchID++, p1->playerID, p2->playerID);
                groupStageMatches.enqueue(newMatch);
            }
            p2 = p2->next;
        }
        p1 = p1->next;
    }
    std::cout << "Generated " << groupStageMatches.currentSize << " group stage matches." << std::endl;
    outputScheduledMatches("scheduled_matches.txt"); // Immediately output newly scheduled matches
}

// Generates the knockout stage bracket based on group stage results.
void MatchScheduler::generateKnockoutStageBracket() {
    // 1. Identify qualifying players based on group stage performance
    //    (e.g., top N players by wins, then by losses).
    allPlayers.sortPlayersByRanking(); // Sorts players in place

    // Create a temporary list of qualifiers
    // For simplicity, let's take a fixed number of top players
    PlayerList qualifiers;
    Player* current = allPlayers.head;
    int qualifiedCount = 0;
    // Example: take top 8 players for knockout (needs 2^N players)
    while (current != nullptr && qualifiedCount < 8) {
        // Create a new Player object (copy data) for the qualifiers list
        Player* qualifiedPlayer = new Player(current->playerID, current->name, current->registrationTime, current->status, current->priority, current->groupID);
        qualifiers.addPlayer(qualifiedPlayer);
        current = current->next;
        qualifiedCount++;
    }

    if (qualifiers.getPlayerCount() < 2) {
        std::cout << "Not enough players qualified for knockout stage." << std::endl;
        // Clean up temporary qualifiers list
        // This is handled by qualifiers destructor since it's a local object.
        return;
    }

    // 2. Build the knockout bracket using the qualified players
    // This will reset the knockoutBracket and build it from the qualifiers.
    if (knockoutBracket.root != nullptr) {
        knockoutBracket.clearBracket(knockoutBracket.root);
        knockoutBracket.root = nullptr;
    }
    knockoutBracket.nextBracketMatchID = 1001; // Reset match ID counter for bracket
    knockoutBracket.buildBracket(&qualifiers); // Pass the qualifiers PlayerList

    std::cout << "Generated knockout stage bracket." << std::endl;
    outputScheduledMatches("scheduled_matches.txt"); // Output knockout matches
}

// Writes currently scheduled (unplayed) matches to an output file.
void MatchScheduler::outputScheduledMatches(const char* output_filename) {
    std::ofstream file(output_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file: " << output_filename << std::endl;
        return;
    }

    // Write group stage matches
    if (!groupStageMatches.isEmpty()) {
        Match* current = groupStageMatches.front;
        do {
            if (!current->played) { // Only write unplayed matches
                file << current->matchID << "," << current->player1ID << "," << current->player2ID << ",GroupStage" << std::endl;
            }
            current = current->next;
        } while (current != groupStageMatches.front);
    }

    // Write knockout stage matches (traverse the bracket to find unplayed matches)
    // A robust implementation would involve traversing the `knockoutBracket.root`
    // and identifying matches where `winnerID` is 0 but `player1ID` and `player2ID` are set.

    file.close();
    std::cout << "Scheduled matches written to " << output_filename << std::endl;
}

// Writes current player standings to an output file.
void MatchScheduler::outputCurrentStandings(const char* output_filename) {
    std::ofstream file(output_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file: " << output_filename << std::endl;
        return;
    }

    Player* current = allPlayers.head;
    while (current != nullptr) {
        file << current->playerID << "," << current->name << "," << current->status
            << "," << current->wins << "," << current->losses << "," << current->groupID << std::endl;
        current = current->next;
    }
    file.close();
    std::cout << "Current standings written to " << output_filename << std::endl;
}

// === Private Helper Functions Implementation ===

// Updates player's stats (wins/losses) and status based on a match result.
void MatchScheduler::updatePlayerStatsAndStatus(int winnerPlayerID, int loserPlayerID) {
    Player* winner = allPlayers.getPlayerByID(winnerPlayerID);
    if (winner) {
        winner->wins++;
        // Update status for group stage players or those just advancing
        if (strcmp(winner->status, "Playing") == 0 || strcmp(winner->status, "Registered") == 0) {
            allPlayers.updatePlayerStatus(winnerPlayerID, "Advanced");
        }
        else if (strcmp(winner->status, "Finalist") == 0) {
            allPlayers.updatePlayerStatus(winnerPlayerID, "Winner"); // If they won the final
        }
    }
    Player* loser = allPlayers.getPlayerByID(loserPlayerID);
    if (loser) {
        loser->losses++;
        // Update status for eliminated players
        if (strcmp(loser->status, "Playing") == 0 || strcmp(loser->status, "Registered") == 0 || strcmp(loser->status, "Advanced") == 0) {
            allPlayers.updatePlayerStatus(loserPlayerID, "Eliminated");
        }
    }
}

// === Display Functions (for debugging/demonstration) ===

void MatchScheduler::displayAllPlayers() {
    allPlayers.printAllPlayers();
}

void MatchScheduler::displayGroupStageQueue() {
    groupStageMatches.printQueue();
}

void MatchScheduler::displayKnockoutBracket() {
    std::cout << "\n--- Tournament Knockout Bracket ---" << std::endl;
    // Pass the total number of players to enable correct round numbering
    knockoutBracket.printBracket(knockoutBracket.root, allPlayers.getPlayerCount());
    // The printBracket now handles the closing line itself.
}

// --- Simulation Functions (for testing the independent operation) ---

// Simulates group stage matches by randomly assigning winners
void MatchScheduler::runGroupStageSimulation() {
    std::cout << "\n--- Running Group Stage Simulation ---" << std::endl;
    // Collect all match IDs first to avoid modifying queue while iterating
    std::vector<int> matchIDsToSimulate;
    if (!groupStageMatches.isEmpty()) {
        Match* current = groupStageMatches.front;
        do {
            if (!current->played) {
                matchIDsToSimulate.push_back(current->matchID);
            }
            current = current->next;
        } while (current != groupStageMatches.front);
    }

    for (size_t i = 0; i < matchIDsToSimulate.size(); ++i) {
        int currentMatchID = matchIDsToSimulate[i];
        Match* match = groupStageMatches.getMatchByID(currentMatchID);
        if (match && !match->played) {
            // Simulate winner randomly (e.g., player1 wins, or toggle for simple alternation)
            int winnerID = match->player1ID;
            int loserID = match->player2ID;

            std::cout << "Simulating Match " << match->matchID << ": P" << match->player1ID << " vs P" << match->player2ID << ". Winner: P" << winnerID << std::endl;

            // Update internal state
            match->winnerID = winnerID;
            match->played = true;
            updatePlayerStatsAndStatus(winnerID, loserID);
        }
    }

    std::cout << "--- Group Stage Simulation Complete ---" << std::endl;
    outputCurrentStandings("current_standings.txt");
    outputScheduledMatches("scheduled_matches.txt"); // Update to show no more group matches
}

// Simulates knockout stage matches
void MatchScheduler::runKnockoutStageSimulation() {
    // This will now call the robust simulation within TournamentBracket
    knockoutBracket.simulateKnockoutMatches(&allPlayers);

    outputCurrentStandings("current_standings.txt");
    outputScheduledMatches("scheduled_matches.txt");
}