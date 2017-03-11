#include "send.h"

void print_as_bytes (unsigned char*, ssize_t);

int waitButNotBlock(const int x){
  fd_set descriptors;
  FD_ZERO (&descriptors);
  FD_SET (sockfd, &descriptors);
  struct timeval tv; tv.tv_sec = x; tv.tv_usec = 0;
  return select (sockfd+1, &descriptors, NULL, NULL, &tv);
}

int sockfd;
int main(int argc, char** args){
  //PackageData packageData;
  packageData.addressIp = args[1];

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  printf("%s\n %i\n %i\n", packageData.addressIp, sockfd, packageData.pid);
  
  if (sockfd < 0) {
    fprintf(stderr, "socket error: %s\n", strerror(errno)); 
    return EXIT_FAILURE;
  }

  
  for (;;) {
    struct sockaddr_in sender;	
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];

    makeSendPacket();

    int x  = 2;
    int ready = waitButNotBlock(x);
    if(ready < 0) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
      return EXIT_FAILURE;
    }else if(ready == 0){
      fprintf(stderr, "Time out after %i seconds\n", x); 
    }
    
    ssize_t packet_len =
      recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
    		(struct sockaddr*)&sender, &sender_len);
   
    if (packet_len < -1) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
      return EXIT_FAILURE;
    }
    
    char sender_ip_str[20]; 
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
    printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);

    struct iphdr* ip_header = (struct iphdr*) buffer;
    ssize_t ip_header_len = 4 * ip_header->ihl;
    
    printf ("IP header: "); 
    print_as_bytes (buffer, ip_header_len);
    printf("\n");
    
    printf ("IP data:   ");
    print_as_bytes (buffer + ip_header_len, packet_len - ip_header_len);
    printf("\n\n");
  }
  
  return 0;
}


void print_as_bytes (unsigned char* buff, ssize_t length)
{
  for (ssize_t i = 0; i < length; i++, buff++)
    printf ("%.2x ", *buff);	
}
