#ifndef request_hpp
#define request_hpp

#include <stdio.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <netdb.h>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
class Request{
private:
    string requestLine;
    string requestHeader;//full 
    
public:
    Request(){}
    Request(const Request& rhs) {
        requestLine = rhs.requestLine;
        requestHeader = rhs.requestHeader;
    }
//    Response(const Response& rhs) {
//        header = rhs.header;
//        fulltext = rhs.fulltext;
//    }
    string getRequestLine(){
        return requestLine;
    }
    string getRequestHeader(){
        return requestHeader;
    }
    // GET http://www.testingmcafeesites.com/index.html HTTP/1.1
        // User-Agent: PostmanRuntime/7.29.0
        // Accept: */*
        // Postman-Token: 9b9545d8-0392-4eea-91d4-91dbe4219e71
        // Host: www.testingmcafeesites.com
        // Accept-Encoding: gzip, deflate, br
        // Connection: keep-alive
    
//    url_host_file is :www.testingmcafeesites.com/index.html
//    url_host = www.testingmcafeesites.com
//    url_file = /index.html
//    requestServerInfo is: GET /index.html HTTP/1.1
//    ip address:161.69.49.22
//    client_sockfd=5
    //requestServerInfo:requestLine
    Request(vector<char> header) {
        requestHeader.insert(requestHeader.begin(), header.begin(), header.end());
        int pos = requestHeader.find("\r\n");
        requestLine = requestHeader.substr(0, pos);
    }
    Request(string line, string header) {
        requestLine.insert(requestLine.begin(), line.begin(), line.end());
        requestHeader.insert(requestHeader.begin(), header.begin(), header.end());
    }
    string getMethod() {
        int pos = requestLine.find(" ");
        return requestLine.substr(0, pos);
    }
    string getURL() {
        int p1 = requestLine.find(" ");
        int p2 = requestLine.find(" ", p1 + 1);
        return requestLine.substr(p1+1, p2-p1-1);
    }
    
};


#endif /* request_hpp */
