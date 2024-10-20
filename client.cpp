#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 9090
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    string query;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cout << "Socket creation error" << endl;
        return -1;
    }

    // Allow socket to send broadcast messages
    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        cout << "Failed to set broadcast option" << endl;
        return -1;
    }

    // Configure server address for broadcasting
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Broadcast address

    // Available queries
    cout << "Available queries:" << endl;
    cout << "- police" << endl;
    cout << "- ambulance" << endl;
    cout << "- fire station" << endl;
    cout << "- vehicle repair" << endl;
    cout << "- food delivery" << endl;
    cout << "- blood bank" << endl;

    // Send a test query to establish connection
    string testQuery = "ping";
    sendto(sock, testQuery.c_str(), testQuery.length(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Reset the buffer before reading the response
    memset(buffer, 0, BUFFER_SIZE);
    socklen_t addr_len = sizeof(serv_addr);
    
    // Receive the response from the server to confirm connection
    int bytesRead = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serv_addr, &addr_len);
    if (bytesRead < 0) {
        cout << "Error establishing connection with the server" << endl;
        close(sock);
        return -1;
    }

    cout << "Connection with server is done" << endl;

    // Loop to continuously prompt the user for queries
    while (true) {
        cout << "Enter your query or type 'exit' to stop: ";
        getline(cin, query);

        // Check if the user wants to exit
        if (query == "exit") {
            break;
        }

        // Send the query to the server
        sendto(sock, query.c_str(), query.length(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        // Reset the buffer before reading the response
        memset(buffer, 0, BUFFER_SIZE);

        // Receive the response from the server
        addr_len = sizeof(serv_addr);  // Reset addr_len
        bytesRead = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serv_addr, &addr_len);
        if (bytesRead < 0) {
            cout << "Error reading response from server" << endl;
            break;
        }

        cout << "Emergency service: " << buffer << endl;
    }

    // Close the socket
    close(sock);
    return 0;
}
