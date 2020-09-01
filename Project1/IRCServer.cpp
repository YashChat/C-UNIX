const char * usage =
"                                                               \n"
"IRCServer:                                                     \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                            \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "IRCServer.h"
using namespace std;
int QueueLength = 5;

void split(const string &s, const char* delim, vector<string> & v){
    char * dupl = strdup(s.c_str());
    char * token = strtok(dupl, delim);
    while(token != NULL)
    {
        v.push_back(string(token));
        token = strtok(NULL, delim);
    }
    free(dupl);
}

//test

int
IRCServer::open_server_socket(int port) {

        // Set the IP address and port for this server
        struct sockaddr_in serverIPAddress;
        memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
        serverIPAddress.sin_family = AF_INET;
        serverIPAddress.sin_addr.s_addr = INADDR_ANY;
        serverIPAddress.sin_port = htons((u_short) port);

        // Allocate a socket
        int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
        if ( masterSocket < 0) {
                perror("socket");
                exit( -1 );
        }

        // Set socket options to reuse port. Otherwise we will
        // have to wait about 2 minutes before reusing the sae port number
        int optval = 1;
        int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR,
                             (char *) &optval, sizeof( int ) );

        // Bind the socket to the IP address and port
        int error = bind( masterSocket,
                          (struct sockaddr *)&serverIPAddress,
                          sizeof(serverIPAddress) );
        if ( error ) {
                perror("bind");
                exit( -1 );
        }
        
        // Put socket in listening mode and set the 
        // size of the queue of unprocessed connections
        error = listen( masterSocket, QueueLength);
        if ( error ) {
                perror("listen");
                exit( -1 );
        }

        return masterSocket;
}

void
IRCServer::runServer(int port)
{
        int masterSocket = open_server_socket(port);

        initialize();

        while ( 1 ) {

                // Accept incoming connections
                struct sockaddr_in clientIPAddress;
                int alen = sizeof( clientIPAddress );
                int slaveSocket = accept( masterSocket,
                                          (struct sockaddr *)&clientIPAddress,
                                          (socklen_t*)&alen);

                if ( slaveSocket < 0 ) {
                        perror( "accept" );
                        exit( -1 );
                }

                // Process request.
                processRequest( slaveSocket );
        }
}

int
main( int argc, char ** argv )
{
        // Print usage if not enough argumentList
        if ( argc < 2 ) {
                fprintf( stderr, "%s", usage );
                exit( -1 );
        }

        // Get the port from the argumentList
        int port = atoi( argv[1] );
        
        IRCServer ircServer;
        
        // It will never return
        ircServer.runServer(port);
 
}

//
//   Commands:
//   Commands are started y the client.
//
//   Request: ADD-USER <USER> <PASSWD>\r\n
//   Answer: OK\r\n or DENIED\r\n
//
//   REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
//   Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//
//   REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LIST-ROOMS <USER> <PASSWD>\r\n
//   Answer: room1\r\n
//           room2\r\n
//           ...
//           \r\n
//
//   Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LEAVE-ROOM <USER> <PASSWD>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
//   Answer: OK\n or DENIED\n
//
//   Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
//   Answer: MSGNUM1 USER1 MESSAGE1\r\n
//           MSGNUM2 USER2 MESSAGE2\r\n
//           MSGNUM3 USER2 MESSAGE2\r\n
//           ...\r\n
//           \r\n
//
//    REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
//    Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//

