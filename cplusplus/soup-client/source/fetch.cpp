#include <string>

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

//fetch teams data asynchronously [not implemented]
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken){
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET,"https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true");

    std::string tokenstr = "Bearer " + chatSvcAggToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    //soup_session_send_async(session,msg,NULL,sendMessageCallback,NULL);
}