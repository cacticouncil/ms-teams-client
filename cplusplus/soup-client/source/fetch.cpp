#include <libsoup/soup.h>

#include "../include/fetch.h"


//synchronously fetch teams data
void fetchTeamsSync(SoupSession *session, std::string &chatSvcAggToken){
    //init message with url
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    //append bearer auth token to header
    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    GError *error = NULL;
    GInputStream *stream = soup_session_send(session,msg,NULL,&error);

    void *resbuff = malloc(8192);
    g_input_stream_read(stream,resbuff,8192,NULL,&error);

    if (error) {
        g_printerr ("ERROR: %s\n", error->message);
        g_error_free (error);
    }
    else{
        g_print("Success! Code: %d\n",msg->status_code);

        g_print("Response Buffer: %s\n",(char*)resbuff);

        g_input_stream_close(stream,NULL,&error);
        g_object_unref (msg);
        g_object_unref (session);
    }

    free(resbuff);
}

//fetch teams data asynchronously working
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop){
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    std::string tokenstr = "Bearer " + chatSvcAggToken;

    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,fetchTeamsCallback,loop);
}


void fetchTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    displayResponseInfo( msg, "fetchTeamsInfo.local.json");

    GMainLoop *loop = (GMainLoop *) user_data;
    g_main_loop_quit(loop);
}

//This function is used to obtain the messages associated with a specific team channel
//This can be used to figure out which is the latest message int eh channel etc?
void fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize, GMainLoop* loop, SoupSession* session){
    //formulating the url
    std::string url = "https://teams.microsoft.com/api/csa/api/v2/teams/" + teamId + "/channels/" + channelId;
    url += "?";
    url += "pageSize=" + std::to_string(pageSize);
    if(teamId == channelId) url+= "&filterSystemMessage=true";

    //Initializing the mesage after url is fully constructed
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET, url.c_str());

    //auth
    std::string tokenstr = "Bearer " + chatSvcAggToken;

    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,fetchChannelMessagesCallback,loop);

}

void fetchChannelMessagesCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    displayResponseInfo( msg, "fetchChannelMessagesInfo.local.json");

    GMainLoop *loop = (GMainLoop *) user_data;
    g_main_loop_quit(loop);
}

//Given an array of oid values, this function returns the information associated with 
//Might be used for fetching teh information from users in a channel?
void fetchUsersInfo(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, std::vector<std::string>& userIds){
   
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/users/fetchShortProfile?isMailAddress=false&enableGuest=true&includeIBBarredUsers=true&skypeTeamsInfo=true";
    
    std::string userIdsStr= "[";
    for (int i= 0; i<userIds.size(); i++){
        std::string v = userIds[i];
        userIdsStr+= "\"" + v + "\"" ;
        if(i!=userIds.size()-1){
            userIdsStr+=",";
        }
    }
    userIdsStr+= "]";

    //std::cout <<"Constructed user string" <<userIdsStr<< "\n"; //Debug Statement

    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST, url.c_str());
    std::string payload = userIdsStr;
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,fetchUsersInfoCallback,loop);
}

void JsonArraForEach_Function (  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data) {  //(* JsonArrayForeach) 
    std::cout<<"Currently in iteration " + std::to_string((int)index_);
    JsonObject* currObj =json_array_get_object_element(array, index_);
    JsonNode* user =json_object_get_member(currObj, "userPrincipalName");
    std::string userStr = json_node_get_string(user);
    std::cout<< "\nThis is the princip[al name: " + userStr + "\n";

}

void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    std::string credFilename = "fetchUsersInfo.local.json";
    JsonParser *parser = json_parser_new();
    GError *err;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        std::cout<<"\nHey, I am able to parse this file :D How cool!\n";

         JsonReader *reader = json_reader_new(json_parser_get_root(parser));
         JsonNode* root= json_parser_get_root(parser);
         JsonObject* rootObj= json_node_get_object(root); //getting the object to read the array from here rather than from a JsonNode* since this is a complex type
         JsonArray* arr= json_object_get_array_member(rootObj, "value");

         //json_reader_read_member(reader,"value");
         //json_reader_read_element (reader, 0);
         //json_reader_set_root(reader,json_parser_get_root(parser));
        std::cout << "Is the reader standing on an array right now? "<<json_reader_is_array(reader);

        std::cout<< "\n Count: "<<json_reader_count_elements(reader)<<"\n";
        std::cout << "\n\nIs Object?  "<<json_reader_is_object(reader) << "\n\n";
    //     //initlly 1 

        //JsonArray* arr= (JsonArray*)json_reader_get_value(reader);
        //json_reader_read_element (reader, 0);
        //std::cout << "\n now? "<<json_reader_is_array(reader);
        //std::cout<<"\nReader [0]: "<<json_reader_get_string_value(reader)<<"\n";

        //JsonNode* node= json_reader_get_value(reader);
/*        GValue* g=nullptr;
        json_node_get_value(node, g);
        JsonArray* arr= (JsonArray*)g;
*/
        // std::cout<< "\n Hold Array: "<<JSON_NODE_HOLDS_ARRAY(node)<<"\n";
        //JsonArray* arr = json_node_get_array(node);


        //json_reader_read_element (reader, 1);

        //json_reader_read_member(reader,"givenName");

        //std::string first=json_reader_get_string_value(reader);
        
        // for(int i=0; i<2; i++){
        //     JsonNode* value;
        json_array_foreach_element(arr, JsonArraForEach_Function, user_data); //(* JsonArrayForeach) expecting a fuinction of this type and I am not sure what this entails
        //}

    }
     else{
        g_print ("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);

    }

    //displayResponseInfo( msg, "fetchUsersInfo.local.json");

    GMainLoop *loop = (GMainLoop *) user_data;
    g_main_loop_quit(loop);
}


//This function extracts information from the response object and displays it to the console 
//It also supports the option to print the info to a file given the finelame and extension
//To only print to console pass in an empty string
void displayResponseInfo( SoupMessage *msg, std::string filename){
    
    if (msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\n",msg->status_code);
    }
    
    if(filename != ""){ //if it's not an empty string means you want info printed to a file
        std::ofstream responseFile(filename);
        if (responseFile.is_open()) {
                responseFile << msg->response_body->data;
                responseFile.close();
                std::cout<<"\n Wrote response to file: " + filename + "\n";
        }
       
    }
}
