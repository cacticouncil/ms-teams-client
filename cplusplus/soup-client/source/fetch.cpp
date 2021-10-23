#include <string>

#include <libsoup/soup.h>

#include "../include/fetch.h"
//#include <json-glib/json-glib.h>

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
