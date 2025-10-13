#include "socket.hh"
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  TCPSocket client_socket;
  Address server_addr( host, "http" );

  client_socket.connect( server_addr );

  string http_request = "GET " + path + " HTTP/1.1\r\n";
  http_request += "Host: " + host + "\r\n";
  http_request += "Connection: close\r\n";
  http_request += "\r\n";

  client_socket.write( http_request );

  string response_data;
  while ( !client_socket.eof() ) 
  {
    client_socket.read( response_data );
    cout << response_data;
  }

  client_socket.close();
}

int main( int argc, char* argv[] )
{
  try 
  {
    if ( argc <= 0 ) 
    {
      abort();
    }

    auto args = span( argv, argc );

    if ( argc != 3 ) 
    {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    const string host { args[1] };
    const string path { args[2] };

    get_URL( host, path );
  } 
  catch ( const exception& e ) 
  {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}