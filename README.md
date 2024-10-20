# Emergency Service System

## Overview

The Emergency Service System is a UDP-based client-server application designed to provide quick access to emergency service contact numbers. It consists of a server that holds emergency service information and multiple clients that can query this information.

## Features

- UDP-based communication for fast, lightweight queries
- Server supports multiple concurrent clients
- Client-side caching to reduce network load and improve response times
- Broadcast messaging for easy server discovery
- Support for multiple emergency services (Police, Ambulance, Fire, Vehicle Repair, Food Delivery, Blood Bank)

## Requirements

- C++ compiler with C++11 support
- POSIX-compliant operating system (Linux, macOS, etc.)
- pthread library

## Building the Project

To compile the server and client, use the following commands:

```bash
g++ -std=c++11 -pthread server.cpp -o server
g++ -std=c++11 client.cpp -o client
```

## Running the Application

1. Start the server:

   ```
   ./server
   ```

2. In separate terminal windows, run one or more clients:
   ```
   ./client
   ```

## Usage

### Server

The server automatically starts listening for client queries on port 8080. No additional input is required.

### Client

1. The client will prompt you to enter a query string:

   ```
   Available queries:
    - police
    - ambulance
    - fire station
    - vehicle repair
    - food delivery
    - blood bank
    Enter your query or type 'exit' to stop:
   ```

2. Enter a string to query the corresponding emergency service.

3. The client will display the response from the server.

4. Repeat step 1 to make additional queries.

## Protocol Details

- Communication: UDP on port 9090
- Query format: String
- Response format: String containing service name and phone number
- Maximum message size: 1024 bytes

## File Structure

- `server.cpp`: Implementation of the UDP server
- `client.cpp`: Implementation of the UDP client
- `README.md`: This file, containing project documentation

## Server Cache

The server implements a caching mechanism using an `unordered_map` to store previously queried emergency numbers. This cache helps improve the efficiency of the server by reducing the need to repeatedly compute responses for the same queries.

### How It Works:

- When a client sends a query (e.g., "police", "ambulance"), the server first checks if the query is already present in the cache.
- If the query is found in the cache (Cache Hit), the server retrieves the corresponding emergency number directly from the cache and returns it to the client.
- If the query is not found in the cache (Cache Miss), the server determines the appropriate response, caches it for future requests, and sends it back to the client.
- This caching mechanism significantly reduces response times for frequently requested emergency services and optimizes server performance.

### Benefits of Caching:

- **Reduced Latency**: Cached responses can be returned quickly without processing overhead for repeated queries.
- **Lower Resource Consumption**: Reduces the need for repeated calculations and resource utilization, allowing the server to handle more clients efficiently.

## Security Note

This application uses UDP broadcast messages, which are not encrypted. For use in a production environment, additional security measures should be implemented.
