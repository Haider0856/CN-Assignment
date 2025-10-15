#include "socket.hh"
#include "address.hh"
#include <iostream>
#include <string>

using namespace std;

// Download the resource from the given host and path
void get_URL(const string &host, const string &path)
{
    // Create and connect the TCP socket
    TCPSocket socket;
    Address address(host, "http");
    socket.connect(address);

    // Send HTTP GET request
    string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Connection: close\r\n\r\n";
    socket.write(request);

    // Read and print the response
    string buffer;
    while (!socket.eof()) {
        socket.read(buffer);   // <-- note: read into a string reference
        cout << buffer;
        buffer.clear();
    }

    socket.close();
}

// Main function
int main(int argc, char *argv[])
{
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " HOST PATH\n";
        return EXIT_FAILURE;
    }

    try {
        get_URL(argv[1], argv[2]);
    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
