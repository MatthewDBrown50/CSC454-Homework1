// -----------------------
// Author: Matthew D Brown
// -----------------------

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

class Occupant {
public:
    Occupant() {
        type = -1;
        name = "";
        currentRoom = -1;
        identifier = -1;
    }

    Occupant(int type, string name, int currentRoom, int identifier) {
        this->type = type;
        this->name = name;
        this->currentRoom = currentRoom;
        this->identifier = identifier;
    }

    int getType() {
        return type;
    }

    void setType(int type) {
        this->type = type;
    }

    string getName() {
        return name;
    }

    void setName(string name) {
        this->name = name;
    }

    int getCurrentRoom() {
        return currentRoom;
    }

    void setCurrentRoom(int currentRoom) {
        this->currentRoom = currentRoom;
    }

    int getIdentifier() {
        return identifier;
    }

    void setIdentifier(int identifier) {
        this->identifier = identifier;
    }

private:
    int type;
    string name;
    int currentRoom;
    int identifier;
};

class PC : public Occupant {
public:
    PC() : Occupant() {
        respect = 40;
    }

    PC(string playerName, int playerRoom) : Occupant(0, playerName, playerRoom, -1) {
        respect = 40;
    }

    int getRespect() {
        return respect;
    }

    void setRespect(int respect) {
        this->respect = respect;
    }

private:
    int respect;
};

class Room {
public:
    Room() {
        cleanliness = -1;
        neighbors = new int[4];
        for (int i = 0; i < 4; i++) {
            neighbors[i] = -1;
        }
    }

    ~Room() {
        delete[] neighbors;
    }

    int getCleanliness() const {
        return cleanliness;
    }

    void setCleanliness(int cleanliness) {
        this->cleanliness = cleanliness;
    }

    int getNeighbor(int index) const {
        if (index >= 0 && index < 4) {
            return neighbors[index];
        }
        return -1;
    }

    void setNeighbor(int room, int index) {
        this->neighbors[index] = room;
    }    

private:
    // 0 = clean, 1 = half-dirty, 2 = dirty
    int cleanliness;
    // 0 = north, 1 = south, 2 = east, 3 = west
    int* neighbors;
};

int gameLoop(Room* rooms, Occupant* occupants, int numberOfOccupants, int numberOfRooms, PC* player);
void displayHelp();
void exitGame();
void look(Room* rooms, Occupant* occupants, int numberOfOccupants, int currentRoom);
bool movePlayer(Occupant* occupants, PC* player, int numberOfOccupants, int destination);
void cleanRoom(Room* rooms, Occupant* occupants, int numberOfOccupants, int roomNumber, int creature, PC* player);
void dirtyRoom(Room* rooms, Occupant* occupants, int numberOfOccupants, int roomNumber, int creature, PC* player);
void leaveRoomByChoice(Room* rooms, Occupant* occupants, int numberOfOccupants, int creatureIndex, int currentRoom, PC* player);
int* randomizeNeighbors(Room* rooms, int currentRoom);
void leaveRoomAsDirected(Room* rooms, Occupant* occupants, int numberOfOccupants, int creatureIdentifier, int currentRoom, int destination, PC* player);
int countOccupants(Occupant* occupants, int numberOfOccupants, int room);
bool checkGameOver(PC* player);
void lickFace(string creatureName, PC* player);
void lickFaceALot(string creatureName, PC* player);
void growl(string creatureName, PC* player);
void growlALot(string creatureName, PC* player);
void smile(string creatureName, PC* player);
void smileALot(string creatureName, PC* player);
void grumble(string creatureName, PC* player);
void grumbleALot(string creatureName, PC* player);
Room* createRooms(int numberOfRooms);
Occupant* createOccupants(int numberOfRooms, Room rooms[], int numberOfCreatures, PC* player);

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    int numberOfRooms;
    cout << "Enter number of rooms:" << endl;
    cin >> numberOfRooms;
    cin.ignore(1, '\n');
    cout << endl;

    Room* rooms = createRooms(numberOfRooms);

    int numberOfCreatures;
    cout << "Enter number of creatures:" << endl;
    cin >> numberOfCreatures;
    cin.ignore(1, '\n');
    cout << endl;

    PC* player = new PC();
    Occupant* occupants = createOccupants(numberOfRooms, rooms, numberOfCreatures, player);

    for (int i = 0; i < numberOfCreatures; i++) {
        if (occupants[i].getType() == 0) {
            player->setCurrentRoom(occupants[i].getCurrentRoom());
            player->setType(occupants[i].getType());
            player->setName(occupants[i].getName());
            player->setIdentifier(occupants[i].getIdentifier());
        }
    }

    cout << "The game starts now. Enter the word 'help' to see the list of available commands." << endl;

    int exitCode = gameLoop(rooms, occupants, numberOfCreatures, numberOfRooms, player);

    delete[] rooms;
    delete player;
    delete[] occupants;

    exit(exitCode);
}

