#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include <libsoup/soup.h>

#include <json-glib/json-glib.h>

#include "../include/test.h"
#include "../include/polling.h"
#include "../include/messaging.h"
#include "../include/fetch.h"
#include "../include/admin.h"
#include "../include/app.h"
#include "../include/Team.h"
#include "../include/Callbacks.h"

int main(int argc, char *argv[]){

    //return runConsoleApp();
    //return testingFetchUsers();
    //return testFetchChannelMessages();
    //return testCreateTeam();
    //return testDeleteChannel();
    //return testDeleteTeam();
    return testFetchTeams();
}

int testScript(){
    system("./trigger-login.sh");
    std::cout << "Post script\n";
    return 0;
}

//read auth creds from local file
bool readCredentials(std::string &skypeToken,std::string &chatSvcAggToken,std::string &skypeSpacesToken, std::string &currUserId) {
    //system("./trigger-login.sh");

    // Add this "../../" to the front of the path if generated credentials with npm run login directly from the ms-teams-client directory 
    // Leave it as "ms-teams-credentials.local.json" if running the python script from soup-client dir
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

int testCred(){
    std::string skypeToken;
	std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;

    bool status = readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);
    if(status){
        std::cout << "<<<" << skypeToken << ">>>\n";
        std::cout << "\n<<<" << chatSvcAggToken << ">>>\n";
        std::cout << "\n<<<" << skypeSpacesToken << ">>>\n";
        std::cout << "\nUser ID: " << currUserId << "\n";

        return 0;
    }
    return 1;
}

//test polling system
int testPolling(){
    std::string skypeToken;
	std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);

    GMainLoop *loop = g_main_loop_new(NULL,false);

    SoupSession *session = soup_session_new();

    initPolling(session,loop,skypeToken,initPollCallback);

    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    g_object_unref(session);

    return 0;
}

//test message sending
int testMessaging(){
    std::string skypeToken;
	std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);

    GMainLoop *loop = g_main_loop_new(NULL,false);

    //John/Olga channel
	std::string channelId = "19:5c7c73c0315144a4ab58108a897695a9@thread.tacv2";

    //message id
    std::string messageId = "1636669196234";

    //I would replace these w/ currUserId, but atm it wouldn't work since both of us log in (would require fetchTeams(?) first I believe)
    //John id
	std::string senderUserId = "fdb3a4e9-675d-497e-acfe-4fd208f8ad89";
	//Olga id
	std::string receiverUserId = "7b30ff05-51b2-490a-b28b-2d8ac36cad8e";

    SoupSession *session = soup_session_new();

    std::string msgtext = "Testing abstracted API callback";
    //sendMessageSync(session,msgtext,skypeToken,channelId);
    sendChannelMessage(session,loop,msgtext,skypeToken,channelId,queueMessageCallback);
    //sendReplyMessage(session,loop,msgtext,skypeToken,channelId,messageId);
    //sendDirectMessage(session,loop,msgtext,skypeToken,senderUserId,receiverUserId);

    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    g_object_unref(session);

    return 0;
}

//libsoup/glib hello world
int testSoup(){
    //glib Hello World
    GString* my_string = g_string_new("This Hello world is %d chars long\n");
    g_print(my_string->str, my_string->len);
    g_string_free(my_string, TRUE);

    //libsoup test
    SoupSession *session = soup_session_new ();
    //SoupMessageHeaders *response_headers;

    SoupMessage *msg = soup_message_new (SOUP_METHOD_GET, "https://upload.wikimedia.org/wikipedia/commons/5/5f/BBB-Bunny.png");

    //GInputStream *stream;
    GError *error = NULL;
    GInputStream *stream = soup_session_send(session,msg,NULL,&error);

    if (error) {
        g_printerr ("Failed to download: %s\n", error->message);
        g_error_free (error);
        g_input_stream_close(stream,NULL,&error);
        g_object_unref (msg);
        g_object_unref (session);
        return 1;
    }

    //response_headers = msg->response_headers;
    //const char *content_type;
    const char *content_type = soup_message_headers_get_content_type (msg->response_headers,NULL);

    // content_type = "image/png"
    // bytes contains the raw data that can be used elsewhere
    GBytes *bytes = g_input_stream_read_bytes(stream,8192,NULL,NULL);
    g_input_stream_close(stream,NULL,NULL);
    g_print ("Downloaded %zu bytes of type %s\n",
             g_bytes_get_size (bytes), content_type);
    delete[] content_type;

    g_bytes_unref (bytes);
    g_object_unref (msg);
    g_object_unref (session);

    return 0;
}

//testing fetchTeams Async
int testFetchTeams(){
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);

    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    SoupSession *session = soup_session_new();

    std::vector<Team> teamList;

    GPtrArray *user_data = g_ptr_array_new();
    g_ptr_array_add(user_data,&teamList);  //0 team vector (each team has its own channel vector, which will aslo be filled out)
    g_ptr_array_add(user_data,loop);   //1 loop

    fetchTeams(session,chatSvcAggToken, loop, fetchTeamsCallback , user_data);



    //after teams is full, we will fill up the channelMessages using that API functionality, not exactly sure about the order in the loop etc

    g_main_loop_run (loop);


    std::cout << "\n\n TOTAL NUMBER OF TEAMS IN LSIT: "<<teamList.size() <<std::endl;

    for (Team temp: teamList){
        temp.GetTeamSummary();
    }

    std::cout<<teamList[0].GetTeamDisplayName();

    g_main_loop_unref (loop);

    return 0;
}

