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
    //std::map<std::string*, User*>* userMap = (std::map<std::string, User*>*)g_ptr_array_index(data_arr,2); //getting the user map

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

    displayResponseInfo( msg, true, "fetchChannelMessagesInfo.local.json");
    GPtrArray *data_arr = (GPtrArray*)user_data;
    std::string credFilename = "fetchChannelMessagesInfo.local.json";
    JsonParser *parser = json_parser_new();
    GError *err;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        //using JsonObject* to read the array from here rather than from a JsonNode* since array is a complex type
        
        std::cout<<"\nParsing worked?\n";
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
//(maybe will get the container id here and pass it to the other callback = jsonArrayGetChannelMessages) - can't do bc falling out of scope etc.
void middleLayerChannelMessages(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){
   std::cout<<"\nInside Middel Layer?\n";
    JsonObject* currObj =json_array_get_object_element(array, index_);

    // JsonNode* value = json_object_get_member(currObj, "containerId");
    // std::string tempStr = json_node_get_string(value);
    
    // GPtrArray *data_arr = (GPtrArray*)user_data;
    // std::vector<Message> *msgVect = (std::vector<Message> *)g_ptr_array_index(data_arr, 0);
    // // g_ptr_array_add(data_arr, &tempStr);

    // GPtrArray * new_user_data = g_ptr_array_new();
    // g_ptr_array_add(new_user_data, msgVect);  //0 data array from olf with vector at 0 and loop at 1
    // g_ptr_array_add(new_user_data, &tempStr);  //1 the container id



    JsonArray* messageArr= json_object_get_array_member(currObj, "messages");

    json_array_foreach_element(messageArr, jsonArrayGetChannelMessages, (gpointer) user_data);// user_data);


}


//lower level callback
void jsonArrayGetChannelMessages(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data){
    
    std::cout<<"\nInside Lower Layer?\n";
   
   
    GPtrArray *data_arr = (GPtrArray*)user_data; //(GPtrArray*)g_ptr_array_index(greatestArr, 0);
    int i = (int) index_;
       
    std::vector<Message> *msgVect = (std::vector<Message> *)g_ptr_array_index(data_arr, 0);
    

    // for (Message i : *(msgVect) ){
    //     std::cout<<i.GetMsgContent()<<std::endl;
    // }


    Message t;
    t.SetMsgContainerChannelId("");  // THIS NEEDS TO BE UPDATED BY THE CHANNEL WITH ITS OWN ID OR SOMETHING

   // std::cout<<"\nThis the cointained id: " + t.GetMsgContainerChannelId() + "\n";
    

    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected

    JsonNode* value = json_object_get_member(currObj, "content");
    //std::string tempStr = json_node_get_string(value);
    t.SetMsgContent(json_node_get_string(value));

    std::cout<<"\nThis the content of messaeg" + t.GetMsgContent() + "\n";

    value=json_object_get_member(currObj, "id");

    t.SetMsgId(json_node_get_string(value));

    std::cout<<"\nThis the id of messaeg" + t.GetMsgId() + "\n";

    value=json_object_get_member(currObj, "parentMessageId");
    t.SetMsgParentId(json_node_get_string(value));
    std::cout<<"\nThis the parent id of messaeg " + t.GetMsgParentId() + "\n";

    // value=json_object_get_member(currObj, "sequenceId");
    // t.SetMsgSequenceId(json_node_get_string(value));
    // std::cout<<"\nThis the sequence id of messaeg " + t.GetMsgSequenceId() + "\n";

    value=json_object_get_member(currObj, "from");
    t.SetSenderMri(json_node_get_string(value));

    value=json_object_get_member(currObj, "originalArrivalTime");
    t.SetArrivalTime(json_node_get_string(value));

    std::cout <<"Before push_back ";
    msgVect->push_back(t);

    // std::string containerChannelId;


    // userInfo =json_object_get_member(currObj, "displayName");
    // std::string displayName = json_node_get_string(userInfo);

    // userInfo =json_object_get_member(currObj, "email");
    // std::string email = json_node_get_string(userInfo);

    // userInfo =json_object_get_member(currObj, "mri");
    // std::string mri = json_node_get_string(userInfo);

    // // userVect->at(i)->SetUserOid(objectId); //don't need this since oid is passed through the User object
    // userVect->at(i)->SetUserDisplayName(displayName);
    // userVect->at(i)->SetUserEmail(email);
    // userVect->at(i)->SetUserMri(mri);


}



