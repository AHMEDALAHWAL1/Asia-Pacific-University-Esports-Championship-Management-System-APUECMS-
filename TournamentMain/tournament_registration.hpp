

#ifndef TOURNAMENT_REGISTRATION_HPP
#define TOURNAMENT_REGISTRATION_HPP

#include <string>

struct Player {
    int id;
    std::string name;
    std::string registrationTime;
    std::string status;
    int priority;
    std::string regType;

    Player();
    Player(int, std::string, std::string, std::string, int, std::string);
};

class PriorityQueue {
public:
    Player* queue;
    int size;
    int capacity;

    PriorityQueue();
    ~PriorityQueue();

    void insert(Player);
    Player remove();
    bool isEmpty();
   
    void resize();
};

// Declare the functions you implement in cpp here:
void clearScreen();
void loadPlayersFromFile(PriorityQueue& pq);
void registerPlayer(PriorityQueue& pq, int id);
void checkInPlayer(PriorityQueue& pq, int id);
void withdrawPlayer(PriorityQueue& pq, int id);
void replacePlayer(PriorityQueue& pq, int id, std::string, std::string);
void displayPlayers(const PriorityQueue& pq);


#endif
