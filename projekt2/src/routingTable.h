#pragma once

#include "sender.h"
#include "receiver.h"
#include <chrono>
#include <exception>

class RoutingTable{
  //uint32_t interfaces;
  const uint32_t INF = UINT32_MAX;
  std::vector<Connection> connections;
  std::vector<Connection> interfaces;
  Sender sender;
  Receiver receiver;
public:
  RoutingTable(uint32_t interfaces):
    //interfaces(interfaces),
    sender(),
    receiver(){}

  void sendTable(){
    for(const auto& c: connections){
      auto message = sender.createMessage(c);
      for(auto& i: interfaces)
	sender.send(i.address.broadcast_addr, message);
    }
  }

  inline bool isBetter(Connection &conOld, Connection &conNew){
    return conOld.distance > conNew.distance + conNew.via_ptr->distance;
  }
  
  void relaxConnection(Connection &con){
    for(auto& c: connections)
      if(strcmp(c.address.addr_str, con.address.addr_str) == 0 and c.address.mask == con.address.mask){
	if(isBetter(c, con))
	  c = std::move(con);
	return;
      }

    connections.push_back(con);
  }
  
  void receiveData(std::chrono::seconds dur = std::chrono::seconds(5))try{
    std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();
    for(auto time = std::chrono::steady_clock::now() - timeStart;
	time < dur;
	time = std::chrono::steady_clock::now() - timeStart)
      {
	Connection con(receiver.receive(std::chrono::duration_cast<std::chrono::microseconds>(dur-time)));
	// std::cout << con.address << std::endl
	// 	  << " distance " << con.distance << std::endl
	// 	  <<  " via " << con.via_str << std::endl;
	// std::cout << &con << std::endl << std::endl;
	con.via_ptr = &interfaces[0];
	relaxConnection(con);
      } 
  }
  catch(std::exception &e){//...){
    //std::cout << "error occured: " << e.what() << std::endl; 
  }
  
  friend std::istream& operator>>(std::istream &is, RoutingTable &rt){
    Connection c;
    is >> c.address;
    is.get(); is.ignore(10, ' ');
    is >> c.distance;
    rt.connections.push_back(c);
    rt.interfaces.push_back(std::move(c));
    return is;
  }

  friend std::ostream& operator<<(std::ostream &os, RoutingTable &rt){
    os << std::string(50, '#') << '\n';
    for(const auto &c: rt.connections){
      os << std::setw(16) << std::right << c.address;

      if(c.distance < rt.INF) os << " distance " << c.distance;
      else                    os << " unreachable ";

      if(not c.via_str.empty()) os << " via " << c.via_str << std::endl;
      else                  os <<  " connected directly " << std::endl;
    }
    return os;
  }
};
