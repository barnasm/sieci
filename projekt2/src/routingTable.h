#pragma once

#include "sender.h"

class RoutingTable{
  uint32_t interfaces;
  const uint32_t INF = UINT32_MAX;
  std::vector<Connection> connections;
  Sender sender;
  
public:
  RoutingTable(uint32_t interfaces): interfaces(interfaces), sender(){};

  void sendTable(){
    for(const auto& c: connections){
      auto message = sender.createMessage(c);
      for(uint i = 0; i < interfaces; ++i)
	sender.send(connections[i].address.broadcast_addr, message);
    }
  }
  
  friend std::istream& operator>>(std::istream &is, RoutingTable &rt){
    Connection c;
    is >> c.address;
    is.get(); is.ignore(10, ' ');
    is >> c.distance;
    rt.connections.push_back(std::move(c));
    return is;
  }

  friend std::ostream& operator<<(std::ostream &os, RoutingTable &rt){
    os << std::string(50, '#') << '\n';
    for(const auto &c: rt.connections){
      os << std::setw(16) << std::right << c.address;

      if(c.distance < rt.INF) os << " distance " << c.distance;
      else                    os << " unreachable ";

      if(not c.via.empty()) os <<  " via " << c.via << std::endl;
      else                  os <<  " connected directly " << std::endl;
    }
    return os;
  }
};
