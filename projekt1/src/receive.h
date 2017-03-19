#pragma once

#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <chrono>
#include <assert.h>
#include <netinet/ip_icmp.h>


struct sockaddr_in sender;	
socklen_t sender_len = sizeof(sender);
u_int8_t buffer[IP_MAXPACKET];
