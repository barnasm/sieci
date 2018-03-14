//Michał Barnaś
#include "receive.h"

int notBlockWait(){
  int sec = 1;
  std::chrono::microseconds mc = //one second or less
    std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(sec)) -
    std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-sendTime);
  fd_set descriptors;
  FD_ZERO (&descriptors);
  FD_SET (sockfd, &descriptors);
  struct timeval tv; tv.tv_sec = 0; tv.tv_usec = mc.count();
  return select (sockfd+2, &descriptors, NULL, NULL, &tv);
}

bool isMyPacket(){
  struct iphdr* ip_header = (struct iphdr*) buffer;
    
  u_int8_t* icmp_packet = buffer + 4 * ip_header->ihl;
  struct icmphdr* icmp_header = (struct icmphdr*)icmp_packet;
  
  if(icmp_header->type == ICMP_TIME_EXCEEDED){//our header is in data field of received packet
    ip_header   = (struct iphdr*)   (icmp_packet + 8); //get sent ip header
    icmp_header = (struct icmphdr *)(icmp_packet + 8 + ip_header->ihl * 4); //get sent icmp header
  }
  //printf("ICMP: %d  %d\n", icmp_header->un.echo.id, icmp_header->un.echo.sequence);

  if(icmp_header->un.echo.id != packageData.pid){
    //printf("nie moj pakiet\n");
    return false;
  }
  if(icmp_header->un.echo.sequence/NoPackets != packageData.ttl-1){
    //printf("nie aktualny pakiet\n");
    return false;
  }
  return true;
}

int tryToGetAllPackets(std::chrono::milliseconds &dur){
  int count = 0;
  while(count < NoPackets){
    int ready = notBlockWait();
    if(ready < 0) {
      fprintf(stderr, "select error: %s\n", strerror(errno));
      return EXIT_FAILURE;
    }else if(ready == 0){
      //fprintf(stderr, "Time out\n");
      break;
    }
    std::chrono::high_resolution_clock::time_point receiveTime =
      std::chrono::high_resolution_clock::now();

    struct sockaddr_in sender;	
    socklen_t sender_len = sizeof(sender);

    ssize_t packet_len =
      recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
		(struct sockaddr*)&sender, &sender_len);
    
    if (packet_len < -1) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
      return EXIT_FAILURE;
    }
    
    if(not isMyPacket())
      continue;
      
    dur += std::chrono::duration_cast<std::chrono::milliseconds>(receiveTime-sendTime);
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str[count], sizeof(sender_ip_str[count]));
    
    count++;
  }
  return count;
}
