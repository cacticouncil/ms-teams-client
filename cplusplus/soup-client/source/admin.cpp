#include <string>

#include <libsoup/soup.h>

#include "../include/admin.h"

//added skypeSpacesToken to login/credentials
//name and description currently inoperable
void createTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, std::string &description){
    //initialize soup message with url and method
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/create";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{\"alias\":\"ZestyTeam\",\"displayName\":\"ZestyTeam\",\"displayNamePrefix\":null,\"displayNameSuffix\":null,\"description\":\"\",\"validationRequired\":false,\"isTenantWide\":false,\"accessType\":1}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //auth
    std::string tokenstr = "Bearer " + skypeSpacesToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    soup_session_queue_message(session,msg,createTeamCallback,loop);
}

//callback to execute after message sent
void createTeamCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
        g_printerr("Response: %s\n",msg->response_body->data);
    }

    GMainLoop *loop = (GMainLoop *)user_data;
    g_main_loop_quit(loop);
}

//not functional atm
void createChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &teamId, std::string &name, std::string &description){
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/"+teamId+"/channels";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{\"displayName\":\"Test Channel\",\"description\":\"\",\"groupId\":\"3c0db051-2932-4bac-8fff-38a23e2febe2\"}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //auth
    std::string tokenstr = "Bearer " + skypeSpacesToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    soup_session_queue_message(session,msg,createTeamCallback,loop);
}