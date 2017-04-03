//Michał Barnaś 280012
#include "send.h"
#include "receive.h"
#include "myTraceroute.h"
#include <string>
#include <regex>

int sockfd;
PackageData packageData;
std::chrono::high_resolution_clock::time_point sendTime;
u_int8_t buffer[IP_MAXPACKET];
char sender_ip_str[NoPackets][20];

bool isValidAddr(std::string);
bool isDestinationAddress(int);

int main(int, char** args){

  if(!isValidAddr(args[1])){
    printf("Wrong IP address.\n");
    return EXIT_FAILURE;
  }
  
  packageData.addressIp = args[1];
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  
  if (sockfd < 0) {
    fprintf(stderr, "socket error: %s\n", strerror(errno)); 
    return EXIT_FAILURE;
  }

  while (packageData.ttl < 30){
    std::chrono::milliseconds dur(0);

    sendPackets(NoPackets);
    sendTime = std::chrono::high_resolution_clock::now();

    int count = tryToGetAllPackets(dur);

    printf("%3i. ", packageData.ttl);
    if(count == 0){
      printf("*\n");
      continue;
    }
    // char s[20] = "8.8.8.8"; //test 
    // strcpy(sender_ip_str[2], s);
    // if(count == 1) count++;
    
    for(int i = 0; i < count; i++)
      if(strcmp(sender_ip_str[i], sender_ip_str[i+1]) != 0)
	printf ("%16s  ", sender_ip_str[i]);

    if(count == NoPackets)
      printf("%3i ms\n", (int)dur.count()/NoPackets);
    else printf("???\n");
      
    if(isDestinationAddress(count))
      break;
  }
  
  close(sockfd);
  return EXIT_SUCCESS;
}

bool isDestinationAddress(int n){
  while(n--)
    if(strcmp(sender_ip_str[n], packageData.addressIp) != 0)
      return false;
  return true;
}

bool isValidAddr(std::string addr){
  std::regex e("(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})$");
  std::smatch m;

  if(!std::regex_search (addr,m,e) || !std::regex_match(addr, e))
    return false;
    
  for (uint i = 1; i < m.size(); i++)
    if((m[i] > "255" and m[i].length() > 2)or(m[i].str().at(0) == '0' and m[i].length() > 1)){
      return false;
    }

  return true;
}

