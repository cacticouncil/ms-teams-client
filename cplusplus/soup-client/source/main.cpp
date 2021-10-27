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

int main(){
    //return testPolling();
    //return  testFetching();
    //return testFetchChannelMessage();
    //return testMessaging();
    return  testCreateTeam();
    //return testCred();
}

//read auth creds from local file
bool readCredentials(std::string &skypeToken,std::string &chatSvcAggToken,std::string &skypeSpacesToken) {
    std::string credFilename = "ms-teams-credentials.local.json";

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

    bool status = readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken);
    if(status){
        std::cout << "<<<" << skypeToken << ">>>\n";
        std::cout << "\n<<<" << chatSvcAggToken << ">>>\n";

        return 0;
    }
    return 1;
}

//test polling system
int testPolling(){
    std::string skypeToken;
	std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken);

    GMainLoop *loop = g_main_loop_new(NULL,false);

    SoupSession *session = soup_session_new();

    fetchPollingEndpoint(session,loop,skypeToken);

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
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken);

    GMainLoop *loop = g_main_loop_new(NULL,false);

    //John/Olga channel
	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

    //message id
    std::string messageId = "1634591623619";

    //John id
	std::string senderUserId = "fdb3a4e9-675d-497e-acfe-4fd208f8ad89";
	//Olga id
	std::string receiverUserId = "7b30ff05-51b2-490a-b28b-2d8ac36cad8e";

    SoupSession *session = soup_session_new();

    std::string msgtext = "testing credentials via json lib";
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

//testing fetchTeams Async
int testFetching(){
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken);

    //John/Olga channel
    std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    SoupSession *session = soup_session_new();

//  fetchTeamsSync(session,chatSvcAggToken); //Sync Version
    fetchTeams(session,chatSvcAggToken, loop);
    g_main_loop_run (loop);
  //  g_main_loop_quit (loop); //for when testing the Sync Version
    g_main_loop_unref (loop);

    return 0;
}

int testFetchChannelMessage(){
    std::cout <<"Starting the test"<<std::endl;

    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken);
    
	//John/Olga channel
	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    SoupSession *session = soup_session_new();

	fetchChannelMessages(chatSvcAggToken, channelId, channelId, 5,loop, session);
//void fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize, GMainLoop* loop, SoupSession* session);
    g_main_loop_run (loop);
    g_main_loop_unref (loop);

    return 0;

}

int testCreateTeam(){
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken);

    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    SoupSession *session = soup_session_new();

    std::string teamId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";
    std::string name = "API Team Test";
    std::string description = "Test Description";
    createTeamName(session,loop,skypeSpacesToken,name);
	//createTeam(session,loop,skypeSpacesToken,name,description);
    //createChannel(session,loop,chatSvcAggToken,teamId,name,description);

    g_main_loop_run (loop);

    g_main_loop_unref (loop);
    g_object_unref(session);

    return 0;
}