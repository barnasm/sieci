#include "receiver.h"

Receiver::Receiver(){
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd  < 0) {
    std::cerr << "socket error: " << strerror(errno) << std::endl;
    throw;
  } 
  
  bzero (&server_address, sizeof(server_address));
  server_address.sin_family      = AF_INET;
  server_address.sin_port        = htons(54321);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if (bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    std::cerr << "bind error: " << strerror(errno) << std::endl;
    throw;
  }
}

void Receiver::sendConfirmation(sockaddr_in &sender, socklen_t sender_len){
  //it was not tested!
  const char* reply = "Thank you!";
  ssize_t reply_len = strlen(reply);
  if (sendto(sockfd, reply, reply_len, 0, (struct sockaddr*)&sender, sender_len) != reply_len) {
    std::cerr << "receiver sendto error: " << strerror(errno) << std::endl;
    throw;
  }
}

Connection Receiver::readReceivedData(struct sockaddr_in sender,  uint8_t *buffer, ssize_t datagram_len){
  char sender_ip_str[INET_ADDRSTRLEN]; 
  inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
  
  buffer[datagram_len] = 0;
  char addr_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, buffer, addr_str, INET_ADDRSTRLEN);
  
  uint32_t *dis = (uint32_t*)&buffer[5];
  
  Connection con;
  strcpy(con.address.addr_str, addr_str);
  inet_pton(AF_INET, addr_str, &con.address.addr);
  con.address.mask = buffer[4];
  con.distance = ntohl(*dis);
  con.via_str = sender_ip_str;
  inet_pton(AF_INET, con.via_str.c_str(), &con.address.my_addr);
  
  //printf ("Received UDP packet from IP address: %s, port: %d\n", sender_ip_str, ntohs(sender.sin_port));
  //printf ("%ld-byte message: +%s+\n", datagram_len, buffer);
  //printf("ip: %u.%u.%u.%u\n", buffer[0], buffer[1], buffer[2], buffer[3]);
  //printf("ip: %s\n", addr_str);
  //printf("distance: %u\n", ntohl(*dis));
  //printf("mask: %u\n", buffer[4]);
  //std::cout << std::flush;
  //fflush(stdout);

  return con;
}

Connection Receiver::receive(std::chrono::microseconds time){
  struct sockaddr_in sender;	
  socklen_t sender_len = sizeof(sender);
  u_int8_t buffer[IP_MAXPACKET+1];
  
  fd_set descriptors;
  FD_ZERO (&descriptors);
  FD_SET (sockfd, &descriptors);
  struct timeval tv; tv.tv_sec = 0; tv.tv_usec = time.count();
  
  int ready = select (sockfd+1, &descriptors, NULL, NULL, &tv);
  if(ready < 0) {
    std::cerr << "select error: " << strerror(errno) << std::endl;
    throw;
  }else if(ready == 0)
    throw std::runtime_error("Time out");
  
  ssize_t datagram_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
				   (struct sockaddr*)&sender, &sender_len);
  if (datagram_len < -1){
    std::cerr << "recvfrom error: " << strerror(errno) << std::endl;
    throw;
  }
  //sendConfirmation(sender, sender_len);
  
  return readReceivedData(sender, buffer, datagram_len);
}
