#include <string>
#include <regex>

#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

#include "../include/polling.h"

//fetches the initial endpoint for polling
void fetchPollingEndpoint(SoupSession *session, GMainLoop *loop, std::string &skypeToken){
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
    soup_session_queue_message(session,msg,pollEndpointCallback,loop);
}

//callback after polling endpoint is fetched, intiates polling chain
void pollEndpointCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    //extracts endpoint url from response
    std::cmatch match;
    std::regex expr("\"longPollUrl\":\"([^\"]*)\"");

    if(std::regex_search(msg->response_body->data,match,expr)){
        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");
        std::string endpointUrl = match[1];
        //poll endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl);
    }
}

//poll endpoint for changes
void poll(SoupSession *session, GMainLoop *loop, std::string &skypeToken, std::string &endpointUrl){
    //init msg with endpoint url
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,endpointUrl.c_str());

    //set request auth header token
    soup_message_headers_append(msg->request_headers,"Authentication",skypeToken.c_str());

    //send async message request
    soup_session_queue_message(session,msg,pollCallback,loop);
}

//callback after poll returns with change
void pollCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("\nNew! Response: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    //extracts next endpoint url from response
    std::cmatch match;
    std::regex expr("\"next\":\"([^\"]*)\"");

    if(std::regex_search(msg->response_body->data,match,expr)){
        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");
        std::string endpointUrl = match[1];
        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl);
    }
}