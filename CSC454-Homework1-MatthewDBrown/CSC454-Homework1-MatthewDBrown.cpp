// -----------------------
// Author: Matthew D Brown
// -----------------------

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class Occupant {
public:
    Occupant() {
        type = -1;
        name = "";
        currentRoom = -1;
    }

    Occupant(int occupantType, string occupantName, int occupantRoom) {
        type = occupantType;
        name = occupantName;
        currentRoom = occupantRoom;
    }

    int getType() {
        return type;
    }

    string getName() {
        return name;
    }

    int getCurrentRoom() {
        return currentRoom;
    }

private:
    // 0 for PC, 1 for animal, 2 for NPC
    int type;
    string name;
    int currentRoom;
};

class PC : public Occupant {
public:
    PC() : Occupant() {
        reputation = 40;
    }

    PC(string playerName, int playerRoom) : Occupant(0, playerName, playerRoom) {
        reputation = 40;
    }

private:
    int reputation;
};

class Room {
public:
    Room() {
        cleanliness = -1;
        neighbors = new int[4];
        for (int i = 0; i < 4; i++) {
            neighbors[i] = 0;
        }
        occupants = new Occupant[10];
    }

    Room(int details[5]) {
        cleanliness = details[0];
        neighbors = new int[4];
        for (int i = 0; i < 4; i++) {
            neighbors[i] = details[i + 1];
        }
        occupants = new Occupant[10];
    }

    int getCleanliness() const {
        return cleanliness;
    }

    int getNeighbor(int index) const {
        if (index >= 0 && index < 4) {
            return neighbors[index];
        }
        return -1;
    }

    Occupant* getOccupants() {
        return occupants;
    }

private:
    int cleanliness;
    int* neighbors;
    Occupant* occupants;
};

int gameLoop(Room* rooms, int numberOfRooms, PC* player);
Room* createRooms(int numberOfRooms);
PC* createOccupants(int numberOfRooms, Room* rooms, int numberOfCreatures);

int main()
{
    int numberOfRooms;
    cin >> numberOfRooms;
    cin.ignore(1, '\n');

    Room* rooms = createRooms(numberOfRooms);

    int numberOfCreatures;
    cin >> numberOfCreatures;
    cin.ignore(1, '\n');

    PC* player = createOccupants(numberOfRooms, rooms, numberOfCreatures);

    //cout << player->getName() << " " << to_string(player->getType()) << " " << to_string(player->getCurrentRoom());

    exit(gameLoop(rooms, numberOfRooms, player));
}

int gameLoop(Room* rooms, int numberOfRooms, PC* player) {
    const int inputSize = 20;
    char userInput[inputSize] = { ' ' };
    cin.getline(userInput, sizeof userInput);

    int colonPosition = -1;

    for (int i = 0; i < inputSize; i++) {
        if (userInput[i] == ' ') {
            break;
        }

        if (userInput[i] == ':') {
            colonPosition = i;
            break;
        }
    }

    string creature = "";
    string command = "";

    if (colonPosition >= 0) {
        for (int i = 0; i < colonPosition; i++) {
            creature += userInput[i];
        }

        for (int i = colonPosition + 1; i < inputSize; i++) {
            if (userInput[i] == ' ') {
                break;
            }

            command += userInput[i];
        }

        if (command == "clean") {
            // Condition verifies that the specified creature is an occupant of the current room
            if (rooms[player->getCurrentRoom()].getOccupants()[stoi(creature)].getType() >= 0) {
                cleanRoom(player->getCurrentRoom(), stoi(creature));
            }
            else {
                cout << "That creature is not in this room.";
            }            
        }
        else if (command == "dirty") {

        }
        else if (command == "leave") {

        }
        else {
            cout << "Invalid command!";
            return -1;
        }

        // check win condtion. If win or lose conditions are met, display appropriate message. Otherwise, call gameLoop(rooms, numberOfRooms, player);
    }

    cout << "creature: " + creature << endl;
    cout << "command: " + command << endl;

    return 0;
}

void cleanRoom(int roomNumber, int creature) {

}

Room* createRooms(int numberOfRooms) {
    Room* rooms = new Room[numberOfRooms];

    for (int i = 0; i < numberOfRooms; i++) {
        char roomDescription[99];
        cin.getline(roomDescription, sizeof roomDescription);

        int roomDetails[5] = { 0 };
        stringstream ss(roomDescription);

        for (int j = 0; j < 5 && ss.good(); j++) {
            ss >> roomDetails[j];
        }

        rooms[i] = Room(roomDetails);
    }

    return rooms;
}

PC* createOccupants(int numberOfRooms, Room* rooms, int numberOfCreatures) {
    PC* player = new PC();

    for (int i = 0; i < numberOfCreatures; i++) {
        char creatureDescription[99];
        cin.getline(creatureDescription, sizeof creatureDescription);

        int creatureDetails[2] = { -1 };
        stringstream ss(creatureDescription);

        for (int j = 0; j < 5 && ss.good(); j++) {
            ss >> creatureDetails[j];
        }

        int creatureType = creatureDetails[0];
        string creatureName = "";
        int creatureRoom = creatureDetails[1];

        Occupant* occupant = new Occupant();
        

        switch (creatureType) {
        case 0:
            creatureName = "Player";
            player = new PC(creatureName, creatureRoom);
            break;
        case 1:
            creatureName = "Animal " + to_string(i);
            occupant = new Occupant(creatureType, creatureName, creatureRoom);
            break;
        case 2:
            creatureName = "NPC " + to_string(i);
            occupant = new Occupant(creatureType, creatureName, creatureRoom);
            break;
        default:
            cout << "Invalid creature type!";
            exit(-1);
            break;
        }

        Occupant* occupants = rooms[creatureRoom].getOccupants();

        if (player->getName() != "") {
            for (int j = 0; j < 10; j++) {
                if (occupants[j].getType() == -1) {
                    occupants[j] = *player;
                    break;
                }
            }
        }
        else {
            for (int j = 0; j < 10; j++) {
                if (occupants[j].getType() == -1) {
                    occupants[j] = *occupant;
                    break;
                }
            }
        }
    }

    return player;
}

