#ifndef __COMMON_H__
#define __COMMON_H__

#include <array>
#include <string>

#define IP  "127.0.0.1"
#define PORT 55555

enum class status {NOSTAT=0, DATA=1, ACCEPTED=2, TERMINATE=3};
std::array<std::string, 4> status_string {"NOSTAT", "DATA", "ACCEPTED", "TERMINATE"};

struct Packet
{
    int buffer[2];

    void SetData(status status, int data) {
        buffer[0] = static_cast<int>(status);
        buffer[1] = data;
    }
};
#endif // __COMMON_H__
