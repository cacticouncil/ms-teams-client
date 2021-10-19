#include <string>
#include <iostream>

#include <libsoup/soup.h>

#include "polling.h"
#include "messaging.h"

/* int main(){
    std::string skypeToken;
	std::string chatSvcAggToken;

    readCredentials(skypeToken, chatSvcAggToken);
    //remove new line from skypeToken
    skypeToken.erase(skypeToken.size()-1,1);

    fetchPollingEndpoint(nullptr,nullptr,skypeToken);
    GMainLoop *loop = g_main_loop_new(NULL,false);

    SoupSession *session = soup_session_new();

    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    g_object_unref(session);

    return 0;
} */

void fetchPollingEndpoint(SoupSession *session, GMainLoop *loop, std::string &skypeToken){
    //initialize soup message with url and method
    std::string url = "https://amer.ng.msg.teams.microsoft.com/v2/users/ME/endpoints/";
    std::string id = g_uuid_string_random();
    url += id;
    std::cout << url << std::endl;
    /* SoupMessage *msg = soup_message_new(SOUP_METHOD_PUT,url.c_str());

    //set message request payload
    std::string payload = "{'content':\""+text+"\",'messagetype':'Text','contenttype':'text','asmreferences':[],'properties':{'importance':'','subject':null}}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //set request auth header token
    std::string tokenstr = "skypetoken=" + skypeToken;
    soup_message_headers_append(msg->request_headers,"Authentication",tokenstr.c_str()); */

    //send async message request
    //soup_session_queue_message(session,msg,queueMessageCallback,loop);
}