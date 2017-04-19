#include <iostream>
#include "routingTable.h"
//#include "receiver.h"

int main(){
  uint32_t interfaces;
  std::cout << "Number of interfaces: ";
  std::cin >> interfaces;
  RoutingTable rt(interfaces);
  
  while(interfaces--){
    std::cout << "Enter interface\n";
    std::cin >> rt;
  }

  //Receiver receiver(rt);
  
  for(unsigned i=0;;++i){
    std::cout << "ROUND " << std::left << std::setw(4) << i  << rt;
    rt.receiveData(std::chrono::seconds(5));
    rt.sendTable();
    //sleep(3);
  }
  
  return EXIT_SUCCESS;
}
