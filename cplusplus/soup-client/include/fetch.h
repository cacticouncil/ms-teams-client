#pragma once
#include <iostream>
#include <fstream>
#include <libsoup/soup.h>
#include <vector>
#include <string>
#include <json-glib/json-glib.h>
#include "../include/Team.h"
#include "../include/User.h"
#include "../include/Channel.h"
#include "../include/Message.h"

void fetchTeamsSync(SoupSession *session, std::string &chatSvcAggToken);
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop_param);
void fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize, GMainLoop* loop, SoupSession* session);
void fetchUsersInfo(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, std::map<std::string*, User*>* userMap,SoupSessionCallback callback);//, JsonArrayForeach jArrCallback ); //JsonArrayForeach* jArrCallback

void fetchTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);//(GObject *obj, GAsyncResult *res, gpointer user_data); 
void fetchChannelMessagesCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
//void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);  //will pass both the loop and the callback within the user data pointer 

//void jsonArrayGetUsers(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data); 
void jsonArrayFetchTeams(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data);
void jsonArrayChannelList(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data);

void displayResponseInfo( SoupMessage *msg, bool shouldPrint, std::string filename);