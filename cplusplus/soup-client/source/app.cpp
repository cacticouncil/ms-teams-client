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

//globals
//could maybe replace struct with singleton
Auth appAuth;
std::map<std::string,User*> usersMap;
std::map<std::string,Team*> teamMap;
std::map<std::string,Channel*> channelMap;
std::string currTeamId;
std::string currChannelId;

//add logout
//main console app run function
int runConsoleApp(){
    std::cout << "Welcome to MS-Teams!\n";
    std::cout << "\nSign In: [ENTER]\n";
    std::cout << "Quit: [q]\n";

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
    usersMap.emplace(appAuth.currUserId,&currUser);
    
    std::vector<User*> userList;
    userList.push_back(&currUser);

    //callback data to pass fetchUsersInfo->populateUserData
    GPtrArray *user_callback_data = g_ptr_array_new();
    g_ptr_array_add(user_callback_data,&userList);
    g_ptr_array_add(user_callback_data,loop);
    //can replace with appAuth.skypeToken
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

    //cleanup
    for(auto iter : teamMap){
        delete iter.second;
    }

    for(auto iter : usersMap){
        delete iter.second;
    }

    return 0;
}

//main display function - call from callbacks currrently
//add back function
void displayMain(SoupSession *session, GMainLoop *loop){
    //handle Team/Channel selection
    if(currTeamId.empty() || currChannelId.empty()){
        bool isTeams = currTeamId.empty();
        std::string input;
        do{
            //print names
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
                    currTeamId = teamMap[iter->first]->GetTeamId();
                }
                else{
                    auto channelList = teamMap[currTeamId]->GetChannelList();
                    currChannelId = channelList[inVal]->GetChannelId();
                }
                break;
            }

            //display prompt
            std::cout << "\nSelect a " << (isTeams ? "Team" : "Channel") << " to view: ";
        }
        while(std::getline(std::cin,input));

        displayMain(session,loop);

        return;
    }

    //capture current Team/Channel objects
    Team currTeam = *teamMap[currTeamId];
    Channel currChannel = *channelMap[currChannelId];

    //print messages 
    if(!currChannel.GetChannelMgs().empty()){
        for(Message *m : currChannel.GetChannelMgs()){
            std::cout << "\nFrom: " << m->GetSenderOid() << "\n";
            std::cout << "Content: " << m->GetMsgContent() << "\n";
        }
    }
    
    //display menu
    std::cout << "\nSend Message: [1]\n";
    std::cout << "Create New Team: [2]\n";
    std::cout << "Fetch Messages: [3]\n";
    std::cout << "Create New Channel: [4]\n";
    std::cout << "Refresh: [ENTER]\n";
    std::cout << "Quit: [q]\n";

    //temp sent message counter
    static int msgCt = 1;

    //handle user input
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

        sendChannelMessage(session,loop,msgtext,appAuth.skypeToken,currChannelId,sendMessageCallback);
        
        msgCt++;
    }
    else if(input == "2"){
        std::cout << "Enter Team name: ";
        std::string teamname;
        std::getline(std::cin,teamname);

        std::cout << "Verifying name...\n";

        createTeamName(session,loop,appAuth.skypeSpacesToken,teamname,teamNameValidatedCallback);
    }
    else if(input == "3"){
        GPtrArray *msgs_callback_data = g_ptr_array_new();
        std::vector<Message*> msgVect = currChannel.GetChannelMgs();
        g_ptr_array_add(msgs_callback_data,&msgVect); //0
        g_ptr_array_add(msgs_callback_data,loop);//1
        std::cout << "Init vect: " << &msgVect << "\n";
        fetchChannelMessages(session,appAuth.chatSvcAggToken,loop,&currTeam,&currChannel,5,fetchMessagesCallback,msgs_callback_data);
    }
    else if(input == "4"){
        std::cout << "Enter Channel name: ";
        std::string channelname;
        std::getline(std::cin,channelname);

        std::cout << "Creating new channel...\n";

        createChannel(session,loop,appAuth.skypeSpacesToken,currTeamId,channelname,appAuth.skypeToken,teamCreatedCallback);
    }
    else{
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
        g_print("Message sent! Time: %s\n",msg->response_body->data);
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
        json_array_foreach_element(subsArr,ArrayCallback,&endpointUrl);

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

            //remove team vector
            std::vector<Team> teamList;
            GPtrArray *user_data = g_ptr_array_new();
            g_ptr_array_add(user_data,&teamList);  //0 team vector (each team has its own channel vector, which will aslo be filled out)
            g_ptr_array_add(user_data,loop);   //1 loop
            fetchTeams(session,appAuth.chatSvcAggToken,loop,populateTeamsCallback,user_data);
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
        g_print("Response: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\nResponse: %s\n",msg->status_code,msg->response_body->data);
    }

    displayMain(session,(GMainLoop *)user_data);
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

    GPtrArray *data_arr = (GPtrArray*)user_data;
    GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr, 1);

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        JsonNode* root = json_parser_get_root(parser);
        JsonObject* rootObj = json_node_get_object(root); 
        JsonArray* arr = json_object_get_array_member(rootObj, "replyChains");

        json_array_foreach_element(arr, parseReplyChains, user_data); //middle level callback

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

    JsonNode* value = json_object_get_member(currObj, "containerId");
    std::string tempStr = json_node_get_string(value);
    
    //appending the channel id to the callback data for the next callback to assign to the message channel container id 
    GPtrArray *data_arr = (GPtrArray*)user_data;
    g_ptr_array_add(data_arr, &tempStr);

    JsonArray* messageArr = json_object_get_array_member(currObj, "messages");

    json_array_foreach_element(messageArr, parseMessages, (gpointer) data_arr);
}

