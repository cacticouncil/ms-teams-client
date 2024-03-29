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

//develop standardized solution for tokens in core API; currently ones called only from callbacks use prepended token
//possibly switch all to just token; develop function which takes Authentication vs Authorization and prepended tokenstr, extracts just token to pass

//update object stores from polling notification

//globals
//could maybe replace struct with singleton
Auth appAuth;
std::map<std::string,User*> usersMap;
std::map<std::string,Team*> teamMap;
std::map<std::string,Channel*> channelMap;
std::string currTeamId;
std::string currChannelId;

int main(){
    return runConsoleApp();
}

//add logout
//main console app run function
int runConsoleApp(){
    //print login menu
    std::cout << "Welcome to MS-Teams!\n";
    std::cout << "\nSign In: [ENTER]\n";
    std::cout << "Quit: [q]\n";

    //handle user input
    std::string input;
    std::cout << "\nInput: ";
    std::getline(std::cin,input);
    if(input == "q"){
        std::cout << "Goodbye!\n";
        return 0;
    }
    else{
        //load/check credentials
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

    //initialize userid list with currUserId to create curr User
    std::vector<std::string> userIdList;
    userIdList.push_back(appAuth.currUserId);

    //callback data to pass fetchUsersInfo->populateUserData
    GPtrArray *user_callback_data = g_ptr_array_new();
    g_ptr_array_add(user_callback_data,loop);
    bool isLogin = true;
    g_ptr_array_add(user_callback_data,&isLogin);

    //fetch display name for curr user and trigger initial event chain
    fetchUsersInfo(session,appAuth.chatSvcAggToken,loop,userIdList,populateUserData,user_callback_data);

    //run event loop
    g_main_loop_run(loop);

    //free glib memory
    g_ptr_array_unref(user_callback_data);
    g_main_loop_unref(loop);
    g_object_unref(session);

    //a little map experimentation
    std::cout << "Goodbye, " << usersMap[appAuth.currUserId]->GetUserDisplayName() << "!\n";

    //memory cleanup
    for(auto iter : teamMap){
        delete iter.second;
    }
    for(auto iter : usersMap){
        delete iter.second;
    }

    return 0;
}

//main display function - triggered from callbacks currrently
void displayMain(SoupSession *session, GMainLoop *loop){
    //handle Team/Channel selection
    if(currTeamId.empty() || currChannelId.empty()){
        std::string input;
        do{
            bool isTeams = currTeamId.empty();

            //handle exit
            if(input == "q"){
                g_main_loop_quit(loop);
                return;
            }
            //handle Team/Channel creation
            else if(input == "c"){
                std::cout << "Enter new " << (isTeams ? "Team" : "Channel") << " name: ";
                std::string name;
                std::getline(std::cin,name);

                std::cout << "Creating new " << (isTeams ? "Team" : "Channel") << "...\n";

                if(isTeams){
                    createTeamName(session,loop,appAuth.skypeSpacesToken,name,teamNameValidatedCallback);
                }
                else{
                    std::string groupId = teamMap[currTeamId]->GetTeamGroupId();
                    createChannel(session,loop,appAuth.skypeSpacesToken,currTeamId,name,appAuth.skypeToken,groupId,/* teamCreatedCallback */channelCreatedCallback);
                }
                return;
            }
            else if(!isTeams && input == "d"){
                deleteTeam(session,loop,appAuth.skypeSpacesToken,appAuth.skypeToken,currTeamId,deletionResponse);
                return;
            }
            else if(!isTeams && input == "b"){
                currTeamId = "";
                displayMain(session,loop);
                return;
            }

            //convert input to int for processing
            int inVal;
            try{
                inVal = std::stoi(input);
            } catch(const std::invalid_argument& ){
                inVal = -1;
            }

            //if input valid, select and break loop
            if(inVal >= 0 && inVal < (isTeams ? teamMap.size() : teamMap[currTeamId]->GetChannelList().size())){
                if(isTeams){
                    auto iter = teamMap.begin();
                    for(int i = 0; i < inVal; i++){
                        iter++;
                    }
                    currTeamId = iter->second->GetTeamId();
                }
                else{
                    auto channelList = teamMap[currTeamId]->GetChannelList();
                    currChannelId = channelList[inVal]->GetChannelId();
                }
                break;
            }
            /* else{
                std::cout << "Invalid response! Please select from the list.\n";
            } */

            //print menu display
            std::cout << "\nAvailable " << (isTeams ? "Teams" : "Channels") << ":\n";
            int index = 0;
            if(isTeams){
                for(auto iter : teamMap){
                    std::cout << iter.second->GetTeamDisplayName() << " [" << index++ << "]\n";
                }
            }
            else{
                for(Channel *c : teamMap[currTeamId]->GetChannelList()){
                    std::cout << c->GetChannelDisplayName() << " [" << index++ << "]\n";
                }
            }
            std::cout << "Create new " << (isTeams ? "Team" : "Channel") << ": [c]\n";
            if(!isTeams){
                std::cout << "Delete Team: [d]\n";
                std::cout << "Back to Teams: [b]\n";
            }
            std::cout << "Quit: [q]\n";

            //display prompt
            std::cout << "\nInput: ";
        }
        while(std::getline(std::cin,input));

        displayMain(session,loop);

        return;
    }

    //print messages 
    if(channelMap[currChannelId]->GetMessagesRetrievedStatus()){//!channelMap[currChannelId]->GetChannelMgs().empty()){
        for(Message *m : channelMap[currChannelId]->GetChannelMgs()){
            std::cout << "\nFrom: " << m->GetSenderOid() << "\n";
            std::cout << "Content: " << m->GetMsgContent() << "\n";
        }
    }
    else{
        fetchChannelMessages(session,appAuth.chatSvcAggToken,loop,currTeamId,currChannelId,5,fetchMessagesCallback,nullptr);
        return;
    }
    
    //display menu
    std::cout << "\nSend Message: [m]\n";
    std::cout << "Delete Channel: [d]\n";
    std::cout << "Back to Channels: [b]\n";
    //std::cout << "Refresh: [ENTER]\n";
    std::cout << "Quit: [q]\n";

    //handle user input
    std::string input;
    std::cout << "\nInput: ";
    std::getline(std::cin,input);
    if(input == "q"){
        g_main_loop_quit(loop);
        return;
    }
    else if(input == "m"){
        std::cout << "Enter message text: ";
        std::string msgtext;
        std::getline(std::cin,msgtext);

        std::cout << "Sending message...\n";

        sendChannelMessage(session,loop,msgtext,appAuth.skypeToken,currChannelId,sendMessageCallback);
    }
    else if(input == "d"){
        deleteChannel(session,loop,appAuth.skypeSpacesToken,appAuth.skypeToken,currTeamId,currChannelId,deletionResponse);
    }
    else if(input == "b"){
        currChannelId = "";

        displayMain(session,loop);
    }
    else{
        std::cout << "Invalid input! Please select from the menu.\n";
        displayMain(session,loop);
        return;
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
        g_print("Message sent! %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    displayMain(session,(GMainLoop*)user_data);
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
        json_array_foreach_element(subsArr,arrayCallback,&endpointUrl);

        g_object_unref(parser);

        //can replace with appAuth.skypeToken
        std::string skypeToken = soup_message_headers_get_one(msg->request_headers,"Authentication");

        //poll next endpoint for changes
        poll(session,(GMainLoop *)user_data,skypeToken,endpointUrl,newEventCallback);
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
    GMainLoop* loop = (GMainLoop*)g_ptr_array_index(data_arr,0);

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonNode* root = json_parser_get_root(parser);
        JsonObject* rootObj = json_node_get_object(root); 
        JsonArray* arr = json_object_get_array_member(rootObj, "value");

        json_array_foreach_element(arr, parseUsersResponse, nullptr);

        g_object_unref (parser);

        bool isLogin = *((bool*)g_ptr_array_index(data_arr,1));//3));
        if(isLogin){
            //print greeting
            std::cout << "Hello, " << usersMap[appAuth.currUserId]->GetUserDisplayName() << "!\n";

            //trigger polling
            initPolling(session,loop,appAuth.skypeToken,initCallback);

            //fetchTeams info
            fetchTeams(session,appAuth.chatSvcAggToken,loop,populateTeamsCallback,nullptr);
        }
    }
    else{
        g_printerr("ERROR: Unable to parse user info response: %s! Exiting...\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        g_main_loop_quit(loop);
    }
}

//Function that gets executed for each element of the Json Array
void parseUsersResponse(JsonArray* array, guint index_, JsonNode* element_node, gpointer user_data){
    JsonObject* currObj = json_array_get_object_element(array, index_);  //current array object being disected

    JsonNode* userInfo = json_object_get_member(currObj, "objectId");
    std::string objectId = json_node_get_string(userInfo);

    userInfo = json_object_get_member(currObj, "displayName");
    std::string displayName = json_node_get_string(userInfo);

    userInfo = json_object_get_member(currObj, "email");
    std::string email = json_node_get_string(userInfo);

    userInfo = json_object_get_member(currObj, "mri");
    std::string mri = json_node_get_string(userInfo);

    //create new user
    User *newUser = new User();

    //populate new user fields
    newUser->SetUserOid(objectId);
    newUser->SetUserDisplayName(displayName);
    newUser->SetUserEmail(email);
    newUser->SetUserMri(mri);

    //insert new user into user map
    usersMap.emplace(objectId,newUser);
}

//parse name validation response (if 2xx code) and call createTeam with response alias name (if parse successful)
void teamNameValidatedCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Validation Response: %s\n",msg->response_body->data);

        JsonParser *parser = json_parser_new();
        GError *err = NULL;

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
        //g_print("Response: %s\n",msg->response_body->data);
        g_print("New Team Created!\n");
    }
    else{
        g_printerr("ERROR: Code: %d\nResponse: %s\n",msg->status_code,msg->response_body->data);
    }

    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonNode* root = json_parser_get_root(parser);
        JsonObject* rootObj = json_node_get_object(root); 
        JsonObject* valueObj = json_object_get_object_member(rootObj, "value");

        Team *newTeam = new Team();

        //I think this is all that is needed at present
        std::string teamId = json_object_get_string_member(valueObj,"objectId");
        newTeam->SetTeamId(teamId);

        std::string dispName = json_object_get_string_member(valueObj,"displayName");
        newTeam->SetTeamDisplayName(dispName);

        JsonObject* siteInfo = json_object_get_object_member(valueObj,"siteInfo");
        std::string groupId = json_object_get_string_member(siteInfo,"groupId");
        newTeam->SetTeamGroupId(groupId);

        teamMap.emplace(teamId,newTeam);

        //create General channel
        Channel *newChannel = new Channel();
        newChannel->SetChannelId(teamId);
        newChannel->SetChannelTeamId(teamId);
        newChannel->SetChannelDisplayName("General");

        newTeam->GetChannelList().push_back(newChannel);
        channelMap.emplace(teamId,newChannel);

        displayMain(session,(GMainLoop*)user_data);
    }
     else{
        g_printerr("ERROR: Unable to parse response: %s\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        g_main_loop_quit((GMainLoop*)user_data);
    }
}

//called after channel is created
void channelCreatedCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        //g_print("Response: %s\n",msg->response_body->data);
        g_print("New Channel Created!\n");
    }
    else{
        g_printerr("ERROR: Code: %d\nResponse: %s\n",msg->status_code,msg->response_body->data);
    }

    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonNode* root = json_parser_get_root(parser);
        JsonObject* rootObj = json_node_get_object(root); 
        JsonObject* valueObj = json_object_get_object_member(rootObj, "value");

        Channel *newChannel = new Channel();

        //I think this is all that is needed at present
        std::string channelId = json_object_get_string_member(valueObj,"objectId");
        newChannel->SetChannelId(channelId);

        std::string dispName = json_object_get_string_member(valueObj,"displayName");
        newChannel->SetChannelDisplayName(dispName);

        teamMap[currTeamId]->GetChannelList().push_back(newChannel);
        channelMap.emplace(channelId,newChannel);

        displayMain(session,(GMainLoop*)user_data);
    }
     else{
        g_printerr("ERROR: Unable to parse response: %s\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        g_main_loop_quit((GMainLoop*)user_data);
    }
}

void fetchMessagesCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Messages Retrieved\n");
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }
    
    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    GMainLoop *loop = (GMainLoop*)user_data;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonNode* root = json_parser_get_root(parser);
        JsonObject* rootObj = json_node_get_object(root); 
        JsonArray* arr = json_object_get_array_member(rootObj, "replyChains");

        json_array_foreach_element(arr, parseReplyChains, user_data); //middle level callback

        channelMap[currChannelId]->SetMessagesRetrievedStatus(true);

        displayMain(session,loop);
    }
     else{
        g_printerr("ERROR: Unable to parse response: %s\n", err->message);
        g_error_free (err);
        g_object_unref (parser);

        g_main_loop_quit(loop);
    }
}

void parseReplyChains(JsonArray* array, guint index_, JsonNode* element_node, gpointer user_data){
    JsonObject* currObj = json_array_get_object_element(array, index_);
    JsonArray* messageArr = json_object_get_array_member(currObj, "messages");

    json_array_foreach_element(messageArr, parseMessages, user_data);//(gpointer) data_arr);
}

void parseMessages(JsonArray* array, guint index_, JsonNode* element_node, gpointer user_data){
    //create and populate new message
    Message *t = new Message();
    t->SetMsgContainerChannelId(currChannelId);

    JsonObject* currObj = json_array_get_object_element(array, index_);  //current array object being disected

    JsonNode* value = json_object_get_member(currObj, "content");
    t->SetMsgContent(json_node_get_string(value));
    
    value = json_object_get_member(currObj, "id");
    t->SetMsgId(json_node_get_string(value));

    value = json_object_get_member(currObj, "parentMessageId");
    t->SetMsgParentId(json_node_get_string(value));

    int seqId = json_object_get_int_member (currObj, "sequenceId");
    t->SetMsgSequenceId(seqId);

    value = json_object_get_member(currObj, "from");
    t->SetSenderMri(json_node_get_string(value));

    value = json_object_get_member(currObj, "originalArrivalTime");
    t->SetArrivalTime(json_node_get_string(value));

    //append new message to channel message list
    channelMap[currChannelId]->GetChannelMgs().push_back(t);
}

void populateTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Teams Retrieved\n");
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    JsonParser *parser = json_parser_new();
    GError *err = nullptr;

    GMainLoop *loop = (GMainLoop*)user_data;

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        //using JsonObject* to read the array from here rather than from a JsonNode* since array is a complex type
        JsonNode* root= json_parser_get_root(parser);
        JsonObject* rootObj= json_node_get_object(root); 
        JsonArray* arr= json_object_get_array_member(rootObj, "teams");  //just like this, there is another whole "chats" array after the teams with information about group DMs

        json_array_foreach_element(arr, parseTeamsResponse, user_data);

        //trigger main display function
        displayMain(session,loop);
    }
     else{
        g_printerr("ERROR: Unable to parse response: %s\n", err->message);
        g_error_free (err);
        g_object_unref (parser);
        
        g_main_loop_quit(loop);
    }
}

void parseTeamsResponse(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){
    //Constructing each individual team
    Team *t = new Team();

    JsonObject* currObj = json_array_get_object_element(array, index_);  //current array object being disected
    JsonNode* value = json_object_get_member(currObj, "displayName"); 
    t->SetTeamDisplayName(json_node_get_string(value));

    value = json_object_get_member(currObj, "id"); 
    t->SetTeamId(json_node_get_string(value));

    value = json_object_get_member(currObj, "creator");
    t->SetCreatorMri(json_node_get_string(value));


    JsonNode* teamValue = json_object_get_member(currObj, "teamSiteInformation");
    JsonObject* teamSiteInfo = json_node_get_object(teamValue); //turning it into an object to get info from its members
    JsonNode* member = json_object_get_member(teamSiteInfo, "groupId");
    std::string groupId = json_node_get_string(member);
    t->SetTeamGroupId(groupId);

  
    JsonNode* membershipValue = json_object_get_member(currObj, "membershipSummary");
    JsonObject* memberObject = json_node_get_object(membershipValue); //turning it into an object to get info from its members
    int count = json_object_get_int_member(memberObject, "totalMemberCount");
    t->SetTotalMemberCount(count);


    JsonArray* channelArr = json_object_get_array_member(currObj, "channels");

    json_array_foreach_element(channelArr, parseChannelList, (gpointer) t); //pointer to the team

    //insert new team into map
    teamMap.emplace(t->GetTeamId(),t);
}

