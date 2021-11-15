#include <string>
#include <iostream>
#include <ctime>

#include <libsoup/soup.h>

#include <json-glib/json-glib.h>

#include "../include/polling.h"
#include "../include/messaging.h"
#include "../include/fetch.h"
#include "../include/admin.h"
#include "../include/app.h"

int runConsoleApp(){
    std::cout << "Welcome to MS-Teams!\n";
    std::cout << "[NOTE] If running this app via WSL on Windows, make sure that you are running an active XServer to login!\n";
    std::cout << "\nSign In: [ENTER]\n";
    std::cout << "Quit: [q]\n";

    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;

    std::string input;
    std::cout << "\nInput: ";
    std::getline(std::cin,input);
    if(input == "q"){
        std::cout << "Goodbye!\n";
        return 0;
    }
    else{
        bool status = checkCredentialsValid();
        if(!status) system("./trigger-login.sh");
        status = readCredentialsOnly(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);
        if(!status || !checkCredentialsValid()){
            std::cout << "ERROR: Invalid credentials! Exiting...\n";
            return 1;
        };
        std::cout << "Signed in as " << currUserId << "\n"; //convert to display name via getUsersInfo
    }

    SoupSession *session = soup_session_new();
    GMainLoop *loop = g_main_loop_new(NULL,false);

    initPolling(session,loop,skypeToken,initCallback);

    /* skypeToken = "skypetoken=" + skypeToken;
    displayMainUnsource(session,loop,skypeToken); */

    /* GPtrArray *user_data = g_ptr_array_new();
    g_ptr_array_add(user_data,session);
    g_ptr_array_add(user_data,loop);
    g_ptr_array_add(user_data,&skypeToken);
    g_idle_add(G_SOURCE_FUNC(displayMain),user_data); */

    g_main_loop_run(loop);

    //g_ptr_array_unref(user_data);
    g_main_loop_unref(loop);
    g_object_unref(session);

    return 0;
}

bool displayMain(gpointer user_data){
    std::cout << "\nSend Message: [1]\n";
    std::cout << "Continue: [ENTER]\n";
    std::cout << "Quit: [q]\n";

    static int msgCt = 1;
    
    std::string channelId = "19:5c7c73c0315144a4ab58108a897695a9@thread.tacv2";
    std::string msgtext = "Testing event loop (" + std::to_string(msgCt) + ")";

    GPtrArray *data_arr = (GPtrArray*)user_data;
    SoupSession *session = (SoupSession*)g_ptr_array_index(data_arr,0);
    GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr,1);
    std::string *skypeToken = (std::string*)g_ptr_array_index(data_arr,2);

    std::string input;
    std::cout << "\nInput: ";
    std::getline(std::cin,input);
    if(input == "1"){
        std::cout << "Sending message...\n";
        std::string tokenPrefix = "skypetoken=";
        std::string unprefixedToken = (*skypeToken).substr(tokenPrefix.size(),std::string::npos);
        std::cout << "token: " << unprefixedToken << "\n";
        sendChannelMessage(session,loop,msgtext,unprefixedToken,channelId,sendMessageCallback);
        msgCt++;
    }
    else if(input == "q"){
        g_main_loop_quit(loop);
        return false;
    }
    else{
        return true;
    }

    return true;
}

void displayMainUnsource(SoupSession *session, GMainLoop *loop, std::string &skypeToken){
    std::cout << "\nSend Message: [1]\n";
    std::cout << "Refresh: [ENTER]\n";
    std::cout << "Quit: [q]\n";

    static int msgCt = 1;
    
    std::string channelId = "19:5c7c73c0315144a4ab58108a897695a9@thread.tacv2";

    std::string input;
    std::cout << "\nInput: ";
    std::getline(std::cin,input);
    if(input == "q"){
        g_main_loop_quit(loop);
        return;
    }
    else if(input == "1"){
        std::cout << "Enter message text: ";
        std::string msgtext;
        std::getline(std::cin,msgtext);
        msgtext += " (" + std::to_string(msgCt) + ")";

        std::cout << "Sending message...\n";

        std::string tokenPrefix = "skypetoken=";
        std::string unprefixedToken = skypeToken.substr(tokenPrefix.size(),std::string::npos);
        sendChannelMessage(session,loop,msgtext,unprefixedToken,channelId,sendMessageCallback);
        
        msgCt++;
    }
    else{
        return;
    }
}

