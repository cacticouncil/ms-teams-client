#include <iostream>
#include <libsoup/soup.h>

#include "../include/fetch.h"

//fetch teams data asynchronously working
//In the future a function pointer would be passed here to the callback rather than having the callback here. this is temporary.
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, SoupSessionCallback callback, GPtrArray* callback_data){
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    std::string tokenstr = "Bearer " + chatSvcAggToken;

    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,callback,(gpointer)loop/* callback_data */);//loop);  //fetchTeamsCallback
}

//This function is used to obtain the messages associated with a specific team channel
//This can be used to figure out which is the latest message in the channel etc

//void fetchChannelMessages(SoupSession* session, std::string& chatSvcAggToken,  GMainLoop* loop, std::string& teamId, std::string& channelId, int pageSize) //old
void fetchChannelMessages(SoupSession* session, std::string& chatSvcAggToken, GMainLoop* loop, /* Team* team */std::string &teamId, /* Channel* channel */std::string &channelId, int pageSize,  SoupSessionCallback callback, GPtrArray* callback_data){
    /* std::string teamId= team->GetTeamId();
    std::string channelId=channel->GetChannelId(); */
    //formulating the url
    std::string url = "https://teams.microsoft.com/api/csa/api/v2/teams/" + teamId + "/channels/" + channelId;
    url += "?";
    url += "pageSize=" + std::to_string(pageSize);
    if(teamId == channelId) url += "&filterSystemMessage=true";
    //Initializing the mesage after url is fully constructed
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET, url.c_str());

    //auth
    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    
    soup_session_queue_message(session,msg,callback,(gpointer)loop);//callback_data);//fetchChannelMessagesCallback

}

//Given an array of oid values, this function returns the information associated with
void fetchUsersInfo(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, /* std::vector<User*>* userVect */std::vector<std::string> &userIds, SoupSessionCallback callback, GPtrArray* callback_data){
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/users/fetchShortProfile?isMailAddress=false&enableGuest=true&includeIBBarredUsers=true&skypeTeamsInfo=true";
    
    //Constructing the string array with the users' oids
    std::string userIdsStr= "[";
    for (int i=0; i< /* userVect->size() */userIds.size(); i++){
        std::string v =  userIds[i];//userVect->at(i)->GetUserOid();// GetUserOid();
        userIdsStr+= "\"" + v + "\"" ;
        if(i!=/* userVect->size() */userIds.size()-1){
            userIdsStr+=",";
        }
    }
    userIdsStr+= "]";
   
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST, url.c_str());
    std::string payload = userIdsStr;
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    soup_session_queue_message(session,msg,callback,(gpointer)callback_data);
}