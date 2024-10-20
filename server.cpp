#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <unordered_map>
#include <string>

#define PORT 9090
#define BUFFER_SIZE 1024

using namespace std;

// Cache to store already queried emergency numbers
unordered_map<string, string> emergencyCache;

// Function to return emergency numbers (with caching)
string getEmergencyNumber(const string& query) {
    // Check if the query is already in the cache
    if (emergencyCache.find(query) != emergencyCache.end()) {
        cout << "Cache Hit" << endl;
        return emergencyCache[query];
    }

    // Determine the response based on the query
    string response;
    if (query == "police") response = "Police Station: 100";
    else if (query == "ambulance") response = "Ambulance: 102";
    else if (query == "fire station") response = "Fire Station: 101";
    else if (query == "vehicle repair") response = "Vehicle Repair: 1800-123-456";
    else if (query == "food delivery") response = "Food Delivery: 1800-654-321";
    else if (query == "blood bank") response = "Blood Bank: 1800-789-123";
    else response = "Invalid query!";

    // Cache the result
    emergencyCache[query] = response;

    return response;
}

void* handleClient(void* client_socket) {
    int clientSock = *((int*)client_socket);
    char buffer[BUFFER_SIZE] = {0};

    while (true) {
        // Read query from client
        memset(buffer, 0, BUFFER_SIZE); // Clear buffer before reading
        int bytesRead = read(clientSock, buffer, BUFFER_SIZE);
        if (bytesRead <= 0) {
            // Client disconnected or error
            break;
        }
        
        string query(buffer);
        cout << "Request received: " << query << endl;

        // Convert query to lowercase (for case-insensitive queries)
        for (auto& c : query) c = tolower(c);

        // Check if the client wants to exit
        if (query == "exit") {
            cout << "Client requested to exit." << endl;
            break; // Exit the loop and close the connection
        }

        // Get the emergency number based on the query (with caching)
        string response = getEmergencyNumber(query);

        // Send the response to the client
        send(clientSock, response.c_str(), response.length(), 0);
    }

    // Close the connection
    close(clientSock);
    return nullptr;
}

int main() {
    int serverSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    pthread_t thread_id;

    // Create a socket
    if ((serverSock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverSock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSock, 3) < 0) {
        perror("Listen failed");
        close(serverSock);
        exit(EXIT_FAILURE);
    }

    cout << "Emergency server listening on port " << PORT << "..." << endl;

    while (true) {
        // Accept an incoming client connection
        if ((clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientAddrLen)) < 0) {
            perror("Client accept failed");
            continue;
        }

        // Create a new thread to handle the client
        if (pthread_create(&thread_id, nullptr, handleClient, &clientSock) != 0) {
            perror("Thread creation failed");
            close(clientSock);
        }

        // Detach the thread to handle multiple clients independently
        pthread_detach(thread_id);
    }

    // Close the server socket
    close(serverSock);
    return 0;
}
