#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9090
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    string query;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error" << endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert broadcast address or server IP address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address / Address not supported" << endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection failed" << endl;
        return -1;
    }

    // Available queries
    cout << "Available queries:" << endl;
    cout << "- police" << endl;
    cout << "- ambulance" << endl;
    cout << "- fire station" << endl;
    cout << "- vehicle repair" << endl;
    cout << "- food delivery" << endl;
    cout << "- blood bank" << endl;

    // Loop to continuously prompt the user for queries
    while (true) {
        cout << "Enter your query or type 'exit' to stop: ";
        getline(cin, query);

        // Check if the user wants to exit
        if (query == "exit") {
            break;
        }

        // Send the query to the server
        send(sock, query.c_str(), query.length(), 0);

        // Reset the buffer before reading the response
        memset(buffer, 0, BUFFER_SIZE);

        // Receive the response from the server
        int bytesRead = read(sock, buffer, BUFFER_SIZE);
        if (bytesRead < 0) {
            cout << "Error reading response from server" << endl;
            break;
        } else if (bytesRead == 0) {
            cout << "Server closed the connection" << endl;
            break;
        }

        cout << "Emergency service: " << buffer << endl;
    }

    // Close the socket
    close(sock);
    return 0;
}
