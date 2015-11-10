/*
 * server.cpp    
 *  g++ -o server server.cpp
 *  ./server ip port
 * time:2015-10-27
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <map>
#include <set> 
#include <utility>
#include <vector>
#include "duckchat.h"

#define BUFLEN 1024
using namespace std;

struct sockaddr rec_addr;
socklen_t fromlen = sizeof(rec_addr);
int sockfd;
struct addrinfo *addr_ar;
map<string,vector<string> > usr_tlk_chan;
multimap<string,struct sockaddr_in> user_to_addr_strct;
map<string,vector<pair<string,struct sockaddr_in> > > chan_tlk_user;
vector<string> channels;

string get_user_of_curr_addr();
int check_addr_eq(struct sockaddr_in, struct sockaddr_in);
struct sockaddr_in get_addr_struct();
int check_valid_addr();
int say_req(struct request_say*);
int login_req(struct request_login*);
int logout_req(struct request_logout*);
int error_msg(struct sockaddr_in,string);
int join_req(struct request_join*);
int leave_req(struct request_leave*);
int list_req(struct request_list*);
int who_req(struct request_who*);
int read_request_type(struct request*, int);
int create_bind_socket(char*, char*);

int main(int argc, char **argv)
{
    addr_ar = NULL;
    sockfd = 0;
    if((argc-1) != 2) { 
        cout << "need at least 2 argument: server IP and port.\n";
        return -1;
    }
    
    create_bind_socket(argv[1], argv[2]);
	
    while(1)
    {
        char *buf = new char[BUFLEN];
        struct request *requests = (struct request*)malloc(sizeof(struct request*) + BUFLEN);  
        int bal = 0;
        bal = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&rec_addr, &fromlen);
        if(bal > 0) {
            requests = (request*) buf;
            read_request_type(requests, bal);
        } 
       delete[] buf;   
    }
    return 0;
}


//returns string of username of current request address
string get_user_of_curr_addr()
{ 
    struct sockaddr_in* address = (struct sockaddr_in*)&rec_addr;
    string aTmp = "";     
    multimap<string,struct sockaddr_in>::iterator i;
    for(i=user_to_addr_strct.begin(); i != user_to_addr_strct.end(); i++) {
        if(check_addr_eq(i->second,*address) == 0) {
            aTmp = i->first;
        }
    }
    return aTmp;
}

int check_addr_eq(struct sockaddr_in a, struct sockaddr_in b)
{
    char *addrA = (char*)malloc(sizeof(char)*BUFLEN);
    char *addrB = (char*)malloc(sizeof(char)*BUFLEN);
    inet_ntop(AF_INET, &(a.sin_addr), addrA, BUFLEN);
    inet_ntop(AF_INET, &(b.sin_addr), addrB, BUFLEN);
    string stringA = addrA;
    string stringB = addrB;
    if(stringA == stringB) {
        int portA = a.sin_port;
        int portB = b.sin_port;
        if(portA == portB) {
            return 0;
        }
    }
    return -1;
}

struct sockaddr_in get_addr_struct() 
{
    struct sockaddr_in* address = (struct sockaddr_in*)&rec_addr;
    return *address;
}

//check if current request address is valid or exist in map
int check_valid_addr() 
{
    struct sockaddr_in* address = (struct sockaddr_in*)&rec_addr;
    multimap<string,struct sockaddr_in>::iterator i;
    for(i=user_to_addr_strct.begin(); i != user_to_addr_strct.end(); i++) {
        if(check_addr_eq(i->second,*address)==0) {
            return 0;
        }
    }
    return -1;
}

//handle say requests
int say_req(struct request_say *rs)
{
    string channel = rs->req_channel;
    string message = rs->req_text;
    string username = get_user_of_curr_addr();
    struct sockaddr_in fromAddr = get_addr_struct();
    map<string,vector<pair<string,struct sockaddr_in> > >::iterator hit = chan_tlk_user.find(channel);
    if(hit == chan_tlk_user.end()) {
        return -1; 
    }
    vector<pair<string,struct sockaddr_in> > tmpU = hit->second;
    for(int i=0; i<tmpU.size(); i++) {
        struct sockaddr_in address;
        void *goData;
        address = tmpU[i].second;
        struct text_say msg;
		memset(&msg,0,sizeof(msg));
        msg.txt_type= TXT_SAY;
        strncpy(msg.txt_username, username.c_str(), strlen(username.c_str()));
        strncpy(msg.txt_text, message.c_str(), strlen(message.c_str()));
        strncpy(msg.txt_channel, channel.c_str(), strlen(channel.c_str()));
        int size = sizeof(struct sockaddr*);
        goData = &msg;
        int res= sendto(sockfd, goData, sizeof(msg), 0, (struct sockaddr*)&address, sizeof(address));
        if (res == -1) {
            return -1;
        }
    }
    tmpU.clear();
	cout << "server: " << username << " s sends say message in " << channel << endl;
    return 0;
}

//handle login requests
int login_req(struct request_login *rl)
{
    string username = rl->req_username;
    struct sockaddr_in strctAddr = get_addr_struct();
    user_to_addr_strct.insert(user_to_addr_strct.end(), pair<string, struct sockaddr_in>(username, strctAddr));
	cout << "server: " << username <<" logs in" << endl;
    return 0;
}

//handle logout requests
int logout_req(struct request_logout *rl)
{
    string username = get_user_of_curr_addr();
    multimap<string, struct sockaddr_in>::iterator sockIt = user_to_addr_strct.find(username);
    user_to_addr_strct.erase(sockIt);
    map<string,vector<string> >::iterator git;
    git = usr_tlk_chan.find(username);
    if(git != usr_tlk_chan.end()) {
        usr_tlk_chan.erase(username);
    }
    //erase user on channels in chan_tlk_user
    for(int ick=0; ick<channels.size(); ick++) {
        map<string,vector<pair<string,struct sockaddr_in> > >::iterator itck = chan_tlk_user.find(channels[ick]);
        vector<pair<string,struct sockaddr_in> > usersC = itck->second;
        for(int j=0; j<usersC.size(); j++) {
            if(usersC[j].first == username) {
                usersC.erase(usersC.begin()+j);
            }
        }
        chan_tlk_user.erase(itck);
        chan_tlk_user.insert(pair<string,vector<pair<string,struct sockaddr_in> > >(channels[ick],usersC));
    }
	cout << "server: " << username <<" logs out" << endl;
    return 0;
}

//error message
int error_msg(struct sockaddr_in addr, string msg)
{
    ssize_t bytes;
    void *send_data;
    size_t len;
    struct text_error send_msg;
    send_msg.txt_type = TXT_ERROR;
    const char* str = msg.c_str();
    strcpy(send_msg.txt_error, str);
    send_data = &send_msg;
    len = sizeof send_msg;
    struct sockaddr_in send_sock = addr;
    bytes = sendto(sockfd, send_data, len, 0, (struct sockaddr*)&send_sock, sizeof send_sock);
    if(bytes < 0) {
        return -1;
    }
    else {
       return 0;
    }
}


//handle join requests
int join_req(struct request_join *rj)
{
    //create tmp vars for username and channel of request
    string chan = (string)rj->req_channel;
    string user = get_user_of_curr_addr();
    struct sockaddr_in reqAddr = get_addr_struct();
    map<string,vector<pair<string,struct sockaddr_in> > >::iterator it = chan_tlk_user.find(chan);
    vector<pair<string,struct sockaddr_in> > usersC;
    if(it == chan_tlk_user.end()) {
        //new channel
        usersC.insert(usersC.begin(), pair<string,struct sockaddr_in>(user,reqAddr));
        chan_tlk_user.insert(pair<string,vector<pair<string,struct sockaddr_in> > >(chan, usersC));
        channels.push_back(chan);
    } else {
        //old channel
        it = chan_tlk_user.find(chan);
        usersC = it->second;
        for(int i=0; i<usersC.size(); i++) {
            if(usersC[i].first == user) {
                return -1;
            }
        }
        usersC.insert(usersC.begin(), pair<string,struct sockaddr_in>(user, reqAddr));
        chan_tlk_user[chan] = usersC;
    }
    vector<string> chanTlk = usr_tlk_chan[user];
    chanTlk.insert(chanTlk.begin(), chan);
    usr_tlk_chan[user] = chanTlk;
	cout << "server: " << user << " joins channel " << chan <<endl;
    return 0;
}

//handle leave requests
int leave_req(struct request_leave *rl)
{
    string username = get_user_of_curr_addr();
    struct sockaddr_in reqAddr = get_addr_struct();
    string chaNel = (string)(rl->req_channel);
    multimap<string, struct sockaddr_in>::iterator ui = user_to_addr_strct.find(username);
    struct sockaddr_in address = ui->second;
    map<string,vector<pair<string,struct sockaddr_in> > >::iterator vi;
    if((vi = chan_tlk_user.find(chaNel)) == chan_tlk_user.end()) {
        return -1;
    }
    vector<pair<string,struct sockaddr_in> > v = vi->second;
    for(int vecI=0; vecI<v.size(); vecI++) {
        if(v[vecI].first == username) {
            if(check_addr_eq(v[vecI].second, reqAddr) == 0) {
                v.erase (v.begin()+vecI);
            } 
        }
    }
    chan_tlk_user.erase(vi);
    if(v.size() != 0) {
        chan_tlk_user.insert(pair<string,vector<pair<string,struct sockaddr_in> > >(chaNel,v));
        return 0;
    } else {
        for(int i=0; i<channels.size(); i++) {
            if(channels[i] == chaNel) {
                channels.erase(channels.begin()+i);
            }
        }
    }
    vector<string> chanTlk = usr_tlk_chan[username];
    for(int vv=0; vv<chanTlk.size(); vv++) {
        if(chanTlk[vv] == chaNel) {
            chanTlk.erase(chanTlk.begin()+vv);
        }
    }
    usr_tlk_chan[username] = chanTlk;
	cout << "server: " << username << " leaves channel " << chaNel << endl;    
	return 0;
}

//handle list requests
int list_req(struct request_list *rl)
{
    string username = get_user_of_curr_addr();
    struct sockaddr_in address; 
    multimap<string, struct sockaddr_in>::iterator ui = user_to_addr_strct.find(username);
    int numCHAN = channels.size();
    address = ui->second;
    struct text_list *msg = (struct text_list*)malloc((sizeof(struct text_list)+(numCHAN *sizeof(struct channel_info))));
    msg->txt_type= TXT_LIST;
    msg->txt_nchannels = numCHAN;
    for (int i = 0; i<channels.size(); i++) {
        const char* tstr = channels[i].c_str();
        strcpy(((msg->txt_channels)+i)->ch_channel, tstr);
    }
    int size = sizeof(struct sockaddr);
    int res= sendto(sockfd, msg,  (sizeof(struct text_list)+(numCHAN *sizeof(struct channel_info))), 0, (struct sockaddr*)&address, size);
    if (res == -1) {
        return -1;
    }
    free(msg);
	cout << "server: " << username <<" lists channels" << endl;
    return 0;
}

//handle who requests
int who_req(struct request_who *rw)
{
    string username = get_user_of_curr_addr();
    struct sockaddr_in address; 
    string chaNel = (string)(rw->req_channel);
    multimap<string, struct sockaddr_in>::iterator ui = user_to_addr_strct.find(username);
    map<string,vector<pair<string,struct sockaddr_in> > >::iterator vi;
    if((vi = chan_tlk_user.find(chaNel)) == chan_tlk_user.end()) {
        return -1;
    }
    int numCHAN = (vi->second).size();
    vector<pair<string,struct sockaddr_in> > v = vi->second;
    address = ui->second;
    struct text_who *msg = (struct text_who*)malloc(sizeof(struct text_who)+(numCHAN* sizeof(struct user_info)));
    msg->txt_type= TXT_WHO;
    msg->txt_nusernames = numCHAN;
    const char* str = chaNel.c_str();
    strcpy(msg->txt_channel, str);
    for (int i = 0; i<v.size(); i++) {
        const char* tstr = v[i].first.c_str();
        strcpy(((msg->txt_users)+i)->us_username, tstr);
    }
    int size = sizeof(struct sockaddr);
    int res= sendto(sockfd, msg,  (sizeof(struct text_who)+(numCHAN* sizeof(struct user_info))), 0, (struct sockaddr*)&address, size);
    if (res == -1) {
        return -1;
    }
    free(msg);
	cout << "server: " << username << " list users in channel " << chaNel << endl;
    return 0;
}

int read_request_type(struct request *r, int b) 
{
    int fin = 0;
    int netHost = 0;
    netHost = ntohl(r->req_type);
    //check if addres is a crazy number or normal
    if(netHost > 10 || netHost < 0) {
       netHost = r->req_type;
    }
    //check if request address is valid
    if(netHost != 0) {
        if(check_valid_addr() == -1) {
            return -1;
        } 
    }
    switch(netHost) {
        case REQ_LOGIN:
            if(sizeof(struct request_login) == b) {
                fin = login_req((struct request_login*) r);
                break;
            } else {
                break;
            } 
        case REQ_LOGOUT:
            if(sizeof(struct request_logout) == b) {
                fin = logout_req((struct request_logout*) r);
                break;
            } else {
                break;
            }   
        case REQ_JOIN:
            if(sizeof(struct request_join) == b) {
                fin = join_req((struct request_join*) r);
                break;
            } else {
                break;
            }      
        case REQ_LEAVE:
            if(sizeof(struct request_leave) == b) {
                fin = leave_req((struct request_leave*) r);
                break;
            } else {
                break;
            }
        case REQ_SAY:
            if(sizeof(struct request_say) == b) {
                fin = say_req((struct request_say*) r);
                break;
            } else {
                break;
            }
        case REQ_LIST:
            if(sizeof(struct request_list) == b) {
                fin = list_req((struct request_list*) r);
                break;
            } else {
                break;
            }
        case REQ_WHO:
            if(sizeof(struct request_who) == b) {
                fin = who_req((struct request_who*) r);
                break;
            } else {
                break;
            }
        default:
            break;
    }
    return fin;
}

int create_bind_socket(char* ip, char* port)
{
    struct addrinfo addressTmp;
    memset(&addressTmp, 0, sizeof addressTmp);
    addressTmp.ai_family = AF_INET;
    addressTmp.ai_socktype = SOCK_DGRAM;
    addressTmp.ai_flags = AI_PASSIVE;
    int check = 0;
    if((check = getaddrinfo(ip, port, &addressTmp, &addr_ar))!= 0)
    {
        return false;
    }
    if((sockfd = socket(addr_ar->ai_family, addr_ar->ai_socktype, addr_ar->ai_protocol)) == -1)
    {
        return false;
    }
    if(bind(sockfd, addr_ar->ai_addr, addr_ar->ai_addrlen) == -1)
    {
        return false;
    }
    return true;
}
