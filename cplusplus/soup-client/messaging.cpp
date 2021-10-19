#include <string>

#include <libsoup/soup.h>

#include "messaging.h"

//send a message synchronously
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

//callback to execute after message sent
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

//send message in channel
void sendChannelMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId){
    sendMessage(session,loop,text,skypeToken,channelId);
}

//send reply to specified message
void sendReplyMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId, std::string &messageId){
    std::string params = channelId + ";" + "messageid=" + messageId;
    sendMessage(session,loop,text,skypeToken,params);
}

//send direct chat message
void sendDirectMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &senderUserId, std::string &receiverUserId){
    std::string params = "19:" + receiverUserId + "_" + senderUserId + "@unq.gbl.spaces";
    sendMessage(session,loop,text,skypeToken,params);
}