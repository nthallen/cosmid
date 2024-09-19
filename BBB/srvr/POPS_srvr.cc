/**
POPS_Server
  Listen on TCP port 7079 (if possible: apparently so, but could check during operation)
  On accept, send current status
  Loop on read() supporting:
    V: Status : Respond with current status
    B: Start : Runs POPS_startup script system("/root/SW/bin/start_POPS");
               Status is updated to Active
    E: Shutdown : Runs system("/sbin/shutdown -h now");
                Status is updated to shutdown
                Reply before shutting down
    D: Disconnect : Don't respond, wait for connection to break
    
    Status is reported as an integer
    Error returns are preceded with "Error"
 */
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "POPS_srvr.h"
#include "dasio/msg.h"
#include "nl_assert.h"

POPS_status_t POPS_status = POPS_idle;

bool pops_socket::protocol_input() {
  while (cp + 2 <= nc) {
    unsigned char cmd = buf[cp];
    if (buf[cp+1] != '\n') {
      msg(2, "%s: Invalid command string", iname);
      ++cp;
    } else {
      cp += 2;
      switch (cmd) {
        case 'V': // Status
          msg(MSG_DEBUG, "V request");
          break;
        case 'B':
          msg(0, "Starting POPS software");
          system("/root/SW/bin/start_POPS");
          POPS_status = POPS_active;
          break;
        case 'E':
          if (POPS_status == POPS_active && send_shutdown()) {
            msg(0, "Shutdown forwarded to POPS");
            POPS_status = POPS_shutdown;
          } else {
            msg(0, "Issuing Shutdown");
            POPS_status = POPS_shutdown;
            system("/sbin/shutdown -h now");
          }
          break;
        case 'F':
          msg(0, "Issuing Forced Shutdown");
          POPS_status = POPS_shutdown;
          system("/sbin/shutdown -h now");
          break;
        case 'D':
          msg(0, "Received disconnect message");
          consume(cp);
          return false;
        default:
          if (isgraph(cmd)) {
            msg(MSG_ERROR, "Invalid command code: '%c'", cmd);
          } else {
            msg(MSG_ERROR, "Invalid command value: '0x%02X'", cmd);
          }
          iwrite("Error: Invalid command\n");
          break;
      }
    }
  }
  consume(cp);
  return send_status();
}

bool pops_socket::send_status() {
  char buf[8];
  snprintf(buf, 8, "%d\n", POPS_status);
  if (iwrite(buf) || !obuf_empty()) return true;
  return false;
}

bool pops_socket::send_shutdown() {
  int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_sock == -1) {
    msg(MSG_ERROR, "%s: Unable to create UDP socket for shutdown: %s",
      iname, strerror(errno));
    return false;
  }
  struct addrinfo hints, *res;
  hints.ai_flags = AI_NUMERICHOST;
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_addrlen = 0;
  hints.ai_canonname = 0;
  hints.ai_addr = 0;
  hints.ai_next = 0;
  if (getaddrinfo("10.11.97.50", "7079", &hints, &res)) {
    msg(MSG_ERROR, "%s: getaddrinfo for shutdown failed: %s",
      iname, strerror(errno));
    return false;
  }
  nl_assert(res->ai_next == 0);
  nl_assert(res->ai_addr != 0);

  struct sockaddr_in s;
  socklen_t addrlen;
  nl_assert(((unsigned)res->ai_addrlen) <= sizeof(s));
  memcpy(&s, res->ai_addr, res->ai_addrlen);
  addrlen = res->ai_addrlen;
  freeaddrinfo(res);
  
  const char *buf = "8";
  int msglen = 1;
  int nb = sendto(udp_sock, buf, msglen, 0, (sockaddr*)&s, addrlen);
  if (nb < msglen) {
    msg(MSG_ERROR, "%s: sendto() expected %d, returned %d",
      iname, msglen, nb);
  }
  ::close(udp_sock);
  return true;
}

pops_socket *new_pops_socket(Authenticator *Auth, SubService *SS) {
  SS = SS;
  pops_socket *pops = new pops_socket(Auth, Auth->get_iname());
  return pops;
}

int main(int argc, char **argv) {
  oui_init_options(argc, argv);
  Server server("pops");
  server.add_subservice(new SubService("pops",
      (socket_clone_t)new_pops_socket, (void*)0));
  msg(0, "Starting");
  server.Start(Server::Srv_TCP);
  msg(0, "Terminating");
  return 0;
}
