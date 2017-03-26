 #include "receive.h"

int notBlockWait(std::chrono::high_resolution_clock::time_point& sendTime){
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

void print_as_bytes (unsigned char* buff, ssize_t length)
{
  for (ssize_t i = 0; i < length; i++, buff++)
    printf ("%.2x ", *buff);	
}

bool isItMyPacket(){
  struct iphdr* ip_header = (struct iphdr*) buffer;
    
  u_int8_t* icmp_packet = buffer + 4 * ip_header->ihl;
  struct icmphdr* icmp_header = (struct icmphdr*)icmp_packet;
  
  if(icmp_header->type == ICMP_TIME_EXCEEDED){//our header is in data of received packe
    ip_header   = (struct iphdr*)   (icmp_packet + 8); //getting sent ip header
    icmp_header = (struct icmphdr *)(icmp_packet + 8 + ip_header->ihl * 4); //getting sent icmp header
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

  //ssize_t ip_header_len = 4 * ip_header->ihl;
  // printf ("IP header: "); 
  // print_as_bytes (buffer, ip_header_len);
  // printf("\n");
  
  // printf ("IP data:   ");
  // print_as_bytes (buffer + ip_header_len, packet_len - ip_header_len);
  // printf("\n\n");
}