void
IRCServer::processRequest( int fd )
{
    // Buffer used to store the comand received from the client
    const int MaxCommandLine = 1024;
    char commandLine[MaxCommandLine + 1];
    int commandLineLength = 0;
    int n;

    // Currently character read
    unsigned char prevChar = 0;
    unsigned char newChar = 0;

    //
    // The client should send COMMAND-LINE\n
    // Read the name of the client character by character until a
    // \n is found.
    //

    // Read character by character until a \n is found or the command std::string is full.
    while (commandLineLength < MaxCommandLine && read(fd, &newChar, 1) > 0) {

        if (newChar == '\n' && prevChar == '\r') {
            break;
        }

        commandLine[commandLineLength] = newChar;
        commandLineLength++;
        prevChar = newChar;
    }

    // Add null character at the end of the char *
    // Eliminate last \r
    commandLineLength--;
    commandLine[commandLineLength] = 0;

    std::cout << "RECEIVED: " << commandLine << "\n";

    std::string command;
    std::string user;
    std::string password;
    std::string args = "";

    std::vector<std::string> argList;
    split(commandLine, " ", argList);

    if (argList.size() >= 3) {
        // !!!!!!! watch for empty tokens and the "\r\n" characters at the end of the string !!!!!!
        command = argList[0];
        user = argList[1];
        password = argList[2];
        if (argList.size() > 3) {
            for (std::vector<int>::size_type i = 3; i != argList.size(); i++) {
                args = args + (argList[i] + " ");
            }
            if (args.at(args.length() - 1) == ' ') {
                args.resize(args.length() - 1);
            }
        }
    } else {
        const char * msg =  "DENIED\r\n";
        write(fd, msg, strlen(msg));
        std::cout << "agument error\r\n";
        close(fd);
                return;
    }

    std::cout << "command=" << command << "\n";
    std::cout << "user=" << user << "\n";
    std::cout << "password=" << password << "\n";
    std::cout << "args=" << args << "\n";


    if (command == "ADD-USER") {
        addUser(fd, user.c_str(), password.c_str(), args.c_str());
    } else if (this->checkPassword(fd, user.c_str(), password.c_str())) {
        if (command == "CREATE-ROOM") {
            createRoom(fd, user, password, args);
        } else if (command == "ENTER-ROOM") {
            enterRoom(fd, user.c_str(), password.c_str(), args.c_str());
        } else if (command == "LEAVE-ROOM") {
            leaveRoom(fd, user.c_str(), password.c_str(), args.c_str());
        } else if (command == "LIST-ROOMS") {
            listRooms(fd, user, password, args);
        } else if (command == "SEND-MESSAGE") {
            sendMessage(fd, user.c_str(), password.c_str(), args.c_str());
        } else if (command == "GET-MESSAGES") {
            getMessages(fd, user.c_str(), password.c_str(), args.c_str());
        } else if (command == "GET-USERS-IN-ROOM") {
            getUsersInRoom(fd, user.c_str(), password.c_str(), args.c_str());
        } else if (command == "GET-ALL-USERS") {
            getAllUsers(fd, user.c_str(), password.c_str(), args.c_str());
        } else {
            const char * msg =  "UNKNOWN COMMAND\r\n";
            write(fd, msg, strlen(msg));
            std::cout << "ERROR (Unkown command)\n";
        }
    } else {
        const char * msg = "ERROR (Wrong password)\r\n";
        write(fd, msg, strlen(msg));
        std::cout << "ERROR (Wrong password)\n";
    }

    close(fd);
}

void
IRCServer::initialize()
{
        struct stat buffer;

    struct stat buf;

        if (!(stat ("password.txt", &buf) == 0)) {
                // create file if not there
                std::ofstream tempFile;
                tempFile.open(PASSWORD_FILE);
                tempFile.close();
        }

        std::ifstream passwordStream;
        passwordStream.open(PASSWORD_FILE);
        if (passwordStream.is_open()) {
                std::string line;
                while (getline(passwordStream, line)) {
                        std::vector<std::string> passwordList;
                        split(line, ",", passwordList);
                        if (passwordList.size() - 2 == 0) {
                std::string password = passwordList.at(1);
                                std::string user = passwordList.at(0);
                                users.new_user(user, password);
                        }
                }
        } else {
                std::cout << "Error while reading the password.txt\n";
                exit(1);
        }

        passwordStream.close();
}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password) {
        if (users.exists(user)) {
                if (users.get_password(user) != password)
                        return false;
                else
                        return true;
        }
        return false;
}

