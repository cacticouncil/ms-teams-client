#include <json-glib/json-glib.h>
#include <iostream>
#include <fstream>

#include <libsoup/soup.h>

#include "../include/test.h"
#include "../include/polling.h"
#include "../include/messaging.h"
#include "../include/fetch.h"

int main(int argc, char *argv[]){
    //return testPolling();
    //return  testFetching();
    //return  testFetchChannelMessage();
    //"fetchChannelMessagesInfo.txt"
    /*   if (argc < 2)
    {
      g_print ("Usage: test <filename.json>\n");
      return EXIT_FAILURE;
    }
    */
    std::string filename = "fetchTeamsInfo.txt";
    return testingJson(filename);
}

//read auth creds from local file
void readCredentials(std::string &skypeToken,std::string &chatSvcAggToken) {
	std::ifstream credFile("ms-teams-credentials.local.txt");
	if (credFile.is_open()) {
		getline(credFile, skypeToken);
		getline(credFile, chatSvcAggToken);

		credFile.close();
	}
}

//test polling system
int testPolling(){
    std::string skypeToken;
	std::string chatSvcAggToken;

    readCredentials(skypeToken, chatSvcAggToken);
    //remove new line from skypeToken
    skypeToken.erase(skypeToken.size()-1,1);

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

    readCredentials(skypeToken, chatSvcAggToken);
    //remove new line from skypeToken
    skypeToken.erase(skypeToken.size()-1,1);

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

    std::string msgtext = "dm test";
    //sendMessageSync(session,msgtext,skypeToken,channelId);
    //sendChannelMessage(session,loop,msgtext,skypeToken,channelId);
    //sendReplyMessage(session,loop,msgtext,skypeToken,channelId,messageId);
    sendDirectMessage(session,loop,msgtext,skypeToken,senderUserId,receiverUserId);

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
    readCredentials(skypeToken, chatSvcAggToken);

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
    readCredentials(skypeToken, chatSvcAggToken);
    
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

int testingJson(std::string filename){
    JsonParser *parser;
    JsonNode *root;
    GError *error;
    parser = json_parser_new ();

    error = NULL;
    json_parser_load_from_file (parser,filename.c_str(), &error);
    if (error){
        g_print ("Unable to parse `%s': %s\n", filename.c_str(), error->message);
        g_error_free (error);
        g_object_unref (parser);
        return EXIT_FAILURE;
    }

    g_print("Got past error section just fine");
    root = json_parser_get_root (parser);
    JsonReader *reader = json_reader_new (root);
    //reader->parent_instance.qdata;

    /* manipulate the object tree and then exit */

    g_object_unref (parser);
    
    return 0;
}
