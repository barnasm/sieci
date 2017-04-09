#include <iostream>
#include "routingTable.h"

int main(){
  uint32_t interfaces;
  std::cout << "Number of interfaces: ";
  std::cin >> interfaces;
  RoutingTable rt(interfaces);
  
  while(interfaces--){
    std::cout << "Enter interface\n";
    std::cin >> rt;
  }

  for(;;){
    std::cout << rt;
    rt.sendTable();
    sleep(5);
  }
  
  return 0;
}
