#ifndef MATCH_HPP
#define MATCH_HPP

#include<iostream>
#include<string>
using namespace std;

// Define the Match structure
struct Match {
    int matchID;
    int player1ID;
    int player2ID;
    int winnerID; // 0 if not played, or PlayerID of the winner
    bool played;  // True if the match has concluded
    Match* next;  // Pointer to the next match in the circular queue

    // Constructor for easy initialization
    Match(int id = 0, int p1 = 0, int p2 = 0) : matchID(id), player1ID(p1), player2ID(p2), winnerID(0), played(false), next(nullptr) {}
};

// Define the custom Circular Queue for Matches
class MatchQueue {
public:
    Match* front; // Pointer to the front of the queue
    Match* rear;  // Pointer to the rear of the queue
    int currentSize; // Tracks the number of matches in the queue

    // Constructor
    MatchQueue() : front(nullptr), rear(nullptr), currentSize(0) {}

    // Destructor to free dynamically allocated Match nodes
    ~MatchQueue() {
        if (front == nullptr) return; // Queue is empty

        Match* current = front;
        Match* temp;
        // Iterate through the circular queue until we reach the front again
        do {
            temp = current->next;
            delete current;
            current = temp;
        } while (current != front); // Stop when we loop back to the start
        front = nullptr;
        rear = nullptr;
    }

    // Enqueues a new Match into the circular queue
    void enqueue(Match* newMatch) {
        if (newMatch == nullptr) return; // Don't enqueue null

        if (front == nullptr) { // Queue is empty
            front = newMatch;
            rear = newMatch;
            newMatch->next = front; // Point to itself for circularity
        }
        else {
            rear->next = newMatch; // Link current rear to new match
            rear = newMatch;       // Update rear to new match
            rear->next = front;    // New rear points back to front to maintain circularity
        }
        currentSize++;
    }

    // Dequeues a Match from the front of the circular queue
    Match* dequeue() {
        if (front == nullptr) { // Queue is empty
            return nullptr;
        }

        Match* dequeuedMatch = front;
        if (front == rear) { // Only one element in the queue
            front = nullptr;
            rear = nullptr;
        }
        else {
            front = front->next; // Move front to the next element
            rear->next = front;  // Maintain circularity by linking rear to new front
        }
        dequeuedMatch->next = nullptr; // Isolate the dequeued node
        currentSize--;
        return dequeuedMatch;
    }

    // Checks if the queue is empty
    bool isEmpty() {
        return front == nullptr;
    }

    // Prints the matches currently in the queue (for debugging/display)
    void printQueue() {
        if (isEmpty()) {
            cout << "Match Queue is empty." << endl;
            return;
        }
        Match* current = front;
        std::cout << "Match Queue (" << currentSize << " matches):" << endl;
        do {
            std::cout << "  MatchID: " << current->matchID
                << ", Player1: " << current->player1ID
                << ", Player2: " << current->player2ID
                << ", Winner: " << (current->winnerID == 0 ? "N/A" : to_string(current->winnerID))
                << ", Played: " << (current->played ? "Yes" : "No") << endl;
            current = current->next;
        } while (current != front);
    }

    // Gets a match by its ID (useful for updating results directly)
    Match* getMatchByID(int id) {
        if (isEmpty()) return nullptr;

        Match* current = front;
        do {
            if (current->matchID == id) {
                return current;
            }
            current = current->next;
        } while (current != front);
        return nullptr; // Match not found
    }
};

#endif // MATCH_HPP