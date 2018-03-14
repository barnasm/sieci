#pragma once

#include "myTraceroute.h"

extern char sender_ip_str[NoPackets][20];
extern u_int8_t buffer[IP_MAXPACKET];
extern struct PackageData packageData;
extern std::chrono::high_resolution_clock::time_point sendTime;

int tryToGetAllPackets(std::chrono::milliseconds &dur);