void
IRCServer::addUser(int fd, const char * user2, const char * password2, const char * args)
{
        // Here add a new user

        size_t nothing = std::string::npos;
    string password(password2);
    string user(user2);

        if ( (password.find(";") != nothing) || (password.find(",") != nothing) || (password.find(" ") != nothing) || (user.find(" ") != nothing) || (user.find(";") != nothing) || (user.find(",") != nothing) ) {
                write(fd, "DENIED\r\n", strlen("DENIED\r\n"));
                return;
        }

        if (!users.exists(user)) {
                users.new_user(user, password);
                std::ofstream passFile;
                passFile.open(PASSWORD_FILE, std::ios::in | std::ios::app);
                if (passFile.is_open()) {
                        // write to file with passwords
                        passFile << user << ',' << password << "\n";
                        passFile.close();
                } else {
                        // error
                        write(fd, "DENIED\r\n", strlen("DENIED\r\n"));
                        std::cout << "ERROR, couldn't open password.txt";
                        return;
                }
                write(fd, "OK\r\n", strlen("OK\r\n"));
        } else {
                write(fd, "DENIED\r\n", strlen("DENIED\r\n"));
        }
        return;
}

void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args)
{
        const char * msg;
        std::string room;

        for (int i = 0; i < strlen(args); i++)
        {
                if (args[i] == ' ') {
                        msg = "DENIED\r\n";
                        write(fd, msg, strlen(msg));
                        std::cout << "error, space in args\n";
                        return;
                }
        }

        room = args;

        if (rooms.exists(args)) {
                if (rooms.in_room(room, user)) {
                        msg = "OK\r\n";
                        write(fd, msg, strlen(msg));
                        return;
                } else if (rooms.add_user(room, user)) {
                        msg = "OK\r\n";
                        write(fd, msg, strlen(msg));
                        return;
                } else {
                        msg = "DENIED\r\n";
                        write(fd, msg, strlen(msg));
                        return;
                }
        } else {
                msg = "error, no room created\r\n";
                write(fd, msg, strlen(msg));
                std::cout << msg;
                return;
        }
}

void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args)
{
        const char * msg;
        std::string room;

        for (int i = 0; i < strlen(args); i++)
        {
                if (args[i] == ' ') {
                        msg = "DENIED\r\n";
                        write(fd, msg, strlen(msg));
                        std::cout << "error, space in args\n";
                        return;
                }
        }

        room = args;

        if (rooms.in_room(room, user)) {
                if (rooms.remove_user(room, user)) {
                        msg = "OK\r\n";
                        write(fd, msg, strlen(msg));
                        return;
                } else {
                        msg = "DENIED\r\n";
                        write(fd, msg, strlen(msg));
                        return;
                }
        } else {
                msg = "error, empty room\r\n";
                write(fd, msg, strlen(msg));
                std::cout << msg;
                return;
        }
}

void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args)
{
    const char * msg;
    std::string room;
    std::string message;

    std::vector<std::string> arguments;
    split(args, " ", arguments);


    if (arguments.size() >= 2) {
        room = arguments.at(0);
        for (std::vector<int>::size_type i = 1; i != arguments.size(); i++) {
            message += arguments.at(i) + " ";
        }
    } else {
        msg = "ERROR (user not in room)\r\n";
        write(fd, msg, strlen(msg));
        std::cout << "ERROR (arguments)\n";
        return;
    }

    if (rooms.in_room(room, user)) {
        if (rooms.add_message(room, user, message)) {
            msg = "OK\r\n";
            write(fd, msg, strlen(msg));
            return;
        } else {
            msg = "DENIED\r\n";
            write(fd, msg, strlen(msg));
            return;
        }
    } else {
        msg = "ERROR (user not in room)\r\n";
        write(fd, msg, strlen(msg));
        std::cout << "ERROR (user not in room)\n";
        return;
    }
}

void
IRCServer::getMessages(int fd, const char * user, const char * password, const char * args)
{
        const char * msg;
        int lastMsgNum;
        std::string room;
        std::vector<std::string> argVector;

        split(args, " ", argVector);
        if (argVector.size() == 2) {
                lastMsgNum = atoi(argVector.at(0).c_str());
                room = argVector.at(1);
        } else {
                msg = "error, in argument\r\n";
                write(fd, msg, strlen(msg));
                std::cout << "error, in argument\n";
                return;
        }

        if (rooms.in_room(room, user)) {
                std::vector<std::string> messages;
                
                messages = rooms.get_messages(room, lastMsgNum);
                if (messages.size() < 1) {
                        msg = "No new messages\r\n";
                        std::cout << "No new messages\n";
                        write(fd, msg, strlen(msg));
                        return;
                } else {
                        for (std::vector<int>::size_type i = 0; i != messages.size(); i++) {
                                msg = messages.at(i).c_str();
                                // This write command caused problems, using strlen(msg)) would brek test cases
                                write(fd, msg, strlen(msg + 1));
                                if (messages.at(i) != "No new messages") {
                                        msg = "\r\n"; 
                                        write(fd, msg, strlen(msg));
                                }       
                }               
                msg = "\r\n";
                        write(fd, msg, strlen(msg));
                        return;
                }       
        } else {
                msg = "ERROR (User not in room)\r\n";
                std::cout << "ERROR (User not in room)\n";
                write(fd, msg, strlen(msg));
                return;
        }       
}       

