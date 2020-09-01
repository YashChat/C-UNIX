#include "RoomStruct.h"


// init list
RoomStruct::RoomStruct() {
    this->rooms.head = NULL;
}



bool RoomStruct::exists(const std::string &room) {
    RoomListNode * entryNode;

    entryNode = rooms.head;
    while (entryNode != NULL) {
        if (entryNode->name == room) {
            return true;
        }   
        entryNode = entryNode->next;
    }   
    return false;
}

int RoomStruct::count() {
    RoomListNode * entryNode = rooms.head;

    int c = 0;
        while (entryNode != NULL) {
            entryNode = entryNode->next;
            c++;
        }   
        return c;
}

std::vector<std::string> RoomStruct::get_users(const std::string &room) {
    RoomListNode * entryNode = rooms.head;
    std::vector<std::string> nullvector;

    while (entryNode != NULL) {
        if (entryNode->name == room) {
            return entryNode->users;
        }   
        entryNode = entryNode->next;
    }   
    return nullvector;
}

// append new node at beginning
void RoomStruct::new_room(const std::string &room) {
    RoomListNode * entryNode = new RoomListNode;
    entryNode->msgNum = 0;
    entryNode->next = rooms.head;
    entryNode->name = room;
    rooms.head = entryNode;
}

bool RoomStruct::add_user(const std::string &room, const std::string &user) {
    RoomListNode * entryNode = rooms.head;

    while (entryNode != NULL) {

        if (entryNode->name == room) {
            for (std::vector<int>::size_type i = 0; i != entryNode->users.size(); i++) {
                if (entryNode->users[i] == user) {
                    return true;
                }
            }
            entryNode->users.push_back(user);
            return true;
        }
        entryNode = entryNode->next;
    }
    return false;
}


bool RoomStruct::add_message(const std::string &room, const std::string &user, const std::string &message) {
    RoomListNode * entryNode = rooms.head;
        std::string msgStr = "";

    while (entryNode != NULL) {
        if (entryNode->name == room) {
            if (entryNode->messages.size() > 100) {
                // do not more than 100
                entryNode->messages.erase(entryNode->messages.begin());
                entryNode->messages.shrink_to_fit();
            }
            std::string tempStr;
            char * numstr = (char*) malloc(35 * sizeof(char));
            sprintf(numstr, "%d", entryNode->msgNum);
            tempStr = msgStr + numstr;
            msgStr = tempStr + " " + user + " " + message;
            entryNode->msgNum++;
            entryNode->messages.push_back(msgStr);
            return true;
        }
            entryNode = entryNode->next;
    }
    return false;
}

std::vector<std::string> RoomStruct::get_messages(const std::string &room, const int lastMess) {
    RoomListNode * entryNode = rooms.head;
    std::vector<std::string> returnV;
        std::vector<int>::size_type i;
    while (entryNode != NULL) {
        if (entryNode->name == room) {
            int msgToGet = entryNode->msgNum - lastMess;
            if (msgToGet != 0) {
                                for (i = lastMess; i != entryNode->messages.size(); i++) {
                                        returnV.push_back(entryNode->messages.at(i));
                                }
            } else {
                                returnV.push_back("NO-NEW-MESSAGESS");
            }
                        return returnV;
        }
        entryNode = entryNode->next;
    }
    return returnV;
}



bool RoomStruct::in_room(const std::string &room, const std::string &user) {
    RoomListNode * entryNode = rooms.head;

    if (rooms.head != NULL) {

        while (entryNode != NULL) {
            if (entryNode->name == room) {
                std::vector<int>::size_type i;
                for (i = 0; i != entryNode->users.size(); i++) {
                    if (entryNode->users[i] == user) {
                        return true;
                    }
                }
            }
            entryNode = entryNode->next;
        }
    }

    return false;
}


bool RoomStruct::remove_user(const std::string &room, const std::string &user) {
    RoomListNode * entryNode = rooms.head;
        std::vector<int>::size_type i;
    while (entryNode != NULL) {

        if (entryNode->name == room) {
            int n = 0;
            for (i = 0; i != entryNode->users.size(); i++) {
                if (entryNode->users[i] == user) {
                    n = i;
                    break;
                }
            }
            entryNode->users.erase(entryNode->users.begin() + n);
            entryNode->users.shrink_to_fit();
            return true;
        }
        entryNode = entryNode->next;

    }
    return false;
}


std::vector<std::string> RoomStruct::get_rooms() {
    RoomListNode * entryNode = this->rooms.head;
    std::vector<std::string> roomList;

    while (entryNode != NULL) {
        roomList.push_back(entryNode->name);
        entryNode = entryNode->next;
    }
    return roomList;
}