int gameLoop(Room* rooms, Occupant* occupants, int numberOfOccupants, int numberOfRooms, PC* player) {
    cout << endl;

    const int inputSize = 20;
    char* userInput = new char[inputSize] { ' ' };
    cin.getline(userInput, inputSize);

    int colonPosition = -1;

    cout << endl;

    for (int i = 0; i < inputSize; i++) {
        if (userInput[i] == ':') {
            colonPosition = i;
            break;
        }

        if (!isalnum(userInput[i])) {
            break;
        }
    }

    // If a colon was found, process creature-directed command
    if (colonPosition >= 0) {
        string creature = "";
        string command = "";
        string partialCommand = "";
        string newRoom = "";

        for (int i = 0; i < colonPosition; i++) {
            creature += userInput[i];
        }

        int creatureIdentifier = stoi(creature);
        int secondColonPosition = -1;

        for (int i = colonPosition + 1; i < inputSize; i++) {
            if (userInput[i] == ':') {
                secondColonPosition = i;
                break;
            }

            if (!isalnum(userInput[i])) {
                break;
            }

            command += userInput[i];
        }

        if (secondColonPosition >= 0) {
            for (int i = (secondColonPosition + 1); i < inputSize; i++) {
                if (userInput[i] == ' ') {
                    break;
                }

                newRoom += userInput[i];
            }
        }

        delete[] userInput;

        if (command == "clean") {
            bool creatureIsInCurrentRoom = false;

            for (int i = 0; i < numberOfOccupants; i++) {
                if (occupants[i].getIdentifier() == creatureIdentifier && occupants[i].getCurrentRoom() == player->getCurrentRoom()) {
                    creatureIsInCurrentRoom = true;
                    break;
                }
            }

            if (creatureIsInCurrentRoom) {
                cleanRoom(rooms, occupants, numberOfOccupants, player->getCurrentRoom(), creatureIdentifier, player);
            }
            else {
                cout << "That creature is not in this room." << endl;
            }
        }
        else if (command == "dirty") {
            bool creatureIsInCurrentRoom = false;

            for (int i = 0; i < numberOfOccupants; i++) {
                if (occupants[i].getIdentifier() == creatureIdentifier && occupants[i].getCurrentRoom() == player->getCurrentRoom()) {
                    creatureIsInCurrentRoom = true;
                    break;
                }
            }

            if (creatureIsInCurrentRoom) {
                dirtyRoom(rooms, occupants, numberOfOccupants, player->getCurrentRoom(), creatureIdentifier, player);
            }
            else {
                cout << "That creature is not in this room." << endl;
            }
        }
        else if (command == "leave") {
            leaveRoomAsDirected(rooms, occupants, numberOfOccupants, creatureIdentifier, player->getCurrentRoom(), stoi(newRoom), player);
        }
        else {
            cout << "Invalid command!" << endl;
        }
    }
    // Otherwise, process player command
    else {
        string command = "";

        for (int i = 0; i < inputSize; i++) {
            if (!isalpha(userInput[i])) {
                break;
            }

            command += userInput[i];
        }

        if (command == "help") {
            displayHelp();
        }
        else if (command == "exit") {
            exitGame();
        }
        else if (command == "look") {
            look(rooms, occupants, numberOfOccupants, player->getCurrentRoom());
        }
        else if (command == "clean") {
            cleanRoom(rooms, occupants, numberOfOccupants, player->getCurrentRoom(), -1, player);
        }
        else if (command == "dirty") {
            dirtyRoom(rooms, occupants, numberOfOccupants, player->getCurrentRoom(), -1, player);
        }
        else if (command == "north") {
            int destination = rooms[player->getCurrentRoom()].getNeighbor(0);

            if (destination < 0) {
                cout << "There is no room in that direction" << endl;
            }
            else {
                if (movePlayer(occupants, player, numberOfOccupants, destination)) {
                    cout << "You leave toward the north." << endl;
                }
                else {
                    cout << "That room is full. You cannot go that way." << endl;
                }
            }

            
        }
        else if (command == "south") {
            int destination = rooms[player->getCurrentRoom()].getNeighbor(1);

            if (movePlayer(occupants, player, numberOfOccupants, destination)) {
                cout << "You leave toward the south." << endl;
            }
            else {
                cout << "You cannot go that way." << endl;
            }
        }
        else if (command == "east") {
            int destination = rooms[player->getCurrentRoom()].getNeighbor(2);

            if (movePlayer(occupants, player, numberOfOccupants, destination)) {
                cout << "You leave toward the east." << endl;
            }
            else {
                cout << "You cannot go that way." << endl;
            }
        }
        else if (command == "west") {
            int destination = rooms[player->getCurrentRoom()].getNeighbor(3);

            if (movePlayer(occupants, player, numberOfOccupants, destination)) {
                cout << "You leave toward the west." << endl;
            }
            else {
                cout << "You cannot go that way." << endl;
            }
        }
        else {
            cout << "Invalid command!" << endl;
        }
    }

    // Check win condtion. If win or lose conditions are met, display appropriate message. Otherwise, call gameLoop(rooms, numberOfRooms, player);
    if (!checkGameOver(player)) {
        return gameLoop(rooms, occupants, numberOfOccupants, numberOfRooms, player);
    }

    return 0;
}

