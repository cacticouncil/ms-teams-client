#include <string>
#include <fstream>
#include <iostream>

#include <libsoup/soup.h>

#include "messaging.h"

int main(){
    std::string skypeToken;
	std::string chatSvcAggToken;

    readCredentials(skypeToken, chatSvcAggToken);
    //remove new line from skypeToken
    skypeToken.erase(skypeToken.size()-1,1);

    GMainLoop *loop = g_main_loop_new(NULL,false);

    //John/Olga channel
	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

    //message id
    std::string messageId = "1634591623619";

    //John id
	std::string senderUserId = "fdb3a4e9-675d-497e-acfe-4fd208f8ad89";
	//Olga id
	std::string receiverUserId = "7b30ff05-51b2-490a-b28b-2d8ac36cad8e";

    SoupSession *session = soup_session_new();

    //fetchTeamsSync(session,chatSvcAggToken);
    std::string msgtext = "dm test";
    //sendMessageSync(session,msgtext,skypeToken,channelId);
    //sendChannelMessage(session,loop,msgtext,skypeToken,channelId);
    //sendReplyMessage(session,loop,msgtext,skypeToken,channelId,messageId);
    sendDirectMessage(session,loop,msgtext,skypeToken,senderUserId,receiverUserId);

    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    g_object_unref(session);

    return 0;
}

void readCredentials(std::string &skypeToken,std::string &chatSvcAggToken) {
	std::ifstream credFile("ms-teams-credentials.local.txt");
	if (credFile.is_open()) {
		getline(credFile, skypeToken);
		getline(credFile, chatSvcAggToken);

		credFile.close();
	}
}

void fetchTeamsSync(SoupSession *session, std::string &chatSvcAggToken){
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

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

void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop_param){ //gpointer
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    //soup_session_send_async(session,msg,NULL,sendMessageCallback,NULL);
}

void sendMessageSync(SoupSession *session, std::string &text, std::string &skypeToken, std::string &params){
    //initialize soup message with url and method
    std::string url = "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{'content':\""+text+"\",'messagetype':'Text','contenttype':'text','asmreferences':[],'properties':{'importance':'','subject':null}}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //set request auth header token
    std::string tokenstr = "skypetoken=" + skypeToken;
    soup_message_headers_append(msg->request_headers,"Authentication",tokenstr.c_str());

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

void sendMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &params){
    //initialize soup message with url and method
    std::string url = "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{'content':\""+text+"\",'messagetype':'Text','contenttype':'text','asmreferences':[],'properties':{'importance':'','subject':null}}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //set request auth header token
    std::string tokenstr = "skypetoken=" + skypeToken;
    soup_message_headers_append(msg->request_headers,"Authentication",tokenstr.c_str());

    //send async message request
    soup_session_queue_message(session,msg,queueMessageCallback,loop);
}

void queueMessageCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    GMainLoop *loop = (GMainLoop *)user_data;
    g_main_loop_quit(loop);
}

void sendChannelMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId){
    sendMessage(session,loop,text,skypeToken,channelId);
}

void sendReplyMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId, std::string &messageId){
    std::string params = channelId + ";" + "messageid=" + messageId;
    sendMessage(session,loop,text,skypeToken,params);
}

void sendDirectMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &senderUserId, std::string &receiverUserId){
    std::string params = "19:" + receiverUserId + "_" + senderUserId + "@unq.gbl.spaces";
    sendMessage(session,loop,text,skypeToken,params);
}
