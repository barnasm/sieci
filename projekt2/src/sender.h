#pragma once

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>
#include <vector>
#include <iomanip>
#include <limits>

struct IpAddressCIDR{
  std::string my_addr_str;
  char addr_str[INET_ADDRSTRLEN];
  struct in_addr addr{0}; //network address in network order
  struct in_addr broadcast_addr{0}; //network address in network order
  struct in_addr my_addr{0}; //interface address in network order
  uint8_t mask{0};

  void createBroadcastAddr(){
    uint m = 0;
    for(uint i = 0; i < 32-this->mask; ++i)
      m |= 1<<i;
    
    m = htonl(m);
    auto tmp = this->addr.s_addr | m;
    memcpy(&this->broadcast_addr, &tmp, sizeof(in_addr));
  }
    
  friend std::istream& operator>>(std::istream &is, struct IpAddressCIDR &cidr){
    is >> cidr.my_addr_str;

    cidr.mask = inet_net_pton(AF_INET, cidr.my_addr_str.c_str(), &cidr.my_addr, -1);
    inet_net_ntop(AF_INET, &cidr.my_addr, cidr.mask, cidr.addr_str, INET_ADDRSTRLEN);
    
    inet_net_pton(AF_INET, cidr.addr_str, &cidr.addr, -1);
    inet_ntop(AF_INET, &cidr.addr, cidr.addr_str, INET_ADDRSTRLEN);
    cidr.createBroadcastAddr();

    return is;
  }

  friend std::ostream& operator<<(std::ostream &os, const struct IpAddressCIDR &cidr){
    return os << cidr.addr_str << " /" << std::setw(2) << std::left << (uint)cidr.mask;
  }
};

typedef struct{
  struct IpAddressCIDR address;
  uint32_t distance;
  std::string via;
}Connection;


class Sender{
  int sockfd;
  struct sockaddr_in server_address;
  
public:
  Sender(){
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
      std::cerr << "socket error: ";// << strerror(errno) << std::endl;
      throw;
    }
    
    int broadcastPermission = 1;
    setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST,
    		(void *)&broadcastPermission, sizeof(broadcastPermission));

    bzero (&server_address, sizeof(server_address));
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(12345);
  }
  
  std::string createMessage(const Connection &c){
    // std::string message;
    // message.append((char*)&c.address.addr, 4);
    // message.push_back(c.address.mask);
     auto tmp = htonl(c.distance);
    // message.append((char*)&tmp, 4);

    std::string message(9,0);
    memcpy(&message[0], (char*)&c.address.addr, 4);
    message[4] = c.address.mask;
    memcpy(&message[5], (char*)&tmp, 4);
    return message;
  }
  
  void send(const struct in_addr addr, const std::string& message){
    server_address.sin_addr = addr;
    ssize_t message_len = message.size();
    if (sendto(sockfd, message.c_str(), message_len, 0,
	       (struct sockaddr*) &server_address,
	       sizeof(server_address)) != message_len) {
      std::cerr << "sendto error: " << strerror(errno);
      throw;
    }    
  }
  
  ~Sender(){
    close(sockfd);
  }
};
