#include <string>

#include <libsoup/soup.h>

#include <json-glib/json-glib.h>

#include "../include/admin.h"

//posts propsed name to endpoint; response is team alias
void createTeamName(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, SoupSessionCallback nameValidationCallback){
    //initialize soup message with url and method
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/validateNaming";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{\"displayName\":\""+name+"\",\"displayNamePrefix\":null,\"displayNameSuffix\":null,\"mailNicknamePrefix\":null,\"mailNicknameSuffix\":null,\"groupId\":null}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //auth
    std::string tokenstr = "Bearer " + skypeSpacesToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    soup_session_queue_message(session,msg,nameValidationCallback,loop);
}

//added skypeSpacesToken to login/credentials
void createTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, SoupSessionCallback createTeamCallback){
    //initialize soup message with url and method
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/create";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{\"alias\":\""+name+"\",\"displayName\":\""+name+"\",\"displayNamePrefix\":null,\"displayNameSuffix\":null,\"description\":\"\",\"validationRequired\":false,\"isTenantWide\":false,\"accessType\":1}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //auth
    soup_message_headers_append(msg->request_headers,"Authorization",skypeSpacesToken.c_str());

    soup_session_queue_message(session,msg,createTeamCallback,loop);
}

//Error 401 {"errorCode":"UnauthorizedAccess"}
void createChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &teamId, std::string &name){
    std::string url = "https://teams.microsoft.com/api/mt/part/amer-02/beta/teams/"+teamId+"/channels";
    SoupMessage *msg = soup_message_new(SOUP_METHOD_POST,url.c_str());

    //set message request payload
    std::string payload = "{\"displayName\":\""+name+"\",\"description\":\"\",\"groupId\":\"3c0db051-2932-4bac-8fff-38a23e2febe2\"}";
    soup_message_set_request(msg,"application/json",SOUP_MEMORY_COPY,payload.c_str(),strlen(payload.c_str()));

    //auth
    std::string tokenstr = "Bearer " + skypeSpacesToken;
    soup_message_headers_append(msg->request_headers,"Authorization",tokenstr.c_str());

    soup_session_queue_message(session,msg,nullptr,loop);
}

//parse name validation response (if 2xx code) and call createTeam with response alias name (if parse successful)
void validateNameCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Validation Response: %s\n",msg->response_body->data);

        JsonParser *parser = json_parser_new();
        GError *err;

        if(json_parser_load_from_data(parser,msg->response_body->data,strlen(msg->response_body->data),&err)){
            JsonReader *reader = json_reader_new(json_parser_get_root(parser));

            json_reader_read_member(reader,"value");
            json_reader_read_member(reader,"mailNickname");
            std::string name = json_reader_get_string_value(reader);

            g_object_unref(reader);
            g_object_unref(parser);

            std::string skypeSpacesToken = soup_message_headers_get_one(msg->request_headers,"Authorization");
            createTeam(session,(GMainLoop *)user_data,skypeSpacesToken,name,teamCreateCallback);
        }
        else{
            g_printerr("VALIDATION ERROR: Unable to parse response: %s\n", err->message);
            g_error_free (err);
            g_object_unref (parser);

            GMainLoop *loop = (GMainLoop *)user_data;
            g_main_loop_quit(loop);
        }
    }
    else{
        g_printerr("VALIDATION ERROR: Code: %d\nResponse: %s\n",msg->status_code,msg->response_body->data);

        GMainLoop *loop = (GMainLoop *)user_data;
        g_main_loop_quit(loop);
    }
}

//callback to execute after message sent
void teamCreateCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){
    if(msg->status_code >= 200 && msg->status_code < 300){
        g_print("Response: %s\n",msg->response_body->data);
    }
    else{
        g_printerr("ERROR: Code: %d\nResponse: %s\n",msg->status_code,msg->response_body->data);
    }

    GMainLoop *loop = (GMainLoop *)user_data;
    g_main_loop_quit(loop);
}