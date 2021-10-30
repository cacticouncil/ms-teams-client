#include <string>

#include <libsoup/soup.h>

#include "../include/fetch.h"


//synchronously fetch teams data
void fetchTeamsSync(SoupSession *session, std::string &chatSvcAggToken){
    //init message with url
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    //append bearer auth token to header
    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    GError *error = NULL;
    GInputStream *stream = soup_session_send(session,msg,NULL,&error);

    void *resbuff = malloc(8192);
    g_input_stream_read(stream,resbuff,8192,NULL,&error);

    if (error) {
        g_printerr ("ERROR: %s\n", error->message);
        g_error_free (error);
    }
    else{
        g_print("Success! Code: %d\n",msg->status_code);

        g_print("Response Buffer: %s\n",(char*)resbuff);

        g_input_stream_close(stream,NULL,&error);
        g_object_unref (msg);
        g_object_unref (session);
    }

    free(resbuff);
}

//fetch teams data asynchronously working
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop){
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    std::string tokenstr = "Bearer " + chatSvcAggToken;
    std::cout<<"Inside fetchTeams, right before calling Async \n"; //Debug Statement
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,fetchTeamsCallback,loop);
}


void fetchTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    if (msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);

	std::ofstream responseFile("fetchTeamsInfo.txt");
	if (responseFile.is_open()) {
		responseFile << msg->response_body->data;
		responseFile.close();
	}
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    GMainLoop *loop = (GMainLoop *) user_data;
    g_main_loop_quit(loop);
}

//Async fetch channel messages
void fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize, GMainLoop* loop, SoupSession* session){
    //formulating the url
    std::string url = "https://teams.microsoft.com/api/csa/api/v2/teams/" + teamId + "/channels/" + channelId;
    url += "?";
    url += "pageSize=" + std::to_string(pageSize);
    if(teamId == channelId) url+= "&filterSystemMessage=true";

    //Initializing the mesage after url is fully constructed
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET, url.c_str());

    //auth
    std::string tokenstr = "Bearer " + chatSvcAggToken;

    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,fetchChannelMessagesCallback,loop);

}

void fetchChannelMessagesCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    if (msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);

        std::ofstream responseFile("fetchChannelMessagesInfo.txt");
        if (responseFile.is_open()) {
                responseFile << msg->response_body->data;
                responseFile.close();
        }
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    GMainLoop *loop = (GMainLoop *) user_data;
    g_main_loop_quit(loop);
}

void fetchUsersInfo(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, std::vector<std::string>& userIds){
    //might also need the team? I'm not exactly sure about that.
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/users/fetchShortProfile?isMailAddress=false&enableGuest=true&includeIBBarredUsers=true&skypeTeamsInfo=true";
    
    std::string userIdsStr= "[";
    for (int i= 0; i<userIds.size(); i++){
        std::string v = userIds[i];
        userIdsStr+= "\"" + v + "\"" ;
        if(i!=userIds.size()-1){
            userIdsStr+=",";
        }
    }
    userIdsStr+= "]";
    std::cout <<"Constructed user string" <<userIdsStr<< "\n";

    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST, url.c_str());
    std::string payload = userIdsStr;
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,fetchUsersInfoCallback,loop);
}


void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    
    if (msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);

        // std::ofstream responseFile("fetchUsersInfo.txt");
        // if (responseFile.is_open()) {
        //         responseFile << msg->response_body->data;
        //         responseFile.close();
        // }
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    GMainLoop *loop = (GMainLoop *) user_data;
    g_main_loop_quit(loop);
}