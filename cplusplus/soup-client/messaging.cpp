#include <string>
#include <fstream>
#include <iostream>

#include <libsoup/soup.h>

#include "messaging.h"

int main(){
    std::string skypeToken;
	std::string chatSvcAggToken;

    readCredentials(skypeToken, chatSvcAggToken);
    skypeToken.erase(skypeToken.size()-1,1);

    //John/Olga channel
	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

    SoupSession *session = soup_session_new();

    //fetchTeamsSync(session,chatSvcAggToken);
    std::string msgtext = "Souptest";
    sendMessageSync(session,msgtext,skypeToken,channelId);
    //sendChannelMessage(session,msgtext,skypeToken,channelId);

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

void fetchTeams(SoupSession *session, std::string &chatSvcAggToken){
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    soup_session_send_async(session,msg,NULL,sendMessageCallback,NULL);
}

void sendMessageSync(SoupSession *session, std::string &text, std::string &skypeToken, std::string &params){
    //initialize soup message with url and method
    std::string url = "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    std::string payload = "{'content':'"+text+"','messagetype':'Text','contenttype':'text','asmreferences':[],'properties':{'importance':'','subject':null}}";
    //char *payload = soup_form_encode("content",text,"messagetype","Text","contenttype","text","asmreferences","[]","properties","{importance:'',subject:null}",NULL);
    //soup_message_set_request(msg,"application/x-www-form-urlencoded",SOUP_MEMORY_COPY, payload, strlen(payload));
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

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

void sendMessage(SoupSession *session, std::string &text, std::string &skypeToken, std::string &params){
    //initialize soup message with url and method
    std::string url = "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    //std::string payload = "{content:" + text + ",messagetype:Text,cotenttype:text,asmreferences:[],properties:{importance:'',subject:null}}";
    std::string payload = soup_form_encode("content",text,"messagetype","Text","contenttype","text","asmreferences","[]","properties","{importance:'',subject:null}",NULL);
    soup_message_set_request(msg,"application/x-www-form-urlencoded",SOUP_MEMORY_COPY, payload.c_str(), strlen(payload.c_str()));

    //set request auth header token
    std::string tokenstr = "skypetoken=" + skypeToken;
    soup_message_headers_append(msg->request_headers,"Authentication",tokenstr.c_str());

    //send async message request
    soup_session_send_async(session,msg,NULL,sendMessageCallback,NULL);
}

void sendMessageCallback(GObject *obj, GAsyncResult *res, gpointer user_data){
	std::cout << "Callback" << std::endl;
    
    GError *error = NULL;
    GInputStream *stream = soup_session_send_finish (SOUP_SESSION (obj), res, &error);

    if (error) {
        g_printerr ("ERROR: %s\n", error->message);
        g_error_free (error);
    }
    else{
        g_print("Success!");
        g_input_stream_close(stream,NULL,&error);
    }
}

void sendChannelMessage(SoupSession *session, std::string &text, std::string &skypeToken, std::string &channelId){
    sendMessage(session,text,skypeToken,channelId);
}

//send message (destination determined by params string) and return async response object
/* cpr::AsyncResponse sendMessage(std::string text, std::string& skypeToken, std::string& params){
	cpr::AsyncResponse fr = cpr::PostAsync(
		cpr::Url{ "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages" },
		cpr::Payload{ {"content",text},{"messagetype","Text"},{"contenttype","text"},{"asmreferences","[]"},{"properties","{importance:'',subject:null}"} },
		cpr::Header{ {"Authentication","skypetoken=" + skypeToken} }
	);

	return fr;
} */