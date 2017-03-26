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