void displayHelp() {
    cout << endl;
    cout << "Available commands:" << endl << endl;
    cout << "help - display this list of commands" << endl << endl;
    cout << "exit - leave the game" << endl << endl;
    cout << "look - view details about the current room" << endl << endl;
    cout << "clean - clean the current room" << endl << endl;
    cout << "[creature index]:clean - make the indicated creature clean the current room" << endl << endl;
    cout << "dirty - dirty the current room" << endl << endl;
    cout << "[creature index]:dirty - make the indicated creature dirty the current room" << endl << endl;
    cout << "north - leave the current room to north" << endl << endl;
    cout << "south - leave the current room to south" << endl << endl;
    cout << "east - leave the current room to east" << endl << endl;
    cout << "west - leave the current room to west" << endl << endl;
    cout << "[creature index]:leave:[destination room number] - make the indicated creature move to the indicated room" << endl << endl;
    cout << "NOTE: [creature index] refers to the number that follows 'animal' or 'NPC' in the creature's name." << endl << endl;
    cout << endl;
}

void exitGame() {
    cout << "Goodbye!" << endl;
    exit(0);
}

void look(Room* rooms, Occupant* occupants, int numberOfOccupants, int currentRoom) {

    string description = "Room " + to_string(currentRoom) + ", ";

    switch (rooms[currentRoom].getCleanliness()) {
        case 0: description += "clean, ";
            break;
        case 1: description += "half dirty, ";
            break;
        case 2: description += "dirty, ";
            break;
        default: {
            cout << "Invalid room state (" << to_string(rooms[currentRoom].getCleanliness()) << ")!";
            return;
        }
    }

    description += "neighbors";

    if (rooms[currentRoom].getNeighbor(0) >= 0) {
        description += " " + to_string(rooms[currentRoom].getNeighbor(0)) += " to the north";
    }
    if (rooms[currentRoom].getNeighbor(1) >= 0) {
        description += " " + to_string(rooms[currentRoom].getNeighbor(1)) += " to the south";
    }
    if (rooms[currentRoom].getNeighbor(2) >= 0) {
        description += " " + to_string(rooms[currentRoom].getNeighbor(2)) += " to the east";
    }
    if (rooms[currentRoom].getNeighbor(3) >= 0) {
        description += " " + to_string(rooms[currentRoom].getNeighbor(3)) += " to the west";
    }

    description += ", contains:";

    cout << description << endl;

    for (int i = 0; i < numberOfOccupants; i++) {
        if (occupants[i].getCurrentRoom() == currentRoom) {
            cout << occupants[i].getName() << endl;
        }
    }
}

