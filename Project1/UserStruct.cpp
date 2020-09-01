#include "UserStruct.h"

UserStruct::UserStruct() {
    this->users.head = NULL;
}


void UserStruct::new_user(const std::string &user, const std::string &password) {
    UserListNode * entryNode = new UserListNode;
    entryNode->password = password;
    entryNode->next = this->users.head;
    entryNode->name = user;
    this->users.head = entryNode;
}

bool UserStruct::remove(const std::string &user) {
    
    UserListNode * previous = this->users.head;
    UserListNode * entryNode = this->users.head;
    if (this->users.head == NULL) {
        return false;
    }   

    if (entryNode->name == user) {
        this->users.head = entryNode->next;
        return true;
    }   

    while (entryNode->next != NULL) {
        entryNode = entryNode->next;
        if (entryNode->name == user) {
            previous->next = entryNode->next;
            return true;
        }   
        previous = entryNode;
    }   
    return false;
}

int UserStruct::count() {
    UserListNode * entryNode = this->users.head;

    int counter = 0;
        while (entryNode != NULL) {
            entryNode = entryNode->next;
            counter++;
        }   
    return counter;
    }

bool UserStruct::exists(const std::string &user) {
    UserListNode * entryNode = this->users.head;;

    while (entryNode != NULL) {
        if (entryNode->name == user) {
            return true;
        }
        entryNode = entryNode->next;
    }
    return false;
}


std::string UserStruct::get_password(const std::string &user) {
    UserListNode * entryNode = this->users.head;

    while (entryNode != NULL) {
        if (entryNode->name == user) {
            return entryNode->password;
        }
        entryNode = entryNode->next;
    }
    return NULL;
}

std::vector<std::string> UserStruct::get_users() {
    UserListNode * entryNode = this->users.head;
    std::vector<std::string> userList;

    while (entryNode != NULL) {
        userList.push_back(entryNode->name);
        entryNode = entryNode->next;
    }
    return userList;
}
