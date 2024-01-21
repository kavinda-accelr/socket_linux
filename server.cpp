#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>

#include "common.h"

class ServerSocket{
public:
    ServerSocket() {
        _server_socket = CreateTcpSocket();
        BindSocket(_server_socket, IP, PORT);
        InitListen(_server_socket);
        _accept_socket = WaitForNewConnection(_server_socket);
    }

    ~ServerSocket() {
        Cleanup(_server_socket, _accept_socket);
    }

    void Send(status stat, int data) {
        
        Packet packet_send;
        packet_send.SetData(stat, data);

        int byte_count = send(_accept_socket, reinterpret_cast<void*>(&packet_send.buffer), sizeof(packet_send.buffer), 0);

        if(byte_count == -1)
        {
            std::cerr<< "Send failed - " << strerror(errno) << std::endl;;
            exit(EXIT_FAILURE);
        }

        std::cout << "Message send : "<< status_string[packet_send.buffer[0]] << " - " << packet_send.buffer[1] << std::endl;
    }

    void WaitForAck() {
        Packet packet_rev;
        
        std::cout << "Waiting for ACK" << std::endl;
        
        int byte_count = recv(_accept_socket, reinterpret_cast<void*>(&packet_rev.buffer), sizeof(packet_rev.buffer), 0);

        if(byte_count == -1)
        {
            std::cerr << "Receive failed - " << strerror(errno) << std::endl;;
            exit(EXIT_FAILURE);
        }

        std::cout<< "Message received : "<< status_string[packet_rev.buffer[0]] << " - " << packet_rev.buffer[1] << std::endl;
    }
private:
    int _server_socket;
    int _accept_socket;

    int CreateTcpSocket()
    {
        int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (server_socket == -1) {
            std::cerr << "Error creating socket - " << strerror(errno) << std::endl;;
            return 1;
        }

        return server_socket;
    }

    void BindSocket(int server_socket, const char* ip, uint16_t port)
    {
        sockaddr_in server_address;
        std::memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = inet_addr(ip);
        server_address.sin_port = htons(port);

        int opt = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "Error setting opt - " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        int bind_status = bind(server_socket, (sockaddr *)&server_address, sizeof(server_address));

        if (bind_status == -1) {
            std::cerr << "Error binding socket - " << strerror(errno) << std::endl;;
            exit(EXIT_FAILURE);
        }
    }

    void InitListen(int server_socket)
    {
        int listen_status = listen(server_socket, 1);

        if (listen_status == -1) {
            std::cerr << "Error listening on socket - " << strerror(errno) << std::endl;;;
            exit(EXIT_FAILURE);
        }
        
        std::cout << "Listening for incoming connections..." << std::endl;;;
    }

    int WaitForNewConnection(int server_socket)
    {
        sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_length);

        if (client_socket == -1) {
            std::cerr << "Error accepting connection - " << strerror(errno) << std::endl;;
            return 1;
        }

        std::cout << "Connection accepted from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << "\n";
        return client_socket;
    }

    void Cleanup(int server_socket, int accept_socket)
    {
        close(server_socket);
        close(accept_socket);
    }
};


int main() {

    ServerSocket server_socket;

    int cout = 100;
    for(int i=0; i<cout; i++) {
        status stat;
        if(i == cout - 1) {
            stat = status::TERMINATE;
        }else {
            stat = status::DATA;
        }

        server_socket.Send(stat, i);
        server_socket.WaitForAck();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