bool movePlayer(Occupant* occupants, PC* player, int numberOfOccupants, int destination) {
    for (int i = 0; i < 10; i++) {
        if (countOccupants(occupants, numberOfOccupants, destination) < 10) {
            player->setCurrentRoom(destination);

            return true;
        }
    }

    return false;
}

int countOccupants(Occupant* occupants, int numberOfOccupants, int room) {
    int occupantsInRoom = 0;

    for (int i = 0; i < numberOfOccupants; i++) {
        if (occupants[i].getCurrentRoom() == room) {
            occupantsInRoom++;
        }
    }

    return occupantsInRoom;
}

void cleanRoom(Room* rooms, Occupant* occupants, int numberOfOccupants, int roomNumber, int creature, PC* player) {
    int currentCleanliness = rooms[roomNumber].getCleanliness();

    // If room is already clean, do nothing and return
    if (currentCleanliness == 0) {
        cout << "The room is already clean. No action performed." << endl;
        return;
    }

    // Increase cleanliness of room by 1
    int newCleanliness = currentCleanliness - 1;
    rooms[roomNumber].setCleanliness(newCleanliness);

    // Creatures react according to type. 
    for (int i = 0; i < numberOfOccupants; i++) {
        // Animal reactions
        if (occupants[i].getCurrentRoom() == roomNumber && occupants[i].getType() == 1) {

            // If this Animal is the one who cleaned the room, it reacts 3x
            if (occupants[i].getIdentifier() == creature) {
                lickFaceALot(occupants[i].getName(), player);
            }
            // Otherwise, it reacts once
            else {
                lickFace(occupants[i].getName(), player);
            }
        }

        // NPC reactions
        if (occupants[i].getCurrentRoom() == roomNumber && occupants[i].getType() == 2) {

            // If this NPC is the one who cleaned the room, it reacts 3x
            if (occupants[i].getIdentifier() == creature) {
                grumbleALot(occupants[i].getName(), player);
            }
            // Otherwise, it reacts once
            else {
                grumble(occupants[i].getName(), player);
            }

            // If the room is now "clean", this NPC leaves the room
            if (rooms[roomNumber].getCleanliness() == 0) {
                int creatureIndex = -1;

                leaveRoomByChoice(rooms, occupants, numberOfOccupants, i, roomNumber, player);
            }
        }
    }
}

void dirtyRoom(Room* rooms, Occupant* occupants, int numberOfOccupants, int roomNumber, int creature, PC* player) {
    int currentCleanliness = rooms[roomNumber].getCleanliness();

    // If room is already clean, do nothing and return
    if (currentCleanliness == 2) {
        cout << "The room is already dirty. No action performed." << endl;
        return;
    }

    // Decrease room cleanliness by 1
    int newCleanliness = currentCleanliness + 1;
    rooms[roomNumber].setCleanliness(newCleanliness);

    // Creatures react according to type
    for (int i = 0; i < numberOfOccupants; i++) {
        // Animal reactions
        if (occupants[i].getCurrentRoom() == roomNumber && occupants[i].getType() == 1) {

            // If this animal is the one who dirtied the room, it reacts 3x
            if (occupants[i].getIdentifier() == creature) {
                growlALot(occupants[i].getName(), player);
            }
            // Otherwise, it reacts once
            else {
                growl(occupants[i].getName(), player);
            }

            // If the room's status is now "dirty", this animal leaves the room
            if (newCleanliness == 2) {
                leaveRoomByChoice(rooms, occupants, numberOfOccupants, i, roomNumber, player);
            }
        }

        // NPC reactions
        if (occupants[i].getCurrentRoom() == roomNumber && occupants[i].getType() == 2) {

            // If this NPC is the one who dirtied the room, it reacts 3x
            if (occupants[i].getIdentifier() == creature) {
                smileALot(occupants[i].getName(), player);
            }
            // Otherwise, it reacts once
            else {
                smile(occupants[i].getName(), player);
            }
        }
    }
}

