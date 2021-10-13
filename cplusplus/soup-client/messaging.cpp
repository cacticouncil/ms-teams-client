#include <string>
#include <fstream>

#include <libsoup/soup.h>

#include "messaging.h"

int main(){
    std::string skypeToken;
	std::string chatSvcAggToken;

    readCredentials(skypeToken, chatSvcAggToken);

    //John/Olga channel
	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

    SoupSession *session = soup_session_new();

    std::string msgtext = "Souptest";
    sendChannelMessage(session,msgtext,skypeToken,channelId);

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

void sendMessage(SoupSession *session, std::string &text, std::string &skypeToken, std::string &params){
    //initialize soup message with url and method
    std::string url = "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    //std::string payload = "{content:" + text + ",messagetype:Text,cotenttype:text,asmreferences:[],properties:{importance:'',subject:null}}";
    std::string payload = soup_form_encode("content",text,"messagetype","Text","contenttype","text","asmreferences","[]","properties","{importance:'',subject:null}");
    soup_message_set_request(msg,"application/x-www-form-urlencoded",SOUP_MEMORY_COPY, payload.c_str(), strlen(payload.c_str()));

    //set request auth header token
    std::string tokenstr = "skypetoken=" + skypeToken;
    soup_message_headers_append(msg->request_headers,"Authentication",tokenstr.c_str());

    //send async message request
    soup_session_send_async(session,msg,NULL,sendMessageCallback,NULL);
}

void sendMessageCallback(GObject *obj, GAsyncResult *res, gpointer user_data){
	GError *error = NULL;
    GInputStream *stream = soup_session_send_finish (SOUP_SESSION (obj), res, &error);

    if (error) {
        g_printerr ("ERROR: %s\n", error->message);
    }
    else{
        g_print("Success!");
    }

    g_error_free (error);
    g_input_stream_close(stream,NULL,&error);
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