#include <iostream>
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
//In the future a function pointer would be passed here to the callback rather than having the callback here. this is temporary.
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, SoupSessionCallback callback, GPtrArray* callback_data){
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    std::string tokenstr = "Bearer " + chatSvcAggToken;

    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,callback,(gpointer)loop/* callback_data */);//loop);  //fetchTeamsCallback
}

//This function is used to obtain the messages associated with a specific team channel
//This can be used to figure out which is the latest message in the channel etc

//void fetchChannelMessages(SoupSession* session, std::string& chatSvcAggToken,  GMainLoop* loop, std::string& teamId, std::string& channelId, int pageSize) //old
void fetchChannelMessages(SoupSession* session, std::string& chatSvcAggToken, GMainLoop* loop, Team* team, Channel* channel, int pageSize,  SoupSessionCallback callback, GPtrArray* callback_data){
    std::string teamId= team->GetTeamId();
    std::string channelId=channel->GetChannelId();
    //formulating the url
    std::string url = "https://teams.microsoft.com/api/csa/api/v2/teams/" + teamId + "/channels/" + channelId;
    url += "?";
    url += "pageSize=" + std::to_string(pageSize);
    if(teamId == channelId) url += "&filterSystemMessage=true";
    std::cout << "Url: " << url << "\n";
    //Initializing the mesage after url is fully constructed
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET, url.c_str());

    //auth
    std::string tokenstr = "Bearer " + chatSvcAggToken;
    std::cout << "Token: " << tokenstr << "\n";
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,callback,(gpointer)callback_data);//fetchChannelMessagesCallback

}

//Given an array of oid values, this function returns the information associated with 
//Might be used for fetching teh information from users in a channel?
void fetchUsersInfo(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, std::vector<User*>* userVect , SoupSessionCallback callback, GPtrArray* callback_data){//, JsonArrayForeach jArrCallback ){ //will receive the callback here 
   
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/users/fetchShortProfile?isMailAddress=false&enableGuest=true&includeIBBarredUsers=true&skypeTeamsInfo=true";
    
    //Constructing the string array with the users' oids
    std::string userIdsStr= "[";

    for (int i=0; i< userVect->size(); i++){
        
        std::string v =  userVect->at(i)->GetUserOid();// GetUserOid();
     

        userIdsStr+= "\"" + v + "\"" ;

        if(i!=userVect->size()-1){
            userIdsStr+=",";
        }

    }
    userIdsStr+= "]";

    //This logic is replaced by creating the array outside this function and passing it in
    /* GPtrArray *user_data = g_ptr_array_new();
    g_ptr_array_add(user_data,userVect);  //0
    g_ptr_array_add(user_data,loop);  //1 */

   
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST, url.c_str());
    std::string payload = userIdsStr;
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());
    soup_session_queue_message(session,msg,callback,(gpointer)callback_data);
}


//This function extracts information from the response object 
//It displays the response to the console if shouldPrint is true
//It also supports the option to print to a file given its name with extension
//To only print to console pass in an empty string
void displayResponseInfo( SoupMessage *msg,  bool shouldPrint, std::string filename){
    if(shouldPrint){
        if (msg->status_code >= 200 && msg->status_code < 300){
            g_print("Response: %s\n",msg->response_body->data);
        }
        else{
            g_printerr("ERROR: Code: %d\n",msg->status_code);
        }
    }
    
    if(filename != ""){ //if it's not an empty string means you want info printed to a file
        std::ofstream responseFile(filename);
        if (responseFile.is_open()) {
                responseFile << msg->response_body->data;
                responseFile.close();
                std::cout<<"\nWrote response to file: " + filename + "\n";
        }
       
    }
}
