#include <ctime>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
class Response{
  string response;
 public:
  
  Response(vector<char> v) {
    response.insert(response.begin(), v.begin(), v.end());
  }
  string locate(string s);
  string getResponse(){return response;}
  string getCode();
  string getDate();
  time_t getTime(string _time);
  string getMaxAge();
  string getLastModify();
  string getExpire();
  string getCacheControl();
  string getEtag();
  double getCurrentAge();
  bool is_no_cache();
  bool isFresh();
  bool needRevalidation();
  bool isPrivate();
  long getContentLength();
  bool isChunked();

};
