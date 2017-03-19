#include "send.h"
#include "receive.h"
#include <iostream>

void print_as_bytes (unsigned char*, ssize_t);
std::chrono::high_resolution_clock::time_point sendTime;
const int NoPackets = 3;

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


bool isItMyPacket(){
  return true;
}

int sockfd;
int main(int argc, char** args){

  packageData.addressIp = args[1];
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  //printf("%s\n %i\n %i\n", packageData.addressIp, sockfd, packageData.pid);
  
  if (sockfd < 0) {
    fprintf(stderr, "socket error: %s\n", strerror(errno)); 
    return EXIT_FAILURE;
  }

  
  //for (;;)
  {
    std::chrono::milliseconds dur(0);
    sendPackets(NoPackets);
    sendTime = std::chrono::high_resolution_clock::now();
    
    int ready = notBlockWait();
    if(ready < 0) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
      return EXIT_FAILURE;
    }else if(ready == 0){
      fprintf(stderr, "Time out\n");
      return EXIT_FAILURE; //obsluga tego pozniej!
    }
    std::chrono::high_resolution_clock::time_point receiveTime = std::chrono::high_resolution_clock::now();
    std::cout << "received in " << 
      std::chrono::duration_cast<std::chrono::milliseconds>
      (receiveTime-sendTime).count() << "ms" << std::endl;

    //odczytaj czy to jest moj pakiet
    //jesli to nie bym moj pakiet czekaj na nastepny odpowiednio mniej czasu
    //jesli to nue byl ostatni(3) pakiet to czekaj na nastepny ale jzu nei sekunde tylko odpowiednio mniej
    //jesli to byl ostatni pakiet to przetwarzamy informacje  
    
    
    ssize_t packet_len =
      recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
		(struct sockaddr*)&sender, &sender_len);
    
    if (packet_len < -1) {
      fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
      return EXIT_FAILURE;
    }
    
    if(isItMyPacket()){
      dur += std::chrono::duration_cast<std::chrono::milliseconds>
      (receiveTime-sendTime);
    }
    
    char sender_ip_str[20]; 
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
    //printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);


    printf ("%s %ims\n", sender_ip_str, dur/NoPackets);
    
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