bool checkCredentialsValid(){
    std::string credFilename = "../../ms-teams-credentials.local.json"; 

    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        JsonReader *reader = json_reader_new(json_parser_get_root(parser));

        json_reader_read_member(reader,"skypeSpacesToken");
        json_reader_read_member(reader,"expiration");
        int expirationTime = json_reader_get_int_value(reader);
        int currTime = std::time(nullptr);
        int timeRemaining = expirationTime - currTime;

        g_object_unref(reader);
        g_object_unref(parser);

        bool valid = timeRemaining > 0;
        //std::cout << "Valid: " << valid << " (" << expirationTime << "-" << currTime << ")" << "\n";
        return valid;
    }
    else{
        g_printerr("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
        return false;
    }
}

bool readCredentialsOnly(std::string &skypeToken,std::string &chatSvcAggToken,std::string &skypeSpacesToken, std::string &currUserId){
    std::string credFilename = "../../ms-teams-credentials.local.json"; 

    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        JsonReader *reader = json_reader_new(json_parser_get_root(parser));

        json_reader_read_member(reader,"authSkype");
        json_reader_read_member(reader,"skypeToken");
        skypeToken = json_reader_get_string_value(reader);

        json_reader_set_root(reader,json_parser_get_root(parser));
        json_reader_read_member(reader,"chatSvcAggToken");
        json_reader_read_member(reader,"token");
        chatSvcAggToken = json_reader_get_string_value(reader);

        json_reader_set_root(reader,json_parser_get_root(parser));
        json_reader_read_member(reader,"skypeSpacesToken");
        json_reader_read_member(reader,"token");
        skypeSpacesToken = json_reader_get_string_value(reader);

        json_reader_set_root(reader,json_parser_get_root(parser));
        json_reader_read_member(reader,"oid");
        currUserId = json_reader_get_string_value(reader);

        g_object_unref(reader);
        g_object_unref(parser);
        return true;
    }
    else{
        g_printerr("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
        return false;
    }
}

void sendMessageCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Message sent! Time: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");

    displayMainUnsource(session,(GMainLoop*)user_data,skypeToken);
}

//callback after polling endpoint is fetched, intiates polling chain
void initCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonNode *root = json_parser_get_root(parser);
        JsonObject *rootObj = json_node_get_object(root);
        JsonArray *subsArr = json_object_get_array_member(rootObj,"subscriptions");
        
        std::string endpointUrl;
        json_array_foreach_element(subsArr,ArrayCallback,&endpointUrl);

        g_object_unref(parser);

        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");

        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl,newEventCallback);
        //trigger main display function
        displayMainUnsource(session,(GMainLoop*)user_data,skypeToken);
    }
    else{
        g_printerr("ERROR: Unable to parse polling response: %s! Exiting ...\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        GMainLoop *loop = (GMainLoop *)user_data;
        g_main_loop_quit(loop);
    }
}

//callback after poll returns with change
void newEventCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        //g_print("\nNew! Response: %s\n",msg->response_body->data);
        g_print("New message received!\n");
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonReader *reader = json_reader_new(json_parser_get_root(parser));

        json_reader_read_member(reader,"next");
        std::string endpointUrl = json_reader_get_string_value(reader);

        g_object_unref(reader);
        g_object_unref(parser);

        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");

        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl,newEventCallback);
        //trigger main display function
        //displayMainUnsource(session,(GMainLoop *)user_data,skypeToken);
    }
    else{
        g_printerr("ERROR: Unable to parse polling response: %s! Exiting...\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        GMainLoop *loop = (GMainLoop *)user_data;
        g_main_loop_quit(loop);
    }
}