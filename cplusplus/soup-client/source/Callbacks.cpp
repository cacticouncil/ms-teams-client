#include "../include/Callbacks.h"

void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    //Need to write to the file at least once in order to be able to use the json parsing stuff 
    //Alternatively parse from data rather than the file
    //Pass true vs false to printo to console as well
    displayResponseInfo( msg, false, "fetchUsersInfo.local.json"); 
    
    std::string credFilename = "fetchUsersInfo.local.json";
    JsonParser *parser = json_parser_new();
    GError *err;

    GPtrArray *data_arr = (GPtrArray*)user_data;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        //using JsonObject* to read the array from here rather than from a JsonNode* since array is a complex type
        JsonNode* root= json_parser_get_root(parser);
        JsonObject* rootObj= json_node_get_object(root); 
        JsonArray* arr= json_object_get_array_member(rootObj, "value");

        json_array_foreach_element(arr, jsonArrayGetUsers, user_data);
    }
     else{
        g_print ("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
    }


    GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr,1); //give it the index of the loop

    g_main_loop_quit(loop);
}


//Function that gets executed for each element of the Json Array
void jsonArrayGetUsers (  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data) {  

    GPtrArray *data_arr = (GPtrArray*)user_data;
    int i = (int) index_;

    std::vector<User*>* userVect = (std::vector<User*>*)g_ptr_array_index(data_arr,0);
    

    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected


    JsonNode* userInfo =json_object_get_member(currObj, "objectId");
    std::string objectId = json_node_get_string(userInfo);


    userInfo =json_object_get_member(currObj, "displayName");
    std::string displayName = json_node_get_string(userInfo);

    userInfo =json_object_get_member(currObj, "email");
    std::string email = json_node_get_string(userInfo);

    userInfo =json_object_get_member(currObj, "mri");
    std::string mri = json_node_get_string(userInfo);

    // userVect->at(i)->SetUserOid(objectId); //don't need this since oid is passed through the User object
    userVect->at(i)->SetUserDisplayName(displayName);
    userVect->at(i)->SetUserEmail(email);
    userVect->at(i)->SetUserMri(mri);

    
    // std::cout<< "User object version: " + userVect->at(i)->GetUserDisplayName() + " \n\n";

}



void fetchChannelMessagesCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    displayResponseInfo( msg, false, "fetchChannelMessagesInfo.local.json");
    GPtrArray *data_arr = (GPtrArray*)user_data;
    std::string credFilename = "fetchChannelMessagesInfo.local.json";
    JsonParser *parser = json_parser_new();
    GError *err = nullptr;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        //using JsonObject* to read the array from here rather than from a JsonNode* since array is a complex type
        
        JsonNode* root= json_parser_get_root(parser);
        JsonObject* rootObj= json_node_get_object(root); 
        JsonArray* arr= json_object_get_array_member(rootObj, "replyChains");

        json_array_foreach_element(arr, middleLayerChannelMessages, user_data); //middle level callback
    }
     else{
        g_print ("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
    }


    GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr, 1);

    g_main_loop_quit(loop);
}


//middle level callback here 
//goes one layer deeper in array of messages
//(maybe will get the container id here and pass it to the other callback = jsonArrayGetChannelMessages)
void middleLayerChannelMessages(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){

    JsonObject* currObj =json_array_get_object_element(array, index_);

    JsonNode* value = json_object_get_member(currObj, "containerId");
    std::string tempStr = json_node_get_string(value);
    
    //appending the channel id to the callback data for the next callback to assign to the message channel container id 
    GPtrArray *data_arr = (GPtrArray*)user_data;
    g_ptr_array_add(data_arr, &tempStr);

    JsonArray* messageArr= json_object_get_array_member(currObj, "messages");

    json_array_foreach_element(messageArr, jsonArrayGetChannelMessages, (gpointer) data_arr);

}


//lower level callback
void jsonArrayGetChannelMessages(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){
   
    GPtrArray *data_arr = (GPtrArray*)user_data;
    int i = (int) index_;
       
    std::vector<Message> *msgVect = (std::vector<Message> *)g_ptr_array_index(data_arr, 0);
    
    std::string* channId = (std::string*)g_ptr_array_index(data_arr, 2); //Values at index: 0-vect, 1-loop, 2- just added the channel id in the middleLayer callback
    
    Message t;
    t.SetMsgContainerChannelId(*(channId));

    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected

    JsonNode* value = json_object_get_member(currObj, "content");
    t.SetMsgContent(json_node_get_string(value));
    
    value=json_object_get_member(currObj, "id");
    t.SetMsgId(json_node_get_string(value));

    value=json_object_get_member(currObj, "parentMessageId");
    t.SetMsgParentId(json_node_get_string(value));

    int seqId = json_object_get_int_member (currObj, "sequenceId");
    t.SetMsgSequenceId(seqId);

    value=json_object_get_member(currObj, "from");
    t.SetSenderMri(json_node_get_string(value));

    value=json_object_get_member(currObj, "originalArrivalTime");
    t.SetArrivalTime(json_node_get_string(value));

    msgVect->push_back(t);
}






void fetchTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    displayResponseInfo( msg, false, "fetchTeamsInfo.local.json");
    GPtrArray *data_arr = (GPtrArray*)user_data;
    
    std::string credFilename = "fetchTeamsInfo.local.json";
    JsonParser *parser = json_parser_new();
    GError *err = nullptr;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        //using JsonObject* to read the array from here rather than from a JsonNode* since array is a complex type
        JsonNode* root= json_parser_get_root(parser);
        JsonObject* rootObj= json_node_get_object(root); 
        JsonArray* arr= json_object_get_array_member(rootObj, "teams");

        json_array_foreach_element(arr, jsonArrayFetchTeams, user_data);
    }
     else{
        g_print ("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
    }
    
    GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr, 1);
    
    g_main_loop_quit(loop);
}


void jsonArrayFetchTeams(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){

    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected
    JsonNode* teamName =json_object_get_member(currObj, "displayName"); //member name here
    std::string teamNameStr = json_node_get_string(teamName);

    JsonNode* teamId =json_object_get_member(currObj, "id"); //member name here
    std::string teamIdStr = json_node_get_string(teamId);

    std::cout<< "\nTeam: " + teamNameStr + "\n\n";

    std::cout<< "Id: " + teamIdStr + "\n";

    //Next I will attempt to get a list of channels per Team as well

    std::cout<< "Channel List:\n";


    //My attempt to go into the other array
    JsonArray* channelArr= json_object_get_array_member(currObj, "channels");

    json_array_foreach_element(channelArr, jsonArrayChannelList, user_data);

}


void jsonArrayChannelList(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){
    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected
    JsonNode* channelName =json_object_get_member(currObj, "displayName"); //member name here
    std::string channelNameStr = json_node_get_string(channelName);

    JsonNode* channelId =json_object_get_member(currObj, "id"); //member name here
    std::string channelIdStr = json_node_get_string(channelId);

    std::cout<< channelNameStr + "\n";

    std::cout<< "Id: " + channelIdStr + "\n";

    //Not sure exactly which one of these does what so I'll have to do some testing of my own to see which one to use for new message vs for reply to last message
    // "version": 1635343945368,
    // "threadVersion": 1635300285998,
    
}
