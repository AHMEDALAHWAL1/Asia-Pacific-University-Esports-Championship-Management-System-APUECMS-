#ifndef BRACKET_HPP
#define BRACKET_HPP

#include <iostream>
#include <string>
#include <queue> // For level-order traversal during build and print
#include <vector> // To temporarily hold players for bracket creation
#include <cmath> // For log2 and pow
#include <algorithm> // For std::min
#include "player.hpp" // Include PlayerList to update player stats

// Represents a node in the binary tree (tournament bracket)
struct BracketNode {
    int matchID;
    int player1ID; // ID of player 1 in this match
    int player2ID; // ID of player 2 in this match
    int winnerID;  // ID of the winner of this match
    BracketNode* left;
    BracketNode* right;

    // Constructor for a match node
    BracketNode(int id = 0) : matchID(id), player1ID(0), player2ID(0), winnerID(0), left(nullptr), right(nullptr) {}

    // Constructor for a player node (leaf of the bracket)
    BracketNode(int id, int playerID) : matchID(id), player1ID(playerID), player2ID(0), winnerID(playerID), left(nullptr), right(nullptr) {}
};

// Tournament Bracket (Binary Tree)
class TournamentBracket {
public:
    BracketNode* root; // Root of the bracket tree
    int nextBracketMatchID; // Counter for unique match IDs in the bracket

    TournamentBracket() : root(nullptr), nextBracketMatchID(1001) {} // Start bracket IDs from 1001

    // Destructor to clear the entire bracket
    ~TournamentBracket() {
        clearBracket(root);
        root = nullptr;
    }

    // Recursively deletes all nodes in the bracket
    void clearBracket(BracketNode* node) {
        if (node == nullptr) return;
        clearBracket(node->left);
        clearBracket(node->right);
        delete node;
    }

    // Builds the knockout bracket based on a list of qualified players
    void buildBracket(PlayerList* qualifiedPlayers) {
        if (qualifiedPlayers == nullptr || qualifiedPlayers->head == nullptr || qualifiedPlayers->getPlayerCount() < 2) {
            std::cout << "Not enough qualified players to build a knockout bracket." << std::endl;
            root = nullptr;
            return;
        }

        // Convert PlayerList to a vector for easier indexing/pairing
        std::vector<Player*> players;
        Player* current = qualifiedPlayers->head;
        while (current != nullptr) {
            players.push_back(current);
            current = current->next;
        }

        int numPlayers = static_cast<int>(players.size());
        if ((numPlayers & (numPlayers - 1)) != 0) { // Check if numPlayers is a power of 2
            std::cout << "Warning: Number of qualified players (" << numPlayers << ") is not a power of 2. Bracket might have byes or uneven matches." << std::endl;
            // For simplicity, we'll proceed, but a real system might handle this more robustly.
            // For example, byes in early rounds. Here we'll just use the players we have.
        }

        // Create initial round of player nodes (leaves of the bracket)
        std::queue<BracketNode*> q;
        for (Player* p : players) {
            q.push(new BracketNode(0, p->playerID)); // Player nodes have matchID 0
        }

        // Build the tree level by level
        while (q.size() > 1) {
            BracketNode* p1_node = q.front(); q.pop();
            BracketNode* p2_node = q.front(); q.pop();

            BracketNode* match_node = new BracketNode(nextBracketMatchID++);
            match_node->left = p1_node;
            match_node->right = p2_node;

            // Set the initial player IDs for the first round of matches
            // These are the winners of the leaf 'player nodes' (which is just the player ID itself)
            match_node->player1ID = p1_node->winnerID;
            match_node->player2ID = p2_node->winnerID;

            q.push(match_node); // Add the newly created match node to the queue for the next level
        }
        root = q.front(); // The last node in the queue is the root (final match)
        q.pop();
    }


    // Updates the winner of a specific match within the bracket
    void updateBracketWinner(int matchID, int winnerID) {
        BracketNode* node = findNode(root, matchID);
        if (node != nullptr) {
            node->winnerID = winnerID;
            std::cout << "Updated Bracket Match " << matchID << " winner to " << winnerID << std::endl;
        }
        else {
            std::cerr << "Warning: Knockout Match with ID " << matchID << " not found." << std::endl;
        }
    }

    // Helper to find a node by match ID (pre-order traversal)
    BracketNode* findNode(BracketNode* node, int id) {
        if (node == nullptr) return nullptr;
        if (node->matchID == id) return node;
        BracketNode* found = findNode(node->left, id);
        if (found != nullptr) return found;
        return findNode(node->right, id);
    }

