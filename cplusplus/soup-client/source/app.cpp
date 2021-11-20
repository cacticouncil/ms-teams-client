#include <string>
#include <iostream>
#include <ctime>
#include <map>
#include <vector>

#include <libsoup/soup.h>

#include <json-glib/json-glib.h>

#include "../include/polling.h"
#include "../include/messaging.h"
#include "../include/fetch.h"
#include "../include/admin.h"
#include "../include/app.h"
#include "../include/User.h"
#include "../include/Callbacks.h"

//develop standardized solution for tokens in core API; currently ones called only from callbacks use prepended token
//possibly switch all to just token; develop function which takes Authentication vs Authorization and prepended tokenstr, extracts just token to pass

Auth appAuth;

//add logout
//main console app run function
int runConsoleApp(){
    std::cout << "Welcome to MS-Teams!\n";
    std::cout << "[NOTE] If running this app via WSL on Windows, make sure that you are running an active XServer to login!\n";
    std::cout << "\nSign In: [ENTER]\n";
    std::cout << "Quit: [q]\n";

    /* std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId; */

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
        status = readCredentialsOnly(appAuth.skypeToken, appAuth.chatSvcAggToken, appAuth.skypeSpacesToken, appAuth.currUserId);
        if(!status || !checkCredentialsValid()){
            std::cout << "ERROR: Invalid credentials! Exiting...\n";
            return 1;
        };
    }

    SoupSession *session = soup_session_new();
    GMainLoop *loop = g_main_loop_new(NULL,false);

    User currUser;
    currUser.SetUserOid(appAuth.currUserId);

    std::map<std::string,User*> usersMap;
    usersMap.emplace(appAuth.currUserId,&currUser);
    
    std::vector<User*> userList;
    userList.push_back(&currUser);

    GPtrArray *user_callback_data = g_ptr_array_new();
    g_ptr_array_add(user_callback_data,&userList);
    g_ptr_array_add(user_callback_data,loop);
    g_ptr_array_add(user_callback_data,&appAuth.skypeToken);
    bool isLogin = true;
    g_ptr_array_add(user_callback_data,&isLogin);

    fetchUsersInfo(session,appAuth.chatSvcAggToken,loop,&userList,populateUserData,user_callback_data);

    g_main_loop_run(loop);

    g_ptr_array_unref(user_callback_data);
    g_main_loop_unref(loop);
    g_object_unref(session);

    //a little map experimentation
    std::cout << "Goodbye, " << usersMap[appAuth.currUserId]->GetUserDisplayName() << "!\n";

    return 0;
}

//main display function - call from callbacks currrently
//switch to call from getMessages callback
//add back function
void displayMain(SoupSession *session, GMainLoop *loop){//, std::string &skypeToken){
    std::cout << "\nSend Message: [1]\n";
    std::cout << "Create New Team: [2]\n";
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

        /* std::string tokenPrefix = "skypetoken=";
        std::string unprefixedToken = skypeToken.substr(tokenPrefix.size(),std::string::npos); */
        sendChannelMessage(session,loop,msgtext,appAuth.skypeToken/* unprefixedToken */,channelId,sendMessageCallback);
        
        msgCt++;
    }
    else if(input == "2"){
        std::cout << "Enter Team name: ";
        std::string teamname;
        std::getline(std::cin,teamname);

        std::cout << "Verifying name...\n";

        //populate skypeSpacesToken, pass all tokens to displayMain maybe
        createTeamName(session,loop,appAuth.skypeSpacesToken,teamname,teamNameValidatedCallback);
    }
    else{
        return; //on "refresh", call getMessages/cached messages with custom event
    }
}

//check if current credentials in file are valid
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

//read in credentials from file, return if successful
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

//callback after message sent, triggers main display
void sendMessageCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Message sent! Time: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    //std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");

    displayMain(session,(GMainLoop*)user_data);//,skypeToken);
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

        //can replace with appAuth.skypeToken
        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");

        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl,newEventCallback);
        //trigger main display function
        displayMain(session,(GMainLoop*)user_data);//,skypeToken);
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
        g_print("New notification received!\n");
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

        //can replace with appAuth.skypeToken
        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");

        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl,newEventCallback);
    }
    else{
        g_printerr("ERROR: Unable to parse polling response: %s! Exiting...\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        GMainLoop *loop = (GMainLoop *)user_data;
        g_main_loop_quit(loop);
    }
}

//callback after getUserInfo returns, trigger json parser to populate User list, prints welcome message if login flag
void populateUserData(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("User Info Retrieved\n");
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    GPtrArray *data_arr = (GPtrArray*)user_data;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonNode* root = json_parser_get_root(parser);
        JsonObject* rootObj = json_node_get_object(root); 
        JsonArray* arr = json_object_get_array_member(rootObj, "value");

        json_array_foreach_element(arr, jsonArrayGetUsers, user_data);

        g_object_unref (parser);

        bool isLogin = *((bool*)g_ptr_array_index(data_arr,3));
        if(isLogin){
            std::vector<User*> userList = *((std::vector<User*>*)g_ptr_array_index(data_arr,0));
            std::cout << "Hello, " << userList[0]->GetUserDisplayName() << "!\n";

            GMainLoop* loop = (GMainLoop*)g_ptr_array_index(data_arr,1);
            //can replace with appAuth.skypeToken
            std::string skypeToken = *((std::string*)g_ptr_array_index(data_arr,2));

            initPolling(session,loop,skypeToken,initCallback);
        }
    }
    else{
        g_printerr("ERROR: Unable to parse user info response: %s! Exiting...\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr,1);
        g_main_loop_quit(loop);
    }
}

//parse name validation response (if 2xx code) and call createTeam with response alias name (if parse successful)
void teamNameValidatedCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Validation Response: %s\n",msg->response_body->data);

        JsonParser *parser = json_parser_new();
        GError *err;

        if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
            JsonReader *reader = json_reader_new(json_parser_get_root(parser));

            json_reader_read_member(reader,"value");
            json_reader_read_member(reader,"mailNickname");
            std::string name = json_reader_get_string_value(reader);

            g_object_unref(reader);
            g_object_unref(parser);

            //can replace with appAuth.skypeSpacesToken
            std::string skypeSpacesToken = soup_message_headers_get_one(msg->request_headers,"Authorization");
            createTeam(session,(GMainLoop *)user_data,skypeSpacesToken,name,teamCreatedCallback);
        }
        else{
            g_printerr("VALIDATION ERROR: Unable to parse response: %s\n", err->message);
            g_error_free (err);
            g_object_unref (parser);

            GMainLoop *loop = (GMainLoop *)user_data;
            g_main_loop_quit(loop);
        }
    }
    else{
        g_printerr("VALIDATION ERROR: Code: %d\nResponse: %s\n",msg->status_code,msg->response_body->data);

        GMainLoop *loop = (GMainLoop *)user_data;
        g_main_loop_quit(loop);
    }
}

//callback to execute after team created (well response returned at least), then trigger displayMain
void teamCreatedCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\nResponse: %s\n",msg->status_code,msg->response_body->data);
    }

    displayMain(session,(GMainLoop *)user_data);
}