void parseMessages(JsonArray* array, guint index_, JsonNode* element_node, gpointer user_data){
    GPtrArray *data_arr = (GPtrArray*)user_data;
    int i = (int) index_;

    std::string* channId = (std::string*)g_ptr_array_index(data_arr, 2); //Values at index: 0-vect, 1-loop, 2- just added the channel id in the middleLayer callback
    Channel *currChannel = channelMap[*channId];

    Message *t = new Message();
    t->SetMsgContainerChannelId(*(channId));

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

    currChannel->GetChannelMgs().push_back(t);
}

void populateTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Messages Retrieved\n");
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }

    JsonParser *parser = json_parser_new();
    GError *err = nullptr;

    GPtrArray *data_arr = (GPtrArray*)user_data;
    GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr, 1); //1- loop

    if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
        //using JsonObject* to read the array from here rather than from a JsonNode* since array is a complex type
        JsonNode* root= json_parser_get_root(parser);
        JsonObject* rootObj= json_node_get_object(root); 
        JsonArray* arr= json_object_get_array_member(rootObj, "teams");  //just like this there s a whole other "chats" array afte the teams with information about group DMs

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
    GPtrArray *data_arr = (GPtrArray*)user_data;

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
    JsonObject* teamSiteInfo = json_node_get_object(teamValue); //turing it into an object to get info from its members
    JsonNode* member = json_object_get_member(teamSiteInfo, "groupId");
    std::string groupId = json_node_get_string(member);
    t->SetTeamGroupId(groupId);

  
    JsonNode* membershipValue = json_object_get_member(currObj, "membershipSummary");
    JsonObject* memberObject = json_node_get_object(membershipValue); //turing it into an object to get info from its members
    int count = json_object_get_int_member(memberObject, "totalMemberCount");
    t->SetTotalMemberCount(count);


    JsonArray* channelArr = json_object_get_array_member(currObj, "channels");

    json_array_foreach_element(channelArr, parseChannelList, (gpointer) t); //pointer to the team

    teamMap.emplace(t->GetTeamId(),t);
}

void parseChannelList(JsonArray* array, guint index_, JsonNode* element_node, gpointer user_data){
    //  IMPORTANT NOTE: At this time, this callback DOES NOT fill up the channel message array, this is to be done outside of this API call as a separate call 

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

    Team* createdTeam = (Team*)user_data;
    createdTeam->GetChannelList().push_back(channel);
    
    channelMap.emplace(channel->GetChannelId(),channel);
}