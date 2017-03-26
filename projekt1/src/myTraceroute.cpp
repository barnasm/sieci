#include "send.h"
#include "receive.h"
#include "myTraceroute.h"
#include <string>
#include <regex>

int sockfd;
PackageData packageData;
std::chrono::high_resolution_clock::time_point sendTime;
u_int8_t buffer[IP_MAXPACKET];

bool isValidAddr(std::string);
bool isDestinationAddress(char addrs[][20], int);

int tryToGetAllPackets(std::chrono::milliseconds &dur){
  int count = 0;
  while(count < NoPackets){
    int ready = notBlockWait(sendTime);
    if(ready < 0) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
      return EXIT_FAILURE;
    }else if(ready == 0){
      //fprintf(stderr, "Time out\n");
      break;
    }
    std::chrono::high_resolution_clock::time_point receiveTime =
      std::chrono::high_resolution_clock::now();
    
    ssize_t packet_len =
      recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
		(struct sockaddr*)&sender, &sender_len);
    
    if (packet_len < -1) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
      return EXIT_FAILURE;
    }
    
    if(not isItMyPacket())
      continue;
      
    dur += std::chrono::duration_cast<std::chrono::milliseconds>(receiveTime-sendTime);
    
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str[count], sizeof(sender_ip_str[count]));
    
      count++;
  }
  return count;
}

int main(int, char** args){

  if(!isValidAddr(args[1])){
    printf("Wrong IP address.\n");
    return EXIT_FAILURE;
  }
  
  packageData.addressIp = args[1];
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  //printf("%s\n %i\n %i\n", packageData.addressIp, sockfd, packageData.pid);
  
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
    // char s[20] = "8.8.8.8";
    // strcpy(sender_ip_str[2], s);
    // if(count == 1) count++;
    
    for(int i = 0; i < count; i++)
      if(strcmp(sender_ip_str[i], sender_ip_str[i+1]) != 0)
	printf ("%16s  ", sender_ip_str[i]);

    if(count == NoPackets)
      printf("%3i ms\n", (int)dur.count()/NoPackets);
    else printf("???\n");
      
    if(isDestinationAddress(sender_ip_str, count))
      break;
  }
  
  close(sockfd);
  return 0;
}

// int notBlockWait(){
//   int sec = 1;
//   std::chrono::microseconds mc = //one second or less
//     std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(sec)) -
//     std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-sendTime);
//   fd_set descriptors;
//   FD_ZERO (&descriptors);
//   FD_SET (sockfd, &descriptors);
//   struct timeval tv; tv.tv_sec = 0; tv.tv_usec = mc.count();
//   return select (sockfd+2, &descriptors, NULL, NULL, &tv);
// };

bool isDestinationAddress(char addrs[][20], int n){
  while(n--)
    if(strcmp(addrs[n], packageData.addressIp) != 0)
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

// bool isItMyPacket(){
//   struct iphdr* ip_header = (struct iphdr*) buffer;
    
//   u_int8_t* icmp_packet = buffer + 4 * ip_header->ihl;
//   struct icmphdr* icmp_header = (struct icmphdr*)icmp_packet;
  
//   if(icmp_header->type == ICMP_TIME_EXCEEDED){//our header is in data of received packe
//     ip_header   = (struct iphdr*)   (icmp_packet + 8); //getting sent ip header
//     icmp_header = (struct icmphdr *)(icmp_packet + 8 + ip_header->ihl * 4); //getting sent icmp header
//   }
//   //printf("ICMP: %d  %d\n", icmp_header->un.echo.id, icmp_header->un.echo.sequence);

//   if(icmp_header->un.echo.id != packageData.pid){
//     //printf("nie moj pakiet\n");
//     return false;
//   }
//   if(icmp_header->un.echo.sequence/NoPackets != packageData.ttl-1){
//     //printf("nie aktualny pakiet\n");
//     return false;
//   }
//   return true;

//   //ssize_t ip_header_len = 4 * ip_header->ihl;
//   // printf ("IP header: "); 
//   // print_as_bytes (buffer, ip_header_len);
//   // printf("\n");
  
//   // printf ("IP data:   ");
//   // print_as_bytes (buffer + ip_header_len, packet_len - ip_header_len);
//   // printf("\n\n");
// }
