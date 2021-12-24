#include <string>
#include <regex>

#include <libsoup/soup.h>

#include <json-glib/json-glib.h>

#include "../include/polling.h"

//fetches the initial endpoint for polling
void initPolling(SoupSession *session, GMainLoop *loop, std::string &skypeToken, SoupSessionCallback initPollingCallback){
    //initialize soup message with url and method
    std::string url = "https://amer.ng.msg.teams.microsoft.com/v2/users/ME/endpoints/";
    std::string id = g_uuid_string_random();
    url += id;
    SoupMessage *msg = soup_message_new(SOUP_METHOD_PUT,url.c_str());

    //set message request payload
    std::string payload = "{'startingTimeSpan':0,'endpointFeatures':'Agent,Presence2015,MessageProperties,CustomUserProperties,NotificationStream,SupportsSkipRosterFromThreads','subscriptions':[{'channelType':'HttpLongPoll','interestedResources':['/v1/users/ME/conversations/ALL/properties', '/v1/users/ME/conversations/ALL/messages', '/v1/threads/ALL']}]}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //set request auth header token
    std::string tokenstr = "skypetoken=" + skypeToken;
    soup_message_headers_append(msg->request_headers,"Authentication",tokenstr.c_str());

    //send async message request
    soup_session_queue_message(session,msg,initPollingCallback,loop);
}

//poll endpoint for changes
//only called from initPollingCallback of initPolling, so "skypetoken=" already prepended to token
void poll(SoupSession *session, GMainLoop *loop, std::string &skypeToken, std::string &endpointUrl, SoupSessionCallback pollingCallback){
    //init msg with endpoint url
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,endpointUrl.c_str());

    //set request auth header token
    soup_message_headers_append(msg->request_headers,"Authentication",skypeToken.c_str());

    //send async message request
    soup_session_queue_message(session,msg,pollingCallback,loop);
}