void leaveRoomByChoice(Room* rooms, Occupant* occupants, int numberOfOccupants, int creatureIndex, int currentRoom, PC* player) {
    int* neighborRoomNumbers = randomizeNeighbors(rooms, currentRoom);

    bool left = false;
        
    for (int i = 0; i < 4; i++) {
        if (left) {
            break;
        }

        int roomNumber = neighborRoomNumbers[i];

        if (roomNumber < 0) {
            continue;
        }

        if (countOccupants(occupants, numberOfOccupants, roomNumber) < 10) {
            occupants[creatureIndex].setCurrentRoom(roomNumber);
            string creatureName = occupants[creatureIndex].getName();

            int cleanliness = rooms[roomNumber].getCleanliness();
            int creatureType = occupants[creatureIndex].getType();

            if ((creatureType == 1 && cleanliness == 2) || (creatureType == 2 && cleanliness == 0)) {
                rooms[roomNumber].setCleanliness(1);
            }

            int directionValue = -1;
            string directionText = "";

            for (int k = 0; k < 4; k++) {
                if (roomNumber == rooms[currentRoom].getNeighbor(k)) {
                    directionValue = k;
                }
            }

            switch (directionValue) {
                case 0: directionText = "north";
                    break;
                case 1: directionText = "south";
                    break;
                case 2: directionText = "east";
                    break;
                case 3: directionText = "west";
                    break;
            }

            cout << creatureName << " leaves toward the " << directionText << "." << endl;

            left = true;
        }
    }

    if (!left) {
        string currentCreatureName = occupants[creatureIndex].getName();
        cout << currentCreatureName << " drills a hole in the ceiling and crawls out of the house." << endl;

        occupants[creatureIndex].setCurrentRoom(-1);

        for (int i = 0; i < numberOfOccupants; i++) {
            if (occupants[i].getCurrentRoom() == currentRoom && occupants[i].getType() == 1) {
                growl(occupants[i].getName(), player);
            }
            else if (occupants[i].getCurrentRoom() == currentRoom && occupants[i].getType() == 2) {
                grumble(occupants[i].getName(), player);
            }
        }
    }

    delete[] neighborRoomNumbers;
}

int* randomizeNeighbors(Room* rooms, int currentRoom) {
    int* neighborRoomNumbers = new int[4];

    int room1 = rand() % 4;

    neighborRoomNumbers[0] = rooms[currentRoom].getNeighbor(room1);

    int room2 = room1;

    while (room2 == room1) {
        room2 = rand() % 4;
    }

    neighborRoomNumbers[1] = rooms[currentRoom].getNeighbor(room2);

    int room3 = room1;

    while (room3 == room1 || room3 == room2) {
        room3 = rand() % 4;
    }

    neighborRoomNumbers[2] = rooms[currentRoom].getNeighbor(room3);

    int room4 = room1;

    while (room4 == room1 || room4 == room2 || room4 == room3) {
        room4 = rand() % 4;
    }

    neighborRoomNumbers[3] = rooms[currentRoom].getNeighbor(room4);

    return neighborRoomNumbers;
}

void leaveRoomAsDirected(Room* rooms, Occupant* occupants, int numberOfOccupants, int creatureIdentifier, int currentRoom, int destination, PC* player) {
    int creatureIndex = -1;

    for (int i = 0; i < numberOfOccupants; i++) {
        if (occupants[i].getIdentifier() == creatureIdentifier) {
            creatureIndex = i;
        }
    }

    if (occupants[creatureIndex].getCurrentRoom() != currentRoom) {
        cout << "That creature is not in this room." << endl;
    }


    int creatureType = occupants[creatureIndex].getType();
    string creatureName = occupants[creatureIndex].getName();

    bool destinationIsNeighbor = false;

    for (int i = 0; i < 4; i++) {
        if (rooms[currentRoom].getNeighbor(i) == destination) {
            destinationIsNeighbor == true;
        }
    }

    if (!destinationIsNeighbor) {
        cout << "That room is not a neighbor of this one. Enter 'look' to see which room neighbor this one." << endl;
        return;
    }

    if (countOccupants(occupants, numberOfOccupants, destination) >= 10) {
        cout << "Destination room is full." << endl;

        if (creatureType == 1) {
            growl(creatureName, player);
        }
        else {
            grumble(creatureName, player);
        }

        return;
    }

    if ((creatureType == 1 && rooms[destination].getCleanliness() == 2) || (creatureType == 2 && rooms[destination].getCleanliness() == 0)) {
        rooms[destination].setCleanliness(1);
    }

    occupants[creatureIndex].setCurrentRoom(destination);
    cout << creatureName << " moves to room " << to_string(destination) << endl;
}

