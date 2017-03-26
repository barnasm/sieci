#pragma once

#include "myTraceroute.h"

static struct sockaddr_in sender;	
static socklen_t sender_len = sizeof(sender);
static char sender_ip_str[NoPackets][20];

extern u_int8_t buffer[IP_MAXPACKET];
extern struct PackageData packageData;

int notBlockWait(std::chrono::high_resolution_clock::time_point&);
bool isItMyPacket();
