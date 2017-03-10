#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <chrono>
#include <assert.h>
#include <netinet/ip_icmp.h>

void print_as_bytes (unsigned char* buff, ssize_t length)
{
  for (ssize_t i = 0; i < length; i++, buff++)
    printf ("%.2x ", *buff);	
}

u_int16_t compute_icmp_checksum (const void *buff, int length){
  u_int32_t sum;
  const u_int16_t* ptr = (u_int16_t*)buff;
  assert (length % 2 == 0);

  for (sum = 0; length > 0; length -= 2)
    sum += *ptr++;

  sum = (sum >> 16) + (sum & 0xffff);
  return (u_int16_t)(~(sum + (sum >> 16)));
}

struct icmphdr icmp_header;
void createHeaderToSend(){
  // struct icmphdr icmp_header;
  icmp_header.type = ICMP_ECHO;
  icmp_header.code = 0;
  icmp_header.un.echo.id = getpid();
  icmp_header.un.echo.sequence = 777;
  icmp_header.checksum = 0;
  icmp_header.checksum =
    compute_icmp_checksum((u_int16_t*)&icmp_header, sizeof(icmp_header));
}

struct sockaddr_in recipient;
void fillRecipientAddress(){
  bzero (&recipient, sizeof(recipient));
  recipient.sin_family = AF_INET;
				   
  inet_pton(AF_INET, "adres_ip", &recipient.sin_addr);
}
int sockfd;
void setTTL(const void *ttl){
  setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
}

void sendPacket(){
  ssize_t bytes_sent = sendto
    (sockfd, &icmp_header, sizeof(icmp_header), 0,
     (struct sockaddr*)&recipient, sizeof(recipient));
}

int main(int argc, char** args){
  std::cout << args[1] << std::endl;

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    fprintf(stderr, "socket error: %s\n", strerror(errno)); 
    return EXIT_FAILURE;
  }
  
  for (;;) {
    
    struct sockaddr_in 	sender;	
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];
    
    ssize_t packet_len =
      recvfrom (sockfd, buffer, IP_MAXPACKET, 0,
		(struct sockaddr*)&sender, &sender_len);

    if (packet_len < 0) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
      return EXIT_FAILURE;
    }
    
    char sender_ip_str[20]; 
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
    printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);
    
    
    int x  = 2;
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    struct timeval tv; tv.tv_sec = x; tv.tv_usec = 0;
    int ready = select (sockfd+1, &descriptors, NULL, NULL, &tv);
    if(ready < 0) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
      return EXIT_FAILURE;
    }else if(ready == 0){
      fprintf(stderr, "Time out after %i seconds\n", x); 
    }
    
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
