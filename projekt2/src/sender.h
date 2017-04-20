#pragma once

#include <iostream>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>
#include <vector>
#include <iomanip>
#include <limits>

struct IpAddressCIDR{
  //std::string my_addr_str;
  char addr_str[INET_ADDRSTRLEN];
  struct in_addr addr{0}; //network address in network order
  struct in_addr broadcast_addr{0}; //broadcast address in network order(needed only for interfaces)
  uint8_t mask{0};

  void createBroadcastAddr();
    
  friend std::istream& operator>>(std::istream &is, struct IpAddressCIDR &cidr);
  friend std::ostream& operator<<(std::ostream &os, const struct IpAddressCIDR &cidr);
};

typedef struct Connection{
  struct IpAddressCIDR address;
  uint32_t distance;
  std::string via_str;
  Connection *via_ptr;
  
}Connection;


class Sender{
  int sockfd;
  struct sockaddr_in server_address;
  
public:
  Sender();
  
  std::string createMessage(const Connection &c);
  void send(const struct in_addr addr, const std::string& message);

  ~Sender(){ close(sockfd); }
};
