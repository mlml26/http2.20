#include <cstdlib>
#include "socket.h"

int main(void){
    Server s("152.3.77.9", 12345);
    int serverSockfd = s.connectListenFromServer();
    while(1){
        std::vector<std::vector<char> > accInfo = s.acceptProcessFromServer(serverSockfd);
        int accServerfd = accInfo[0][0];
        std::vector<char> clientText = accInfo[1];
        std::vector<std::vector<char> > requestInfo = s.parseHttpRequest(clientText);
        std::vector<char> requestLine = requestInfo[0];
        std::vector<char> serverIpAddr = requestInfo[1];
        Client c(serverIpAddr.data(), 80);
        int clientSockfd = c.connectFromClient(requestLine, accServerfd);
        close(accServerfd);
    }

    return EXIT_SUCCESS;
}