int testFetchChannelMessages(){
    //Basic Testing of Class Relationships
/*
    Team inQuestion;

    Channel myChannel;

    //If we wanted to add a message to the vector (obviously with all the other info filled up as well)
    Message m;
    m.SetMsgContent("Empty Content\n");
    Message x;
    x.SetMsgContent("X Message\n");
    inQuestion.GetChannelList().push_back(&myChannel);
    myChannel.GetChannelMgs().push_back(m);
    myChannel.GetChannelMgs().push_back(x);

    for (Channel* c: inQuestion.GetChannelList()){
        for (Message i : c->GetChannelMgs() ){
            std::cout<<i.GetMsgContent()<<std::endl;
        }
    }
*/   

   
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);
    
	//John/Olga Team
    std::string teamId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

    //Channel Id for "Creating Channel" Channel of that team
    std::string channelId ="19:5c7c73c0315144a4ab58108a897695a9@thread.tacv2";

    Team team;
    team.SetTeamId(teamId);
    Channel channel;
    channel.SetChannelId(channelId);

    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    SoupSession *session = soup_session_new();

    GPtrArray *msgs_callback_data = g_ptr_array_new();
    g_ptr_array_add(msgs_callback_data,&channel.GetChannelMgs()); //0
    g_ptr_array_add(msgs_callback_data,loop);//1
   

    //This API call is specific for channel message populaiton.
    //Any testing done here with the Team and Channel object are focusing on that, the rest of the fields for these claases (Channel and Team) are empty for this testing
    //Only testing channel messages gets populated
	fetchChannelMessages(session,chatSvcAggToken, loop, &team, &channel, 5, fetchChannelMessagesCallback, msgs_callback_data );



    g_main_loop_run (loop);


    std::cout<< "\n\n AFTER THE API CALLS: \n"<<std::endl;
    
    for (Message i : channel.GetChannelMgs() ){
        std::cout<<i.GetMessageSummary()<<std::endl;
    }

    g_main_loop_unref (loop);

    return 0;
}

int testingFetchUsers(){ 
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);

    //same story as b4 w/ replacing one w/ currUserId
	std::string JohnId = "fdb3a4e9-675d-497e-acfe-4fd208f8ad89";
	std::string OlgaId = "7b30ff05-51b2-490a-b28b-2d8ac36cad8e";
    
    //std::vector<std::string> userIds;
    // userIds.push_back(JohnId);
    // userIds.push_back(OlgaId);

    //std::vector<std::string>;
    // userIds.push_back(JohnId);
    // userIds.push_back(OlgaId);

    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    SoupSession *session = soup_session_new();

    // User john;
    // User olga;
    
    // std::map<std::string, User*> userMap;
    // userMap.emplace(JohnId,&john);
    // userMap.emplace(OlgaId,&olga);


    User john;
    john.SetUserOid("fdb3a4e9-675d-497e-acfe-4fd208f8ad89");
    User olga;
    olga.SetUserOid("7b30ff05-51b2-490a-b28b-2d8ac36cad8e");

    std::vector<User*> userVector;
    userVector.push_back(&john);
    userVector.push_back(&olga);

    GPtrArray *user_data = g_ptr_array_new();
    g_ptr_array_add(user_data,&userVector);  //0
    g_ptr_array_add(user_data,loop);  //1


    fetchUsersInfo(session,chatSvcAggToken, loop, &userVector, fetchUsersInfoCallback, user_data);


    
    // std::cout<<john.GetUserSummary();
    // std::cout<<olga.GetUserSummary();

    //fetchUsersInfo(session,chatSvcAggToken, loop, &userMap, fetchUsersInfoCallback);

    //fetchUsersInfo(session,chatSvcAggToken, loop, userIds, fetchUsersInfoCallback);

    g_main_loop_run (loop);


    std::cout<<john.GetUserSummary() + "\n";
    std::cout<<olga.GetUserSummary();

    g_main_loop_unref (loop);

    return 0;
}
  

int testCreateTeam(){
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);

    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    SoupSession *session = soup_session_new();

    std::string teamId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";
    //19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2
    std::string name = "Testiest of testis";
    //createTeamName(session,loop,skypeSpacesToken,name,validateNameCallback);
	//createTeam(session,loop,skypeSpacesToken,name,description);
    createChannel(session,loop,skypeSpacesToken,teamId,name,skypeToken);

    g_main_loop_run (loop);

    g_main_loop_unref (loop);
    g_object_unref(session);

    return 0;
}

int testDeleteChannel(){
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);

    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    SoupSession *session = soup_session_new();

    std::string teamId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";
    std::string channelId = "19:955c605b9bdb428a846f4e7246c813e6@thread.tacv2";
    deleteChannel(session,loop,skypeSpacesToken,skypeToken,teamId,channelId);

    g_main_loop_run (loop);

    g_main_loop_unref (loop);
    g_object_unref(session);

    return 0;
}

int testDeleteTeam(){
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);

    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    SoupSession *session = soup_session_new();

    std::string teamId = "19:JLj0jdpffy_WxMmxRpkVEYJXsvzIdwhxqOzGvU9OrpM1@thread.tacv2";
    deleteTeam(session,loop,skypeSpacesToken,skypeToken,teamId);

    g_main_loop_run (loop);

    g_main_loop_unref (loop);
    g_object_unref(session);

    return 0;
}
