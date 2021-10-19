#include <string>
#include <iostream>

#include <libsoup/soup.h>

#include "polling.h"
#include "messaging.h"

int main(){
    std::string skypeToken;
	std::string chatSvcAggToken;

    readCredentials(skypeToken, chatSvcAggToken);
    //remove new line from skypeToken
    skypeToken.erase(skypeToken.size()-1,1);
    
    GMainLoop *loop = g_main_loop_new(NULL,false);

    SoupSession *session = soup_session_new();

    fetchPollingEndpoint(session,loop,skypeToken);

    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    g_object_unref(session);

    return 0;
}