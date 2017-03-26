#pragma once

#include "myTraceroute.h"

extern struct PackageData packageData;

void sendPackets(uint n=1, struct PackageData *pd = &packageData);

