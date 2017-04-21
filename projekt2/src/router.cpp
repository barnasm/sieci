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

  for(unsigned i=1;;++i){
    std::cout << "ROUND " << std::left << std::setw(5) << i  << rt;
    rt.receiveData(i, std::chrono::seconds(3));
    rt.sendTable();
  }
  
  return EXIT_SUCCESS;
}
