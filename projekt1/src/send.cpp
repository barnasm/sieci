//Michał Barnaś 280012

#include "send.h"

u_int16_t compute_icmp_checksum (const void *buff, int length){
  u_int32_t sum;
  const u_int16_t* ptr = (u_int16_t*)buff;
  assert (length % 2 == 0);

  for (sum = 0; length > 0; length -= 2)
    sum += *ptr++;

  sum = (sum >> 16) + (sum & 0xffff);
  return (u_int16_t)(~(sum + (sum >> 16)));
}


//struct icmphdr icmp_header;
void fillHeader(const int *nth){
  icmp_header.type = ICMP_ECHO;
  icmp_header.code = 0;
  icmp_header.un.echo.id = packageData.pid;
  icmp_header.un.echo.sequence = *nth;
  icmp_header.checksum =
    compute_icmp_checksum((u_int16_t*)&icmp_header, sizeof(icmp_header));
}

//struct sockaddr_in recipient;
void fillRecipientAddress(const char *address_ip){
  bzero (&recipient, sizeof(recipient));
  recipient.sin_family = AF_INET;				   
  inet_pton(AF_INET, address_ip, &recipient.sin_addr);
}

void setTTL(const void *ttl){
  setsockopt (sockfd, IPPROTO_IP, IP_TTL, ttl, sizeof(int)); 
}

void sendPacket(){
  ssize_t bytes_sent = sendto
    (sockfd, &icmp_header, sizeof(icmp_header), 0,
     (struct sockaddr*)&recipient, sizeof(recipient));
}

void makeAndSendPacket(struct PackageData* pd)
{
  printf("\nmaking packet...\nsock %i \nip %s \nttl %i \n", sockfd, pd->addressIp, pd->ttl);
  fillHeader(&pd->pnr);
  pd->pnr++;

  fillRecipientAddress(pd->addressIp);

  setTTL(&pd->ttl);
  
  sendPacket();
}

void sendPackets(uint n, struct PackageData *pd){
  printf("\n\nmaking a few packets...\nsock %i \nip %s \nttl %i \n",
	 sockfd, pd->addressIp, pd->ttl);
  while(n--)
    makeAndSendPacket(pd);

  pd->ttl++;

}
