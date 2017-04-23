#pragma once

#include "sender.h"
#include <chrono>
#include <stdexcept>

class Receiver{
  int sockfd;
  struct sockaddr_in server_address;

  void sendConfirmation(sockaddr_in &sender, socklen_t sender_len);
  Connection readReceivedData(struct sockaddr_in sender, uint8_t *buffer, ssize_t datagram_len);

public:
  Receiver();
  Connection receive(std::chrono::microseconds time);
  ~Receiver(){close (sockfd);}
};
