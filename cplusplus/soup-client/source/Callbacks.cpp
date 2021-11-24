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
        JsonArray* arr= json_object_get_array_member(rootObj, "teams");  //just like this there s a whole other "chats" array afte the teams with information about group DMs

        json_array_foreach_element(arr, jsonArrayFetchTeams, user_data);
    }
     else{
        g_print ("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
    }
    
    GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr, 1); //1- loop

    g_main_loop_quit(loop);
}


void jsonArrayFetchTeams(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){

    GPtrArray *data_arr = (GPtrArray*)user_data;
 
    std::vector<Team> *teamVect = (std::vector<Team> *)g_ptr_array_index(data_arr, 0); //0-team vect; 1-loop

    //Constructing each individual team

    Team t;

    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected
    JsonNode* value =json_object_get_member(currObj, "displayName"); 
    t.SetTeamDisplayName(json_node_get_string(value));

    value =json_object_get_member(currObj, "id"); 
    t.SetTeamId(json_node_get_string(value));

    value =json_object_get_member(currObj, "creator");
    t.SetCreatorMri(json_node_get_string(value));


    JsonNode* teamValue =json_object_get_member(currObj, "teamSiteInformation");
    JsonObject* teamSiteInfo =json_node_get_object(teamValue); //turing it into an object to get info from its members
    JsonNode* member =json_object_get_member(teamSiteInfo, "groupId");
    std::string groupId =json_node_get_string(member);
    t.SetTeamGroupId(groupId);

  
    JsonNode* membershipValue =json_object_get_member(currObj, "membershipSummary");
    JsonObject* memberObject =json_node_get_object(membershipValue); //turing it into an object to get info from its members
    int count =json_object_get_int_member(memberObject, "totalMemberCount");
    t.SetTotalMemberCount(count);


    std::cout<< "Channel List for " + t.GetTeamDisplayName() + " :\n\n";

    //passing a pointer ot the team to access its vector of channels from withing the next callback (jsonArrayChannelList)
    g_ptr_array_add(data_arr, &t.GetChannelList()); //index 2 for Team channel list

    JsonArray* channelArr= json_object_get_array_member(currObj, "channels");

    json_array_foreach_element(channelArr, jsonArrayChannelList, (gpointer) data_arr);

    teamVect->push_back(t);

}


void jsonArrayChannelList(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){
    //  IMPORTANT NOTE: At this time, this callback DOES NOT fill up the channel message array, this is to be done outside of this API call as a separate call 
       
    GPtrArray *data_arr = (GPtrArray*)user_data;
    std::vector<Channel> * teamChannels= (std::vector<Channel> *)g_ptr_array_index(data_arr, 2);

    Channel channel;

    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected


    JsonNode* value =json_object_get_member(currObj, "displayName"); //member name here
    //std::string channelNameStr = json_node_get_string(channelName);
    channel.SetChannelDisplayName(json_node_get_string(value));

    value = json_object_get_member(currObj, "id"); 
    channel.SetChannelId(json_node_get_string(value));

    value = json_object_get_member(currObj, "parentTeamId"); 
    channel.SetChannelTeamId(json_node_get_string(value));

    //"isMember": false,
    bool isMember = json_object_get_boolean_member(currObj, "isMember");
    channel.SetIsChannelMember(isMember);

    if (isMember){
        value = json_object_get_member(currObj, "creator"); 
        channel.SetChannelCreatorMri(json_node_get_string(value));

        value = json_object_get_member(currObj, "groupId"); 
        channel.SetChannelGroupId(json_node_get_string(value));
    }


    // std::cout<< channel.GetChannelDisplayName() + "\n";

    // std::cout<< "Id: " + channel.GetChannelId() + "\n";


    teamChannels->push_back(channel);
    
}
