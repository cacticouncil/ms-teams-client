#include <string>

#include <libsoup/soup.h>

#include "../include/messaging.h"

void sendMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &params, SoupSessionCallback msgCallback){
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
    soup_session_queue_message(session,msg,msgCallback,loop);
}

//send message in channel
void sendChannelMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId, SoupSessionCallback msgCallback){
    sendMessage(session,loop,text,skypeToken,channelId,msgCallback);
}

//send reply to specified message
void sendReplyMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId, std::string &messageId, SoupSessionCallback msgCallback){
    std::string params = channelId + ";" + "messageid=" + messageId;
    sendMessage(session,loop,text,skypeToken,params,msgCallback);
}

//send direct chat message
void sendDirectMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &senderUserId, std::string &receiverUserId, SoupSessionCallback msgCallback){
    std::string params = "19:" + receiverUserId + "_" + senderUserId + "@unq.gbl.spaces";
    sendMessage(session,loop,text,skypeToken,params,msgCallback);
}

//example callback to execute after message sent
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