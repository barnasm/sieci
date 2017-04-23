#pragma once

#include <iostream>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>
#include <vector>
#include <iomanip>
#include <limits>

struct IpAddressCIDR{//dobrac lepsze nazwy do pol i wydzielic osobny plik dla tej struktury
  char addr_str[INET_ADDRSTRLEN]; //network addr
  struct in_addr my_addr{0}; //ip address in network order
  struct in_addr addr{0}; //network address in network order
  struct in_addr broadcast_addr{0}; //broadcast address in network order(needed only for interfaces)
  uint8_t mask{0};

  void createBroadcastAddr();
    
  friend std::istream& operator>>(std::istream &is, struct IpAddressCIDR &cidr);
  friend std::ostream& operator<<(std::ostream &os, const struct IpAddressCIDR &cidr);
};

typedef struct Connection{//rozbic to na interfejs i podklasy dla connection i interface
  const static uint8_t INF = 32;
  const static unsigned MAX_ATTEMPT = 1;
  
  struct IpAddressCIDR address;
  uint32_t distance;
  std::string via_str;
  const Connection *via_ptr; //point to interface
  mutable unsigned lastReceivedRound = 0;
  mutable bool reachable = true;
  
  bool isReachable(unsigned round){
    if(!via_str.empty() and distance >= INF){
      distance+=5;
      return false;
    }
    if(!via_str.empty() and round - via_ptr->lastReceivedRound > MAX_ATTEMPT){
      distance = INF+1;
      return false;
    }
    if(!via_ptr->reachable){
      distance = INF+1;
    }
    
    return via_ptr->reachable;
  }
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
