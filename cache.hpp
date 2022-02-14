#include <map>
#include <algorithm>
#include <mutex>
#include <thread>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>
using namespace std;

class Cache{
  //  list<string> string:response
  unordered_map<string, list<string> > cache;
  int capacity;
  int size;
public:
  Cache(int capacity):capacity(capacity), size(0);
  Cache& operator =(const Cache & rhs);
  ~Cache();
  string get(string key);
  /*
    void put(string key,string value);
    void addToHead(Node *node);
    void removeNode(Node *node);
    void moveToHead(Node *node);
    void removeTail();
    bool storeResponse(string uri,Response rsp,int id);
    string revalidate(Request request, Response response, int socket,int id);
    string checkIfNoneMatch(Request request,Response response,int socket,string etag,int id);
    string checkIfModifiedSince(Request request,Response response,int socket,string lastModify,int id);
    string reSendRequest(Request request, int socket,int id);
    string checkValidate(Request request,Response response,int socket, string type,string content,int id);
    friend ostream& operator<< (ostream &s, const Cache& rhs);

   */
};