void parseChannelList(JsonArray* array, guint index_, JsonNode* element_node, gpointer user_data){
    //create new channel
    Channel *channel = new Channel();

    JsonObject* currObj = json_array_get_object_element(array, index_);  //current array object being disected

    JsonNode* value = json_object_get_member(currObj, "displayName"); //member name here

    channel->SetChannelDisplayName(json_node_get_string(value));

    value = json_object_get_member(currObj, "id"); 
    channel->SetChannelId(json_node_get_string(value));

    value = json_object_get_member(currObj, "parentTeamId"); 
    channel->SetChannelTeamId(json_node_get_string(value));

    //"isMember": false,
    bool isMember = json_object_get_boolean_member(currObj, "isMember");
    channel->SetIsChannelMember(isMember);

    if (isMember){
        value = json_object_get_member(currObj, "creator"); 
        channel->SetChannelCreatorMri(json_node_get_string(value));

        value = json_object_get_member(currObj, "groupId"); 
        channel->SetChannelGroupId(json_node_get_string(value));
    }

    //append new channel to channel list of created Team
    Team* createdTeam = (Team*)user_data;
    createdTeam->GetChannelList().push_back(channel);
    
    //insert new chhannel into map
    channelMap.emplace(channel->GetChannelId(),channel);
}