void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args)
{
        const char * msg;
        std::string room;
        std::vector<std::string> argVector;

        split(args, " ", argVector);
        if (argVector.size() == 1) {
                room = argVector[0];
        } else {
                msg = "DENIED\r\n";
                write(fd, msg, strlen(msg));
                std::cout << "ERROR (argVector)\n";
                return;
        }

        if (rooms.exists(room)) {
                std::vector<std::string> userList;

                userList = rooms.get_users(room);
                if (userList.size() < 1) {
                        // List is empty
                        msg = "\r\n"; // respnd with only a newline if userList is empty
                        write(fd, msg, strlen(msg));
                        return;
                } else {
                        // Print a sorted list of users currently in room
                        std::sort(userList.begin(), userList.end());
                        for (std::vector<int>::size_type i = 0; i != userList.size(); i++) {
                                msg = userList[i].c_str();
                                write(fd, msg, strlen(msg));
                                msg = "\r\n";
                                write(fd, msg, strlen(msg));
                }
                msg = "\r\n";
                        write(fd, msg, strlen(msg));
                        return;
                }
        } else {
                msg = "DENIED\r\n";
                write(fd, msg, strlen(msg));
                return;
        }
}

void
IRCServer::getAllUsers(int fd, const char * user, const char * password,const  char * args)
{
        const char * msg;
        std::vector<std::string> userList;

        userList = users.get_users();
        if (userList.size() < 1) {
                write(fd, "DENIED\r\n", strlen("DENIED\r\n"));
                return;
        } else {
                // Print a sorted list of all existing users
                std::sort(userList.begin(), userList.end());
                for (std::vector<int>::size_type i = 0; i != userList.size(); i++) {
                        msg = userList[i].c_str();
                        write(fd, msg, strlen(msg));
                        write(fd, "\r\n", strlen("\r\n"));
        }
                write(fd, "\r\n", strlen("\r\n"));
        }
        return;
}

// added method
void IRCServer::createRoom(int fd, const std::string &user, const std::string &password, const std::string &args) {
        const char * msg;
        std::string room;
        // std::cout << "In create room";

        for (int i = 0; i < args.length(); i++)
        {
                if (args[i] == ' ') {
                        msg = "DENIED\r\n";
                        write(fd, msg, strlen(msg));
                        std::cout << "error, space in args\n";
                        return;
                }
        }
        room = args;

        if (!rooms.exists(room)) {
                rooms.new_room(room);
                msg = "OK\r\n";
                write(fd, msg, strlen(msg));
                std::cout << "Room created\n";
                return;
        } else {
                std::cout << "error, room already exists\n";
                msg = "DENIED\r\n";
                write(fd, msg, strlen(msg));
                std::cout << "error, room already exists\n";
                return;
        }
}

void IRCServer::listRooms(int fd, const std::string &user, const std::string &password, const std::string &args) {
        const char * msg;
        std::vector<std::string> roomsList;

        roomsList = rooms.get_rooms();
        if (roomsList.size() > 0) {
                std::sort(roomsList.begin(), roomsList.end());
                // std::cout << "Rooms list:\n";
                for (std::vector<int>::size_type i = 0; i != roomsList.size(); i++) {
                        std::cout << roomsList[i] << "\n";
                        msg = roomsList[i].c_str();
                        write(fd, msg, strlen(msg));
                        msg = "\r\n";
                        write(fd, msg, strlen(msg));
        }
        } else {
                std::cout << "no rooms to list\n";
                write(fd, "OK\r\n", strlen("OK\r\n"));
                return;
        }
}

