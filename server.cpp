#include <cstdlib>
#include "socket.h"
#include "cache.h"
string currentTime() {
    time_t now = time(0);
    char * dt = ctime(&now);
    tm *gmtm = gmtime(&now);
    dt = asctime(gmtm);
    return string(dt);
    //cout << string(dt) << endl;
}

int main(void){
    Server s("152.3.77.9", 12345);
    int serverSockfd = s.connectListenFromServer();
    Cache cache(1000);
    while(1){
        std::vector<std::vector<char> > accInfo = s.acceptProcessFromServer(serverSockfd);
        int accServerfd = accInfo[0][0];
	std::vector<char> clientIpAddr = accInfo[1];
	std::vector<char> clientText = accInfo[2];
        std::vector<std::vector<char> > requestInfo = parseHttpRequest(clientText);
        int method = requestInfo[0][0];
        std::vector<char> requestLines = requestInfo[1];
        std::vector<char> serverIpAddr = requestInfo[2];
        std::cout << "!!!!!!! method is: " << method << "\n";
        int port;
        if(method==0 || method==1){ //GET, POST
            port = 80;
        }
        else{ //CONNECT
            port = 443;
        }
	//	Cache cache(1000);
        Client c(serverIpAddr.data(), port);
        //int clientSockfd = c.connectFromClient(requestLines, accServerfd);

        //
        int clientSockfd = c.connectFromClient(); 
        c.sendFromClient(clientSockfd, requestLines);
	Request input(clientText);
<<<<<<< HEAD
	cout << "@@@@@@ request: " <<input.getRequestLines() << "@@@@@@@" << endl;
=======
	cout << "@@@@@@ request: " <<input.getRequestHeader() << "@@@@@@@" << endl;
>>>>>>> 381c680ca20c0a02b9bf3d0888ac9f4e7183ba9c
        if(method==0 || method==1){
	  cout <<"Logfile: "<< input.getRequestLine() << " from " << clientIpAddr.data() << " @ " << currentTime() << endl;
	  if(cache.inCache(input)) {
	    cout << "~~~~~~~~~~~in cache~~~~~~~~~~~~" << endl;
	    Response rsp = cache.get(input);
	    cout << "----------------old response header:"<< rsp.getHeader()<< "\n------------"<< endl;
	    if(rsp.needRevalidation()) {
                    rsp = cache.revalidation(input, rsp, clientSockfd);
                }
<<<<<<< HEAD
            else {
               //logfile
               cout << "in cache, valid" <<endl;
            }
=======
                else {
                    //logfile
                    cout << "in cache, valid" <<endl;
                }
>>>>>>> 381c680ca20c0a02b9bf3d0888ac9f4e7183ba9c
                send(accServerfd, rsp.getResponse().data(), rsp.getResponse().size(), 0);
	  }
	  else{
	    cout << "not in cache." << endl;
	    std::vector<std::vector<char> > responseInfo = sendDataByGetPost(clientSockfd, accServerfd);
            std::vector<char> responseLines = responseInfo[0];
            std::vector<char> responseBody = responseInfo[1];
	    //cout << "***responseLines: " << responseLines.data() << "*********" << endl;
	    cout << "***response_fulltext: " << responseBody.data() << "**********" <<endl;
	    Response newrsp(responseBody);
            cache.storeResponse(input, newrsp);
            cout << "store in cache" << endl;
	  }
	 
	  //	  std::vector<std::vector<char> > responseInfo = sendDataByGetPost(clientSockfd, accServerfd);
	  //std::vector<char> responseLines = responseInfo[0];
	  //std::vector<char> responseBody = responseInfo[1];
        }
        else{
            //do something
        }
    
        close(accServerfd);
        close(clientSockfd);
     }

    return EXIT_SUCCESS;
}
