#include <string>
#include <iostream>
#include <fstream>

#include <libsoup/soup.h>

#include <json-glib/json-glib.h>

#include "../include/test.h"
#include "../include/polling.h"
#include "../include/messaging.h"
#include "../include/fetch.h"
#include "../include/admin.h"
#include "../include/app.h"

int main(int argc, char *argv[]){

    // std::cout<<"\n\n Fetching Teams Next: \n\n";
    //return testFetchTeams();
    // std::cout<<"\n\n Fetching Channel Message Next: \n\n";
    // testFetchChannelMessage(); 
    //std::cout<<"\n\n Fetching Users Info Next: \n\n";
    //return testingFetchUsers();
    
    /***TESTING ISOLATED FUNCTIONALITY***/
    //return testPolling();
    //return  testFetching();
    //return testMessaging();
    //return  testCreateTeam();
    //return testCred();

    //return testScript();

    return runConsoleApp();
}

int testScript(){
    system("./trigger-login.sh");
    std::cout << "Post script\n";
    return 0;
}

//read auth creds from local file
bool readCredentials(std::string &skypeToken,std::string &chatSvcAggToken,std::string &skypeSpacesToken, std::string &currUserId) {
    system("./trigger-login.sh");

    // Add this to the front of the path if generated credentials with npm run login directly from the ms-teams-client directory "../../"
    // Leave it as "ms-teams-credentials.local.json" if running the python script from soup-client dir
    std::string credFilename = "../../ms-teams-credentials.local.json"; 

    JsonParser *parser = json_parser_new();
    GError *err;

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

    initPolling(session,loop,skypeToken);

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
	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

    //message id
    std::string messageId = "1634591623619";

    //I would replace these w/ currUserId, but atm it wouldn't work since both of us log in (would require fetchTeams(?) first I believe)
    //John id
	std::string senderUserId = "fdb3a4e9-675d-497e-acfe-4fd208f8ad89";
	//Olga id
	std::string receiverUserId = "7b30ff05-51b2-490a-b28b-2d8ac36cad8e";

    SoupSession *session = soup_session_new();

    std::string msgtext = "Logged in via bash script triggered from native code";
    //sendMessageSync(session,msgtext,skypeToken,channelId);
    sendChannelMessage(session,loop,msgtext,skypeToken,channelId);
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

//testing fetchTeams Sync/Async options
int testFetchTeams(){
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);

    //John/Olga channel
    std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    SoupSession *session = soup_session_new();

    //fetchTeamsSync(session,chatSvcAggToken); //for testing the Sync Version
    fetchTeams(session,chatSvcAggToken, loop);
    g_main_loop_run (loop);
    //g_main_loop_quit (loop); //for when testing the Sync Version
    g_main_loop_unref (loop);

    return 0;
}

int testFetchChannelMessage(){
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);
    
	//John/Olga channel
	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    SoupSession *session = soup_session_new();

	fetchChannelMessages(chatSvcAggToken, channelId, channelId, 5,loop, session);

    g_main_loop_run (loop);
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
    
    std::vector<std::string> userIds;
    userIds.push_back(JohnId);
    userIds.push_back(OlgaId);

    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    SoupSession *session = soup_session_new();

    fetchUsersInfo(session,chatSvcAggToken, loop, userIds);

    g_main_loop_run (loop);
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
    std::string name = "Fresh Channel";
    //createTeamName(session,loop,skypeSpacesToken,name);
	//createTeam(session,loop,skypeSpacesToken,name,description);
    createChannel(session,loop,skypeSpacesToken,teamId,name);

    g_main_loop_run (loop);

    g_main_loop_unref (loop);
    g_object_unref(session);

    return 0;
}

//This testing did not turn out super useful XD
int testingJson(){

    // std::string credFilename = "fetchUsersInfo.local.json";
    // JsonParser *parser = json_parser_new();
    // GError *err;

    // if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
    //     std::cout<<"\nHey, I am able to parse this file :D How cool!\n";

    //     JsonReader *reader = json_reader_new(json_parser_get_root(parser));
    //     json_reader_read_member(reader,"value");
    //     std::cout<< json_reader_get_string_value (reader);        
    //     json_reader_set_root(reader,json_parser_get_root(parser));
    //     std::cout << "Is the reader standing on an array right now? What about now? "<<json_reader_is_array(reader) + "\n";

    //     json_reader_read_member(reader,"userPrincipalName");
    //     std::cout<< json_reader_get_string_value (reader);
    //     // json_reader_set_root(reader,json_parser_get_root(parser));
    //     // std::string temp=json_reader_get_string_value(reader);
    //     // std::cout<<"\n guat?" + temp + "\n";
    //     // //JsonArray* arr= (JsonArray*)json_reader_get_value(reader);

    //     // json_reader_read_element (reader, 0);

    //     // json_reader_read_member(reader,"givenName");

    //     // // std::string first=json_reader_get_string_value(reader);

    //     // const char *str_value = NULL;

    //     // //json_reader_read_element (reader, 0);
    //     // str_value = json_reader_get_string_value (reader);
    //     //json_reader_end_element (reader);

    //     // json_reader_read_element (reader, 2);
    //     // str_value = json_reader_get_string_value (reader);
    //     // json_reader_end_element (reader);
    // }
    // else{
    //     g_print ("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
    //     g_error_free (err);
    //     g_object_unref (parser);

    // }

    return 0;
}