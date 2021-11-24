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
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop_param,  SoupSessionCallback callback, GPtrArray* callback_data); //need to add the class parameters here etc
void fetchChannelMessages(SoupSession* session, std::string& chatSvcAggToken, GMainLoop* loop, Team* t, Channel* channel, int pageSize, SoupSessionCallback callback, GPtrArray* callback_data);//,GPtrArray* callback_data);
void fetchUsersInfo(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, std::vector<User*>* userVect,SoupSessionCallback callback,GPtrArray* callback_data);//, JsonArrayForeach jArrCallback ); //JsonArrayForeach* jArrCallback

//Moving all of these functions to the callbacks file rather than the API fucntions file

    //(GObject *obj, GAsyncResult *res, gpointer user_data); 
//void fetchChannelMessagesCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
//void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);  //will pass both the loop and the callback within the user data pointer 

//void jsonArrayGetUsers(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data); 

/*
void fetchTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data); 

void jsonArrayFetchTeams(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data);
void jsonArrayChannelList(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data);

*/



void displayResponseInfo( SoupMessage *msg, bool shouldPrint, std::string filename);