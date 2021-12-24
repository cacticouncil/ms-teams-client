#include <string>
#include <iostream>

#include <libsoup/soup.h>

#include <json-glib/json-glib.h>

#include "../include/admin.h"

//posts propsed name to endpoint; response is team alias
void createTeamName(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, SoupSessionCallback nameValidationCallback){
    //initialize soup message with url and method
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/validateNaming";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{\"displayName\":\""+name+"\",\"displayNamePrefix\":null,\"displayNameSuffix\":null,\"mailNicknamePrefix\":null,\"mailNicknameSuffix\":null,\"groupId\":null}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //auth
    std::string tokenstr = "Bearer " + skypeSpacesToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    soup_session_queue_message(session,msg,nameValidationCallback,loop);
}

//added skypeSpacesToken to login/credentials
//only called from nameValidationCallback of createTeamName, so "Bearer " already prepended to token
void createTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, SoupSessionCallback createTeamCallback){
    //initialize soup message with url and method
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/create";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{\"alias\":\""+name+"\",\"displayName\":\""+name+"\",\"displayNamePrefix\":null,\"displayNameSuffix\":null,\"description\":\"\",\"validationRequired\":false,\"isTenantWide\":false,\"accessType\":1}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //auth
    soup_message_headers_append(msg->request_headers,"Authorization",skypeSpacesToken.c_str());

    soup_session_queue_message(session,msg,createTeamCallback,loop);
}

//unauth err fixed by adding x-skypetoken header
void createChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &teamId, std::string &name, std::string &skypeToken, std::string &groupId, SoupSessionCallback channelCreateCallback){
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/"+teamId+"/channels";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{\"displayName\":\""+name+"\",\"description\":\"\",\"groupId\":\""+/* 3c0db051-2932-4bac-8fff-38a23e2febe2 */groupId+"\"}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //auth
    std::string tokenstr = "Bearer " + skypeSpacesToken;
    soup_message_headers_append(msg->request_headers,"authorization",tokenstr.c_str());
    soup_message_headers_append(msg->request_headers,"x-skypetoken",skypeToken.c_str());

    soup_session_queue_message(session,msg,/* teamCreateCallback */channelCreateCallback,loop);
}

void deleteChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &skypeToken, std::string &teamId, std::string &channelId, SoupSessionCallback deleteCallback){
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/"+teamId+"/channels/"+channelId+"/";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_DELETE,url.c_str());

    std::string tokenstr = "Bearer " + skypeSpacesToken;
    soup_message_headers_append(msg->request_headers,"authorization",tokenstr.c_str());
    soup_message_headers_append(msg->request_headers,"x-skypetoken",skypeToken.c_str());

    soup_session_queue_message(session,msg,deleteCallback,loop);
}

void deleteTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &skypeToken, std::string &teamId, SoupSessionCallback deleteCallback){
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/"+teamId+"/delete";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_DELETE,url.c_str());

    std::string tokenstr = "Bearer " + skypeSpacesToken;
    soup_message_headers_append(msg->request_headers,"authorization",tokenstr.c_str());
    soup_message_headers_append(msg->request_headers,"x-skypetoken",skypeToken.c_str());

    soup_session_queue_message(session,msg,deleteCallback,loop);
}