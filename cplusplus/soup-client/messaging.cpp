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

    return 0;
}

void readCredentials(std::string& skypeToken,std::string& chatSvcAggToken) {
	std::ifstream credFile("ms-teams-credentials.local.txt");
	if (credFile.is_open()) {
		getline(credFile, skypeToken);
		getline(credFile, chatSvcAggToken);

		credFile.close();
	}
}

void sendMessage(SoupSession *session, std::string text, std::string& skypeToken, std::string& params){
    std::string url = "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    SoupMessageBody *body;
    msg->request_body = body;

    std::string tokenstr = "skypetoken=" + skypeToken;
    soup_message_headers_append(msg->request_headers,"Authentication",tokenstr.c_str());

    soup_session_send_async(session,msg,NULL,sendMessageCallback,NULL);
}

void sendMessageCallback(GObject *obj, GAsyncResult *res, gpointer user_data){
	GError *error = NULL;
    GInputStream *stream = stream = soup_session_send_finish (SOUP_SESSION (obj), res, &error);
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