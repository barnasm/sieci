#pragma once

#include "sender.h"
#include "receiver.h"
#include <chrono>
#include <exception>

class RoutingTable{
  std::vector<Connection> connections;
  std::vector<Connection> interfaces;
  Sender sender;
  Receiver receiver;
  unsigned round = 0;

  void relaxConnection(Connection &con){
    for(auto& c: connections){
      if(c.address.addr.s_addr == con.address.addr.s_addr){
	if(c.distance > con.distance or
	   c.via_str == con.via_str or
	   !c.via_ptr->reachable)
	  c = con;

	for(const auto& i: interfaces)//how to get rid of this loop?
	  if(i.address.addr.s_addr == c.address.addr.s_addr)
	    if(i.distance < c.distance and i.reachable){
	      c = i;
	      break;
	    }
	return;
      }
    }
    
    connections.push_back(std::move(con));
  }
  
  bool viaInterface(const Connection &net, Connection &con) const{
    in_addr addr{0};
    char c[INET_ADDRSTRLEN];
    
    inet_net_ntop(AF_INET, &con.address.my_addr, net.address.mask, c, INET_ADDRSTRLEN);
    inet_net_pton(AF_INET, c, &addr, -1);

    return addr.s_addr == net.address.addr.s_addr;
  }

public:
  RoutingTable(uint32_t interfaces):
    sender(),
    receiver(){
    //in interfaces are const number of elements if you dont reserve memory for them their addresses will be changed and pointers(via) to elements will lost
    this->interfaces.reserve(interfaces);
  }

  void sendTable(){
    for(size_t c=0; c < connections.size(); ++c){
      auto message = sender.createMessage(connections[c]);
      for(const auto& i: interfaces){
	try{
	  sender.send(i.address.broadcast_addr, message);
	  i.reachable = true;
	}
	catch(...){
	  i.reachable = false;
	}
      }
      if(connections[c].distance > Connection::INF*2){
	bool fl=true;	
	for(const auto& in: interfaces){
	  if(connections[c].address.addr.s_addr == in.address.addr.s_addr){
	    connections[c] = in;
	    fl = false;
	    break;
	  }
	}
	if(fl){
	  connections[c] = connections.back();
	  connections.pop_back();
	  c--;
	}
      }
    }
  }
  
  void receiveData(unsigned round, std::chrono::seconds dur = std::chrono::seconds(5)) try{
    this->round = round;
    std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();
    for(auto time = std::chrono::steady_clock::now() - timeStart;
	time < dur;
	time = std::chrono::steady_clock::now() - timeStart)
      {
	Connection con(receiver.receive(std::chrono::duration_cast<std::chrono::microseconds>(dur-time)));
	for( auto &i: interfaces){//check if packet comes from particular network
	  if(i.address.my_addr.s_addr == con.address.my_addr.s_addr)
	    break;
	  if(viaInterface(i, con)){
	    con.via_ptr = &i;
	    i.lastReceivedRound = round;
	    con.lastReceivedRound = round;
	    	    
	    con.distance += i.distance;
	    // if(i.address.addr.s_addr == con.address.addr.s_addr){
	    //this should help get rid of mentioned loop
	    //   relaxConnection(i);
	    // }
	    // else
	    relaxConnection(con);
	    break;
	  }
	}
      }
  }
  catch(std::exception &e){//(...){
    //only for catching time out exception 
    //return; //return shouldnt be necessary
  }
  
  friend std::istream& operator>>(std::istream &is, RoutingTable &rt){
    Connection c;
    is >> c.address;
    is.get(); is.ignore(10, ' ');
    is >> c.distance;
    rt.connections.push_back(c);
    rt.interfaces.push_back(c);

    rt.connections.back().via_ptr = &rt.interfaces.back();
    rt.interfaces.back().via_ptr = &rt.interfaces.back();
    return is;
  }

  friend std::ostream& operator<<(std::ostream &os, RoutingTable &rt){
    os << std::string(50, '#') << '\n';
    for(auto &c: rt.connections){
      os << std::setw(16) << std::right << c.address;
      if(c.isReachable(rt.round)) os << " distance " << std::setw(3) << c.distance;
      else                os << " unreachable ";

      if(not c.via_str.empty()) os << " via " << c.via_str << std::endl;
      else                  os <<  " connected directly " << std::endl;
    }
    return os;
  }
};
