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

    /* //extracts endpoint url from response
    std::cmatch match;
    std::regex expr("\"longPollUrl\":\"([^\"]*)\"");

    if(std::regex_search(msg->response_body->data,match,expr)){
        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");
        std::string endpointUrl = match[1];
        //poll endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl);
    } */

    JsonParser *parser = json_parser_new();
    GError *err;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonNode *root = json_parser_get_root(parser);
        JsonObject *rootObj = json_node_get_object(root);
        JsonArray *subsArr = json_object_get_array_member(rootObj,"subscriptions");
        
        std::string endpointUrl;
        json_array_foreach_element(subsArr,ArrayCallback,&endpointUrl);

        g_object_unref(parser);

        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");
        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl);
    }
    else{
        g_printerr("ERROR: Unable to parse response: %s\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        GMainLoop *loop = (GMainLoop *)user_data;
        g_main_loop_quit(loop);
    }
}

void ArrayCallback(JsonArray* arr,guint index,JsonNode *elem,gpointer user_data){
    JsonObject* obj = json_node_get_object(elem);
    std::string channelType = json_object_get_string_member(obj,"channelType");
    if(channelType == "HttpLongPoll"){
        g_print("Channel type: %s",channelType.c_str());
        std::string endpointUrl = json_object_get_string_member(obj,"longPollUrl");
        g_print("URL: %s",endpointUrl.c_str());
        *(std::string*)user_data = endpointUrl.c_str();
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

    /* GError *err;
    JsonReader *reader = json_reader_new(json_from_string(msg->response_body->data,&err));

    if(err){
        g_printerr("ERROR: Unable to parse response: %s\n", err->message);
        g_error_free (err);
        g_object_unref (reader);

        GMainLoop *loop = (GMainLoop *)user_data;
        g_main_loop_quit(loop);

        return;
    }
    else{
        json_reader_read_member(reader,"next");
        std::string endpointUrl = json_reader_get_string_value(reader);

        g_object_unref(reader);

        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");
        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl);
    } */

    JsonParser *parser = json_parser_new();
    GError *err;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonReader *reader = json_reader_new(json_parser_get_root(parser));

        json_reader_read_member(reader,"next");
        std::string endpointUrl = json_reader_get_string_value(reader);

        g_object_unref(reader);
        g_object_unref(parser);

        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");
        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl);
    }
    else{
        g_printerr("ERROR: Unable to parse response: %s\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        GMainLoop *loop = (GMainLoop *)user_data;
        g_main_loop_quit(loop);
    }
}