#pragma once
#include <iostream>
#include <fstream>
#include <libsoup/soup.h>
#include <vector>
#include <string>
#include <json-glib/json-glib.h>

void fetchTeamsSync(SoupSession *session, std::string &chatSvcAggToken);
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop_param);
void fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize, GMainLoop* loop, SoupSession* session);
void fetchUsersInfo(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, std::vector<std::string>& userIds);

void fetchTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);//(GObject *obj, GAsyncResult *res, gpointer user_data);
void fetchChannelMessagesCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);

void jsonArrayGetUsers(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data);
void jsonArrayFetchTeams(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data);
void jsonArrayChannelList(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data);

void displayResponseInfo( SoupMessage *msg, bool shouldPrint, std::string filename);