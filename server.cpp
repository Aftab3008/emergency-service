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

void* handleClient(void* arg) {
    int sock = *((int*)arg);
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    while (true) {
        // Clear the buffer before reading
        memset(buffer, 0, BUFFER_SIZE);

        // Receive query from client
        int bytesRead = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (bytesRead < 0) {
            break; // Error receiving data
        }

        string query(buffer);
        cout << "Request received: " << query << endl;

        // Convert query to lowercase (for case-insensitive queries)
        for (auto& c : query) c = tolower(c);

        // Get the emergency number based on the query (with caching)
        string response = getEmergencyNumber(query);

        // Send the response to the client
        sendto(sock, response.c_str(), response.length(), 0, (struct sockaddr*)&client_addr, addr_len);
    }

    close(sock);
    return nullptr;
}

int main() {
    int serverSock;
    struct sockaddr_in serverAddr;
    pthread_t thread_id;

    // Create a socket
    if ((serverSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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

    cout << "Emergency server listening on port " << PORT << "..." << endl;

    // Handle clients
    handleClient((void*)&serverSock);

    // Close the server socket
    close(serverSock);
    return 0;
}
