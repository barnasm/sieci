#pragma once

#include <netinet/ip.h>
#include <arpa/inet.h>
//#include <stdlib.h>
#include <unistd.h>
//#include <errno.h>
//#include <iostream>

//#include <string>
#include <string.h>
#include <vector>
//#include <iostream>
//#include <climits>
#include <iomanip>
#include <limits>
//#include <cstdio>


struct IpAddressCIDR{
  std::string my_addr_str;
  std::string addr_str;
  struct in_addr addr{0}; //network address in network order
  uint8_t mask{0};

  friend std::istream& operator>>(std::istream &is, struct IpAddressCIDR &cidr){
    is >> cidr.my_addr_str;
    //auto sep = cidr.my_addr_str.rfind('/');
    //sscanf(cidr.my_addr_str.c_str()+sep, "/%u", (uint*)&cidr.mask);

    //inet_pton(AF_INET, cidr.my_addr_str.substr(0, sep).c_str(), &cidr.addr);
    cidr.mask = inet_net_pton(AF_INET, cidr.my_addr_str.c_str(), &cidr.addr, -1);


    // cidr.my_addr_str[sep] = '\0'; //in this method we dont need copy of substr
    // inet_pton(AF_INET, cidr.my_addr_str.c_str(), &cidr.addr);
    // cidr.my_addr_str[sep] = '/';
    char test2[INET_ADDRSTRLEN];
    inet_net_ntop(AF_INET, &cidr.addr, cidr.mask, test2, INET_ADDRSTRLEN);
    //inet_ntop(AF_INET, &cidr.addr, test2, INET_ADDRSTRLEN);
    std::cout << test2 << " " << (int)cidr.mask << "\n";
    return is;
  }

  friend std::ostream& operator<<(std::ostream &os, const struct IpAddressCIDR &cidr){
    char nAddrStr[INET_ADDRSTRLEN];
    uint32_t nAddr, mask=UINT32_MAX;
    for(uint8_t i = 0; i <= cidr.mask; ++i)
      mask += 1<<(32-i);

    mask = htonl(mask);
    uint32_t tmpAddr = cidr.addr.s_addr;
    nAddr = tmpAddr & mask;
    
    inet_ntop(AF_INET, &nAddr, nAddrStr, INET_ADDRSTRLEN);
    //os << nAddrStr << '/' << (uint)cidr.mask;
    os << cidr.my_addr_str;
    return os;
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

    bzero (&server_address, sizeof(server_address));
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(12345);
  }
  
  void send(const Connection& c){
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    //const char *byteArray = (char*)&c.distance;
   
    char message[9];
    memcpy(message, (char*)&c.address.addr, 4);
    message[4] = c.address.mask;
    auto tmp = htonl(c.distance);
    memcpy(&message[5], (char*)&tmp, 4);
    //const char* message("helllo");
    ssize_t message_len = sizeof(message);
    if (sendto(sockfd, message, message_len, 0,
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
