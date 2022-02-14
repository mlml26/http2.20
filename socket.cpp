#include "socket.h"
#include "response.h"

int Socket::createSocket(){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        
        if(sockfd == -1){
            perror("can't create a socket.");
            exit(EXIT_FAILURE);
        }

        return sockfd;
}

int Server::connectListenFromServer() {
    //进行连接
    int server_sockfd = createSocket();
    std::cout << "server_sockfd=" << server_sockfd << "\n";

    //发送客户端连接请求，封装服务器信息
    struct sockaddr_in server_addr;
    socklen_t len_addr = sizeof(server_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ipAddr); //inet_network
    server_addr.sin_port = htons(server_port);

    //设置端口复用
    int opt = 1;
    int ret_setsock = setsockopt(server_sockfd, SOL_SOCKET,SO_REUSEADDR, (const void *)&opt, sizeof(opt));
    if(ret_setsock == -1){
        perror("The server can't set the socket successfully");
        exit(EXIT_FAILURE);
    }

    //将socket与服务器网络信息结构体绑定
    int ret_bind = bind(server_sockfd, (struct sockaddr *)&server_addr, len_addr);
    if(ret_bind == -1){
        perror("The server can't bind with the socket");
        exit(EXIT_FAILURE);
    }

    //将socket设置为被动监听状态
    int ret_listen = listen(server_sockfd, 20);
    if(ret_listen == -1){
        perror("The server can't set the socket to listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "listen success\n"; 

    return server_sockfd;
}

//接收客户端的连接，然后去处理连接
std::vector<std::vector<char> > Server::acceptProcessFromServer(int server_sockfd){
    std::vector<std::vector<char> > acceptInfo;

    //阻塞等待客户端的连接accept
    struct sockaddr_in client_addr;
    socklen_t len_addr = sizeof(client_addr);

    int acceptfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &len_addr);
    std::cout << "accept success\n";
    if(acceptfd == -1){
        perror("The server can't get the accept from the client");
        exit(EXIT_FAILURE);
    }
    std::vector<char> v_acceptfd;
    v_acceptfd.push_back(acceptfd);
    acceptInfo.push_back(v_acceptfd);

    //打印连接的客户端的信息
    std::cout << "ip:" << inet_ntoa(client_addr.sin_addr) << ", port:" << ntohs(client_addr.sin_port) << "\n";

    //进行通信
    //接收数据
    std::vector<char> client_text(MAX_TEXTLEN); 
    int ret_recv = recv(acceptfd, &client_text[0], MAX_TEXTLEN, 0);

    if(ret_recv == -1){
        perror("The server can't receive data from the client");
        exit(EXIT_FAILURE);
    }

    std::cout << "from client: " << client_text.data() << "\n";
    acceptInfo.push_back(client_text);

    return acceptInfo;
}
    
std::vector<std::vector<char> > Server::parseHttpRequest(std::vector<char> client_text){
    std::vector<std::vector<char> > proxy_serverInfo;

    std::string client_str(client_text.data());
    std::size_t pos = client_str.find("\r\n");      // position of "\r\n" in str
    std::string requestLine = client_str.substr(0, pos);     // get from "live" to the end
    std::string requestHeader = client_str.substr(pos);
    std::cout << "the request line is: " << requestLine << "\n";
    std::cout << "the request header is: " << requestHeader << "\n";

    std::stringstream ss(requestLine);
    std::string word;
    std::vector<std::string> requestInfo;
    while(ss >> word){
        requestInfo.push_back(word);
    }

    //获取method
    std::string method_str = requestInfo[0]; //要判断是GET,POST,CONNECT
    
    //获取protocol以及后面的内容
    std::string proto_str = requestInfo[2];

    //获取hostname和filename
    std::string url_str = requestInfo[1];
    std::size_t pos_url = url_str.find("//");
    std::string url_host_file = url_str.substr(pos_url+2);
    std::cout << "url_host_file is :" << url_host_file << "\n";

    //找到hostname
    std::size_t pos_host = url_host_file.find("/");
    std::string url_host = url_host_file.substr(0, pos_host);
    //找到filename, 如果只有host没有file，则用/来表示
    std::string url_file = url_host_file.substr(pos_host);
    std::cout << "url_host = " << url_host << "\n";
    std::cout << "url_file = " << url_file << "\n";

    std::stringstream ss_request;
    ss_request << method_str << " " << url_file << " " << proto_str << requestHeader;
    std::string requestServerInfo = ss_request.str();
    std::cout << "requestServerInfo is: " << requestServerInfo << "\n";
    std::vector<char> v_requestServerInfo(requestServerInfo.begin(), requestServerInfo.end());
    //添加要发给真正客户端的请求行
    proxy_serverInfo.push_back(v_requestServerInfo);

    struct hostent * ipInfo = gethostbyname(url_host.c_str());
    //std::cout << ipInfo->h_length << " " << ipInfo->h_addr_list << "\n";

    char * ip_addr = inet_ntoa(*(struct in_addr *)*ipInfo->h_addr_list);
    std::cout << "ip address:"<< ip_addr << "\n";
    std::vector<char> v_ip_addr(ip_addr, ip_addr+20);
    //添加真正服务端的IP地址
    proxy_serverInfo.push_back(v_ip_addr);

    return proxy_serverInfo;
}

    // GET http://www.testingmcafeesites.com/index.html HTTP/1.1
    // User-Agent: PostmanRuntime/7.29.0
    // Accept: */*
    // Postman-Token: 9b9545d8-0392-4eea-91d4-91dbe4219e71
    // Host: www.testingmcafeesites.com
    // Accept-Encoding: gzip, deflate, br
    // Connection: keep-alive


    /*strcat(client_text, "*****");

    //发送数据
    int ret_send = send(acceptfd, client_text, MAX_TEXTLEN, 0);

    if(ret_send == -1){
        perror("The server can't send text");
        exit(EXIT_FAILURE);
    }*/

    //关闭socket文件描述符
    // close(acceptfd);
    // close(server_sockfd);

// HTTP/1.1 200 OK
// Content-Type: text/html
// Content-Length: 3151
// Connection: keep-alive
// Content-Encoding: gzip
// Last-Modified: Mon, 22 Jun 2020 20:38:18 GMT
// Accept-Ranges: bytes
// ETag: "c7371810d548d61:0"
// Vary: Accept-Encoding
// Server: Microsoft-IIS/8.5
// X-Powered-By: ASP.NET
// SN: DMINIWEB02
// Date: Sun, 13 Feb 2022 00:42:27 GMT

int Client::connectFromClient(std::vector<char> client_text, int server_sockfd) {
    //进行连接
    int client_sockfd = createSocket();
    std::cout << "client_sockfd=" << client_sockfd << "\n";

    //发送客户端连接请求，封装服务器信息
    struct sockaddr_in server_addr;
    socklen_t len_addr = sizeof(server_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ipAddr);  //inet_network
    server_addr.sin_port = htons(server_port);

    int ret_connect = connect(client_sockfd, (struct sockaddr *)&server_addr, len_addr);
    if(ret_connect == -1){
        perror("The client can't connect to the server");
        exit(EXIT_FAILURE);
    }
    std::cout << "Connected!!!\n";

    //进行通信
    //发送数据
    // char client_text[MAX_TEXTLEN];
    // std::string client_string = "";
    // std::cin >> client_string;
    // client_string.copy(client_text, client_string.length(), 0);
    // *(client_text+client_string.length()) = '\0';
    
    int ret_send = send(client_sockfd, client_text.data(), MAX_TEXTLEN, 0);

    if(ret_send == -1){
        perror("The client can't send text");
        exit(EXIT_FAILURE);
    }

    //接收数据
    char server_text[MAX_TEXTLEN] = "";
    int ret_recv = recv(client_sockfd, server_text, MAX_TEXTLEN, 0);

    if(ret_recv == -1){
        perror("The client can't receive data from the server");
        exit(EXIT_FAILURE);
    }

    //std::cout << server_text << "\n";

    // int delimiter = server_text.find("\r\n");
    // std::string test_text = server_text.substr(0, delimiter);
    // std::cout << test_text << "\n";

    /*    std::vector<char> v(server_text, server_text+MAX_TEXTLEN);
    const char *crlf2 = "\r\n\r\n";
    std::vector<char>::iterator it = std::search(v.begin(), v.end(), crlf2, crlf2+strlen(crlf2));
    std::vector<char> v2(v.begin(), it+strlen(crlf2));
    std::cout << v2.data() << "\n";
    */
//     v2_ss << "HTTP/1.1 200 OK\r\n"<<"Content-Type: text/html;charset=utf-8\r\n"
//     <<"Content-Length: 3159\r\n"
// <<"Connection: keep-alive\r\n"
// <<"Content-Encoding: gzip\r\n"
// <<"Last-Modified: Mon, 22 Jun 2020 20:38:18 GMT\r\n"
// <<"Accept-Ranges: bytes\r\n"
// <<"ETag: \"c7371810d548d61:0\""
// <<"Vary: Accept-Encoding\r\n"
// <<"Server: Microsoft-IIS/8.5\r\n"
// <<"X-Powered-By: ASP.NET\r\n"
// <<"SN: DMINIWEB01\r\n"
// <<"Date: Sun, 13 Feb 2022 23:40:39 GMT\r\n\r\n";
    
    //send(server_sockfd, &v2[0], v2.size(), 0);
    //    Response rsp(v2);
    //cout <<"!!!!!!!!!!!!!!AAAA" << rsp.getResponse() << "AAA!!!!!!!!!!!!!!" << endl;
    //std::vector<char> v3(it+strlen(crlf2), v.end());
    //std::string test_str = "<HTML><HEAD><TITLE>Your Title Here</TITLE></HEAD><BODY><H1>This is a Header</H1></BODY></HTML>\r\n";
    //std::cout << "=======responseHeader is: " << responseHeader << "\n";
    //rsp.getContentLength() == std::string contentLength("Content-Length: ");
    /* int pos_content = responseHeader.find(contentLength);
    std::string content_str = responseHeader.substr(pos_content+contentLength.size());
    std::string crlf("\r\n");
    int pos_content_end = content_str.find(crlf);
    int contenLEN = atoi(content_str.substr(0, pos_content_end).c_str());
    */
    //std::cout << "content_str is: " << rsp.getContentLength() * 1.01 << "\n";
    send(server_sockfd, server_text, ret_recv, 0);
    //std::cout << "*******" << v3.size() << "\n";

    //关闭socket文件描述符
    // close(client_sockfd);

    // close(server_sockfd);
    return client_sockfd;
}