    // Prints the bracket in a level-order (breadth-first) fashion
    void printBracket(BracketNode* rootNode, int initialPlayerCount) { // Added initialPlayerCount parameter
        if (rootNode == nullptr) {
            std::cout << "Bracket is empty." << std::endl;
            return;
        }

        std::cout << "\n--- Tournament Knockout Bracket ---" << std::endl;
        std::queue<BracketNode*> q;
        q.push(rootNode);

        // Calculate total levels for correct round numbering
        int totalLevels = 0;
        if (initialPlayerCount > 0) {
            // For N players, there are log2(N) match rounds + 1 player level
            totalLevels = static_cast<int>(std::log2(initialPlayerCount)) + 1;
        }
        else {
            std::cout << "Warning: Initial player count is 0. Round numbering might be off." << std::endl;
            totalLevels = 1; // Default to 1 round if unknown
        }

        int currentInternalLevel = 0; // This goes from 0 (root) to totalLevels - 1 (players)

        while (!q.empty()) {
            int nodesAtCurrentLevel = static_cast<int>(q.size());
            int displayRound = totalLevels - currentInternalLevel; // Map internal level to display round

            // Only print round header if there are matches/players in this round
            if (nodesAtCurrentLevel > 0) {
                if (currentInternalLevel == totalLevels - 1) { // This is the last level (players)
                    std::cout << "\n--- Initial Players (Round " << displayRound << ") ---" << std::endl;
                }
                else {
                    std::cout << "\n--- Round " << displayRound << " ---" << std::endl;
                }
            }


            // This loop processes all nodes at the current level
            for (int i = 0; i < nodesAtCurrentLevel; ++i) {
                BracketNode* current = q.front();
                q.pop();

                if (current->matchID == 0) { // It's a player node (leaf)
                    std::cout << "    Player: " << current->player1ID;
                    if (current->winnerID != 0 && current->winnerID == current->player1ID) {
                        std::cout << " (QUALIFIED)";
                    }
                    std::cout << std::endl;
                }
                else { // It's a match node
                    std::cout << "    Match " << current->matchID << ": ";
                    if (current->player1ID != 0 && current->player2ID != 0) {
                        std::cout << "P" << current->player1ID << " vs P" << current->player2ID;
                    }
                    else {
                        // This indicates players are not yet determined from previous rounds
                        std::cout << "TBD vs TBD";
                    }

                    if (current->winnerID != 0) {
                        std::cout << " (Winner: " << current->winnerID << ")";
                    }
                    else {
                        std::cout << " (Pending)";
                    }
                    std::cout << std::endl;

                    // Enqueue children to process the "previous" round
                    if (current->left != nullptr) {
                        q.push(current->left);
                    }
                    if (current->right != nullptr) {
                        q.push(current->right);
                    }
                }
            }
            currentInternalLevel++; // Increment internal level counter
        }
        std::cout << "-----------------------------------" << std::endl;
    }


    // --- New Simulation Logic ---
    // Public interface to start the knockout stage simulation
    void simulateKnockoutMatches(PlayerList* allPlayersList) {
        if (root == nullptr) {
            std::cout << "No knockout bracket to simulate." << std::endl;
            return;
        }
        std::cout << "\n--- Running Knockout Stage Simulation ---" << std::endl;
        // Call the recursive helper to simulate matches from bottom-up
        simulateMatchNode(root, allPlayersList);
        std::cout << "--- Knockout Stage Simulation Complete ---" << std::endl;
    }

private:
    // Recursive helper to simulate matches within the bracket
    int simulateMatchNode(BracketNode* node, PlayerList* allPlayersList) {
        if (node == nullptr) {
            return 0; // No player/winner from a null node
        }

        // If it's a player node (leaf), it's already a winner of its "match" (itself)
        if (node->matchID == 0) {
            return node->winnerID;
        }

        // Recursively simulate children matches to get their winners
        int player1_from_child = simulateMatchNode(node->left, allPlayersList);
        int player2_from_child = simulateMatchNode(node->right, allPlayersList);

        // Set player IDs for the current match from its children's winners
        node->player1ID = player1_from_child;
        node->player2ID = player2_from_child;

        // If the match is not yet played (winnerID == 0) and both players are determined
        if (node->winnerID == 0 && node->player1ID != 0 && node->player2ID != 0) {
            // Simulate winner (e.g., randomly pick one, or just pick player1 for simplicity)
            int winner = node->player1ID; // Simple simulation: Player 1 always wins
            int loser = node->player2ID;

            node->winnerID = winner;

            std::cout << "Simulating Knockout Match " << node->matchID << ": P" << node->player1ID
                << " vs P" << node->player2ID << ". Winner: P" << winner << std::endl;

            // Update player statuses and wins/losses
            // If it's the final match, the winner gets "Winner" status
            if (node == root) { // Assuming root is the final match
                allPlayersList->updatePlayerStatus(winner, "Winner");
                allPlayersList->updatePlayerStatus(loser, "Eliminated");
            }
            else {
                allPlayersList->updatePlayerStatus(winner, "Advanced");
                allPlayersList->updatePlayerStatus(loser, "Eliminated");
            }
        }
        return node->winnerID; // Return the winner of this match
    }

};

#endif // BRACKET_HPP