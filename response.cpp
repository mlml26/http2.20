#include "response.h"
string Response::locate(string s){
  int p1 = response.find(s);
  if(p1 == -1) {return "";}
  int p2 = response.find("\r\n", p1 + 1);
  string res = response.substr(p1 + s.size() + 2, p2 - (p1 + s.size() + 2));
  cout << "$$$$$$$" << res << "$$$$$$" << endl;
  return res;
}
string Response::getDate() {
  return locate("Date");
}

time_t Response::getTime(string _time) {
  tm mytime;
  int p = _time.find("GMT");
  if(p != -1) {
    _time.erase(p-1, 4);
  }
  strptime(_time.c_str(),"%a, %d %b %Y %H:%M:%S", &mytime);
  time_t ans = mktime(&mytime);
  return ans;
}

double Response::getCurrentAge() {
  string date = getDate();
  time_t responseDate = getTime(date);
  time_t now = time(NULL);
  return difftime(now, responseDate);
}
string Response::getCode(){
  int p1 = response.find(" ");
  int p2 = response.find(" ", p1 + 1);
  string ans = response.substr(p1+1, p2-p1-1);
  return ans;
}

string Response::getMaxAge(){
  //检测空格, 在cache control里面
  string cacheControl = getCacheControl();
  if(cacheControl == "") return "";
  int p1 = cacheControl.find("max-age");
  if(p1 == -1) return "";
  int p2 = cacheControl.find(" ", p1+1);
  if(p2 == -1) p2 = cacheControl.find("\r\n", p1+1);
  return cacheControl.substr(p1+8, p2-p1-8);
}
string Response::getLastModify() {
  return locate("Last-Modified");
}
string Response::getExpire() {
  return locate("Expires");
}
string Response::getCacheControl() {
  return locate("Cache-Control");
}
string Response::getEtag() {
  return locate("Etag");
}

bool Response::is_no_cache() {
  string cc = getCacheControl();
  if(cc == "") return false;
  int p = cc.find("no-cache");
  if(p == -1) return false;
  return true;

}

bool Response::isFresh() {
  bool fresh;
  //if max-age exist, compare max-age and current age
  string maxAge = getMaxAge();
  if(maxAge != "") {
    double currentAge = getCurrentAge();
    long max_age = atoi(maxAge.c_str());
    fresh = currentAge < max_age;
    if(!fresh) {
      //放进logfile里       "in cache, requires validation"
    }
    return fresh;
  }
  //if expires exists, compare Expire time and current time
  string expiresline = getExpire();
  if(expiresline != "") {
    time_t expires = getTime(expiresline);
    time_t current = time(NULL);
    fresh = current < expires;
    if(!fresh) {
      //放进logfile里,      "in cache, but expired at " + expires                                                                                          
    }
    return fresh;
  }
  // if Last modify exists, calculate the freshness time, and then compare freshness time and current age;


  string lastModify = getLastModify();
  if(lastModify != "") {
    string lastModify = getLastModify();
    time_t lastModified = getTime(lastModify);
    string date = getDate();
    time_t responseDate = getTime(date);
    //freshness_lifetime =( ResponseTime - LastModified) * 10%                                                    
    double freshness_lifetime = difftime(responseDate, lastModified) * 0.1;
    double currentAge = getCurrentAge();
    fresh	= currentAge < freshness_lifetime;
    if(!fresh){
      //放进logfile里   "in cache, requires validation"                                         
    }
    return fresh;
  }
  return false;
}

long Response::getContentLength() {
  string len = locate("Content-Length");
  cout << len << endl;
  long ans = atoi(len.c_str());
  cout << ans << endl;
  //long ans = stoi(len);
  return ans;
}

bool Response::isPrivate(){
  string cc = getCacheControl();
    if(cc == "") return false;
    int p = cc.find("private");
    if(p == -1) return false;
    return true;
};
