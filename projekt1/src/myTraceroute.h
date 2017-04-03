//Michał Barnaś 280012
//http://www.networksorcery.com/enp/protocol/icmp.htm#Data
//http://www.networksorcery.com/enp/protocol/ip.htm#Padding
//https://www.frozentux.net/iptables-tutorial/chunkyhtml/x281.html

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

extern int sockfd;
const int NoPackets = 3;

struct PackageData {
  char *addressIp;
  int ttl = 0;
  int pnr = 0;
  const pid_t pid = getpid();
};
