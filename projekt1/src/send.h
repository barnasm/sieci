
#pragma once

#include "myTraceroute.h"

//struct PackageData {
  //char *addressIp;
  //int ttl = 0;
  //int pnr = 0;
  //const pid_t pid = getpid();
//}extern packageData;
extern struct PackageData packageData;

void sendPackets(uint n=1, struct PackageData *pd = &packageData);

