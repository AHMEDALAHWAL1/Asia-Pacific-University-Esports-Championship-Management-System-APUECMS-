🎮 Asia Pacific University Esports Championship Management System (APUECMS)
The Asia Pacific University Esports Championship Management System (APUECMS) is a C++-based prototype designed to manage and streamline various aspects of the prestigious APU Esports Championship — one of Asia’s premier collegiate esports tournaments. Built as part of a data structures lab project, this system demonstrates the practical implementation of advanced data structures such as stacks, queues, priority queues, and circular queues to solve real-world tournament management challenges.


🧑‍💻 Language: C++ (Without STL containers)
🎯 Goal: Efficient esports tournament management using custom-built data structures.

💡 Problem Statement
Organizing an esports tournament at this scale involves complex logistics — including fair match scheduling, real-time updates, priority queueing for streamers and spectators, and accurate result logging. APUECMS is developed to address these through:

🧩 Match Scheduling & Player Progression

📝 Tournament Registration & Player Queueing

📺 Live Stream & Spectator Queue Management

🧾 Game Result Logging & Performance History

🛠️ Features & Data Structures Used
Task	Description	Data Structure Used
Task 1	Match Scheduling & Player Progression	Stacks, Queues
Task 2	Tournament Registration & Player Queueing	Circular Queue
Task 3	Live Stream & Spectator Queue Management	Priority Queue
Task 4	Game Result Logging & Performance History	Linked List / Custom History Log

Each module was developed by a dedicated team member, ensuring modularity and clarity in code structure.

📁 File Structure
kotlin
Copy
Edit
📦 GROUP17_PROGRAM
├── match_scheduling.cpp
├── registration_queue.cpp
├── spectator_streaming.cpp
├── result_logging.cpp
├── data/
│   ├── players.csv
│   ├── matches.csv
│   └── results.txt
└── README.md
