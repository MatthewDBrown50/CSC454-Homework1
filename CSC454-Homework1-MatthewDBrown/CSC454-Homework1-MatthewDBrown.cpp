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

    string getName() {
        return name;
    }

    int getCurrentRoom() {
        return currentRoom;
    }

    int getIdentifier() {
        return identifier;
    }

private:
    // 0 for PC, 1 for animal, 2 for NPC
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

    ~Room() {
        delete[] neighbors;
        delete[] occupants;
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

    void setCleanliness(int cleanliness) {
        this->cleanliness = cleanliness;
    }

    Occupant* getOccupants() {
        return occupants;
    }

private:
    // 0 = dirty, 1 = half-dirty, 2 = clean
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

    // TODO: REMOVE THIS WHEN FINISHED TESTING.
    //cout << player->getName() << " " << to_string(player->getType()) << " " << to_string(player->getCurrentRoom());

    int exitCode = gameLoop(rooms, numberOfRooms, player);

    delete[] rooms;
    delete player;

    exit(exitCode);
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

    if (colonPosition >= 0) {
        string creature = "";
        string command = "";

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
                cleanRoom(rooms, player->getCurrentRoom(), stoi(creature), player);
            }
            else {
                cout << "That creature is not in this room.";
            }            
        }
        else if (command == "dirty") {
            if (rooms[player->getCurrentRoom()].getOccupants()[stoi(creature)].getType() >= 0) {
                dirtyRoom(rooms, player->getCurrentRoom(), stoi(creature), player);
            }
            else {
                cout << "That creature is not in this room.";
            }
        }
        else if (command == "leave") {
            // TODO: THIS CREATURE LEAVES THE ROOM
        }
        else {
            cout << "Invalid command!";
            return -1;
        }

        // Check win condtion. If win or lose conditions are met, display appropriate message. Otherwise, call gameLoop(rooms, numberOfRooms, player);
        if (!checkGameOver(player)) {
            return gameLoop(rooms, numberOfRooms, player);
        }
    }

    return 0;
}

void cleanRoom(Room* rooms, int roomNumber, int creature, PC* player) {
    int currentCleanliness = rooms[roomNumber].getCleanliness();

    if (currentCleanliness == 2) {
        cout << "The room is already clean. No action performed." << endl;
        return;
    }

    int newCleanliness = currentCleanliness + 1;

    rooms[roomNumber].setCleanliness(newCleanliness);

    for (int i = 0; i < 10; i++) {
        if (rooms[roomNumber].getOccupants()[i].getType() < 0) {
            break;
        }

        if (rooms[roomNumber].getOccupants()[i].getType() == 0) {
            continue;
        }      

        if (rooms[roomNumber].getOccupants()[i].getType() == 1) {
            if (rooms[roomNumber].getOccupants()[i].getIdentifier() == creature) {
                player->setRespect(player->getRespect() + 3);

                cout << rooms[roomNumber].getOccupants()[i].getName() << " licks your face a lot. Respect is now " << to_string(player->getRespect()) << "." << endl;
            }
            else {
                player->setRespect(player->getRespect() + 1);

                cout << rooms[roomNumber].getOccupants()[i].getName() << " licks your face. Respect is now " << to_string(player->getRespect()) << "." << endl;
            }
        }

        if (rooms[roomNumber].getOccupants()[i].getType() == 2) {
            if (rooms[roomNumber].getOccupants()[i].getIdentifier() == creature) {
                player->setRespect(player->getRespect() - 3);

                cout << rooms[roomNumber].getOccupants()[i].getName() << " grumbles a lot. Respect is now " << to_string(player->getRespect()) << "." << endl;
            }
            else {
                player->setRespect(player->getRespect() - 1);

                cout << rooms[roomNumber].getOccupants()[i].getName() << " grumbles. Respect is now " << to_string(player->getRespect()) << "." << endl;
            }

            if (rooms[roomNumber].getCleanliness() == 2) {
                // TODO: THIS NPC LEAVES THE ROOM
            }
        }
    }
}

void dirtyRoom(Room* rooms, int roomNumber, int creature, PC* player) {
    int currentCleanliness = rooms[roomNumber].getCleanliness();

    if (currentCleanliness == 0) {
        cout << "The room is already dirty. No action performed." << endl;
        return;
    }

    int newCleanliness = currentCleanliness - 1;

    rooms[roomNumber].setCleanliness(newCleanliness);

    for (int i = 0; i < 10; i++) {
        if (rooms[roomNumber].getOccupants()[i].getType() <= 0) {
            continue;
        }

        if (rooms[roomNumber].getOccupants()[i].getType() == 1) {
            if (rooms[roomNumber].getOccupants()[i].getIdentifier() == creature) {
                player->setRespect(player->getRespect() - 3);

                cout << rooms[roomNumber].getOccupants()[i].getName() << " growls a lot. Respect is now " << to_string(player->getRespect()) << "." << endl;
            }
            else {
                player->setRespect(player->getRespect() - 1);

                cout << rooms[roomNumber].getOccupants()[i].getName() << " growls. Respect is now " << to_string(player->getRespect()) << "." << endl;
            }

            if (rooms[roomNumber].getCleanliness() == 0) {
                // TODO: THIS ANIMAL LEAVES THE ROOM
            }
        }

        if (rooms[roomNumber].getOccupants()[i].getType() == 2) {
            if (rooms[roomNumber].getOccupants()[i].getIdentifier() == creature) {
                player->setRespect(player->getRespect() + 3);

                cout << rooms[roomNumber].getOccupants()[i].getName() << " smiles a lot. Respect is now " << to_string(player->getRespect()) << "." << endl;
            }
            else {
                player->setRespect(player->getRespect() + 1);

                cout << rooms[roomNumber].getOccupants()[i].getName() << " smiles. Respect is now " << to_string(player->getRespect()) << "." << endl;
            }            
        }
    }
}

bool checkGameOver(PC* player) {
    if (player->getRespect() <= 0) {
        cout << "Your respect has reached 0. You have failed. Game over.";
        return true;
    }
    else if (player->getRespect() >= 80) {
        cout << "Congratulations! Your respect is " + to_string(player->getRespect()) + "! You are victorious! Game over.";
        return true;
    }

    return false;
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
            occupant = new Occupant(creatureType, creatureName, creatureRoom, i);
            break;
        case 2:
            creatureName = "NPC " + to_string(i);
            occupant = new Occupant(creatureType, creatureName, creatureRoom, i);
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
