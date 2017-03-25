
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

//try to make it extern
static struct sockaddr_in recipient;
static struct icmphdr icmp_header;

extern int sockfd;


struct PackageData {
  char *addressIp;
  int ttl = 100;
  int pnr = 10;
  const pid_t pid = getpid();
}static packageData;

//void makeAndSendPacket(struct PackageData*pd = &packageData);
void sendPackets(uint n=1, struct PackageData *pd = &packageData);