void deletionResponse(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        std::string didDelete = msg->response_body->data;
        if(didDelete == "true"){
            g_print("Deletetion Successful!\n");
            if(currChannelId.empty()){
                //remove from Team map and delete
                Team *t = teamMap[currTeamId];
                teamMap.erase(currTeamId);
                delete t;
                currTeamId = "";
            }
            else{
                //remove from Team channelList
                std::vector<Channel*> &channelList = teamMap[currTeamId]->GetChannelList();
                auto iter = channelList.begin();
                while(iter != channelList.end()){
                    if((*iter)->GetChannelId() == currChannelId){
                        channelList.erase(iter);
                        break;
                    }
                    iter++;
                }

                delete channelMap[currChannelId];
                currChannelId = "";
            }
        }
        else{
            std::cout << "Unable to delete: " << (currChannelId.empty() ? teamMap[currTeamId]->GetTeamDisplayName() : channelMap[currChannelId]->GetChannelDisplayName()) << "\n";
        }
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    displayMain(session,(GMainLoop*)user_data);
}

void arrayCallback(JsonArray* arr,guint index,JsonNode *elem,gpointer user_data){
    JsonObject* obj = json_node_get_object(elem);
    std::string channelType = json_object_get_string_member(obj,"channelType");
    if(channelType == "HttpLongPoll"){
        std::string endpointUrl = json_object_get_string_member(obj,"longPollUrl");
        //g_print("URL: %s\n",endpointUrl.c_str());
        *(std::string*)user_data = endpointUrl.c_str();
    }
}