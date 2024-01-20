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

class ClientSocket {
public:
    ClientSocket() {
        _client_socket = CreateTcpSocket();
        ConnectSocket(_client_socket, IP, PORT);
    }

    ~ClientSocket() {
        Cleanup(_client_socket);
    }

    status Recv() {
        Packet packet_rev;
        packet_rev.SetData(status::NOSTAT, -1);

        int byte_count = recv(_client_socket, reinterpret_cast<void*>(&packet_rev.buffer), sizeof(packet_rev.buffer), 0);

        if(byte_count == -1)
        {
            std::cerr<< "Receive failed - " << strerror(errno) << std::endl;;
            exit(EXIT_FAILURE);
        }

        std::cout << "Message received : "<< status_string[packet_rev.buffer[0]] << " - " << packet_rev.buffer[1] << std::endl;

        return static_cast<status>(packet_rev.buffer[0]);
    }

    void SendAck() {
        std::cout << "Sending the ACK" << std::endl;

        Packet packet_send;
        packet_send.SetData(status::ACCEPTED, 0);

        int byte_count = send(_client_socket, reinterpret_cast<void*>(&packet_send.buffer), sizeof(packet_send.buffer), 0);

        if(byte_count == -1)
        {
            std::cerr<< "Send failed - " << strerror(errno) << std::endl;;
            exit(EXIT_FAILURE);
        }

        std::cout << "Message send : "<< status_string[packet_send.buffer[0]] << " - " << packet_send.buffer[1] << std::endl;
    }
private:
    int _client_socket;

    int CreateTcpSocket()
    {
        int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (client_socket == -1) {
            std::cerr << "Error creating socket - " << strerror(errno) << std::endl;;
            return 1;
        }

        return client_socket;
    }

    void ConnectSocket(int client_socket, const char* ip, uint16_t port)
    {
        struct sockaddr_in client_service;
        memset(&client_service, 0, sizeof(client_service));
        client_service.sin_family = AF_INET;
        client_service.sin_addr.s_addr = inet_addr(ip);
        client_service.sin_port = htons(port);

        if (connect(client_socket, (struct sockaddr *)&client_service, sizeof(client_service)) < 0) {
            std::cerr <<"client failed to connect - " << strerror(errno) << std::endl;;
            exit(EXIT_FAILURE);
        }
        
        std::cerr << "client connected.\n";
    }

    void Cleanup(int client_socket)
    {
        close(client_socket);
    }

};


int main(int args, char** argv)
{
    ClientSocket client_socket;

    while (true)
    {
        status stat = client_socket.Recv();
        client_socket.SendAck();

        if(stat == status::TERMINATE) {
            std::cout << "Client terminated" << std::endl;
            break;
        }
    }

    return 0;
}