bool checkGameOver(PC* player) {
    if (player->getRespect() <= 0) {
        cout << "Your respect has reached 0. You have failed. Game over." << endl;
        return true;
    }
    else if (player->getRespect() >= 80) {
        cout << "Congratulations! Your respect is " << to_string(player->getRespect()) << "! You are victorious! Game over." << endl;
        return true;
    }

    return false;
}

void lickFace(string creatureName, PC* player) {
    player->setRespect(player->getRespect() + 1);
    cout << creatureName << " licks your face. Respect is now " << to_string(player->getRespect()) << "." << endl;
}

void lickFaceALot(string creatureName, PC* player) {
    player->setRespect(player->getRespect() + 3);
    cout << creatureName << " licks your face a lot. Respect is now " << to_string(player->getRespect()) << "." << endl;
}

void growl(string creatureName, PC* player) {
    player->setRespect(player->getRespect() - 1);
    cout << creatureName << " growls. Respect is now " << to_string(player->getRespect()) << "." << endl;
}

void growlALot(string creatureName, PC* player) {
    player->setRespect(player->getRespect() - 3);
    cout << creatureName << " growls a lot. Respect is now " << to_string(player->getRespect()) << "." << endl;
}

void smile(string creatureName, PC* player) {
    player->setRespect(player->getRespect() + 1);
    cout << creatureName << " smiles. Respect is now " << to_string(player->getRespect()) << "." << endl;
}

void smileALot(string creatureName, PC* player) {
    player->setRespect(player->getRespect() + 3);
    cout << creatureName << " smiles a lot. Respect is now " << to_string(player->getRespect()) << "." << endl;
}

void grumble(string creatureName, PC* player) {
    player->setRespect(player->getRespect() - 1);
    cout << creatureName << " grumbles. Respect is now " << to_string(player->getRespect()) << "." << endl;
}

void grumbleALot(string creatureName, PC* player) {
    player->setRespect(player->getRespect() - 3);
    cout << creatureName << " grumbles a lot. Respect is now " << to_string(player->getRespect()) << "." << endl;
}

Room* createRooms(int numberOfRooms) {
    Room* rooms = new Room[numberOfRooms];

    for (int i = 0; i < numberOfRooms; i++) {
        char* roomDescription = new char[99];

        cout << "Enter details for room " << i << ":" << endl;
        int cleanliness, northNeighbor, southNeighbor, eastNeighbor, westNeighbor;
        cin >> cleanliness >> northNeighbor >> southNeighbor >> eastNeighbor >> westNeighbor;
        cout << endl;

        rooms[i].setCleanliness(cleanliness);
        rooms[i].setNeighbor(northNeighbor, 0);
        rooms[i].setNeighbor(southNeighbor, 1);
        rooms[i].setNeighbor(eastNeighbor, 2);
        rooms[i].setNeighbor(westNeighbor, 3);

        delete[] roomDescription;
    }

    return rooms;
}

Occupant* createOccupants(int numberOfRooms, Room rooms[], int numberOfCreatures, PC* player) {

    Occupant* occupants = new Occupant[numberOfCreatures];

    for (int i = 0; i < numberOfCreatures; i++) {
        char* creatureDescription = new char[99];

        cout << "Enter details for next creature:" << endl;
        cin.getline(creatureDescription, 99);
        cout << endl;

        int* creatureDetails = new int[2] { -1 };
        stringstream ss(creatureDescription);

        for (int j = 0; j < 5 && ss.good(); j++) {
            ss >> creatureDetails[j];
        }

        int creatureType = creatureDetails[0];
        string creatureName = "";
        int creatureRoom = creatureDetails[1];

        delete[] creatureDescription;
        delete[] creatureDetails;

        Occupant* occupant = new Occupant();

        switch (creatureType) {
            case 0:
                creatureName = "Player";
                occupant = new PC(creatureName, creatureRoom);
                break;
            case 1:
                creatureName = "Animal " + to_string(i);
                occupant = new Occupant(creatureType, creatureName, creatureRoom, i);
                break;
            case 2:
                creatureName = "NPC " + to_string(i);
                occupant = new Occupant(creatureType, creatureName, creatureRoom, i);
                break;
            default:
                cout << "Invalid creature type!" << endl;
                exit(-1);
                break;
        }

        occupants[i] = *occupant;
    }

    return occupants;
}





