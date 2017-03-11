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

static struct sockaddr_in recipient;
static struct icmphdr icmp_header;

extern int sockfd;


struct PackageData {
  char *addressIp;
  int ttl = 0;
  int pnr = 0;
  const pid_t pid = getpid();
}static packageData;

void makeSendPacket(int *pnr = &packageData.pnr,
		    const char *addrIp = packageData.addressIp,
		    int *ttl = &packageData.ttl);

