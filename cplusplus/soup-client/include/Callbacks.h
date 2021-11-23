#pragma once
#include <iostream>
#include <fstream>
#include <libsoup/soup.h>
#include <vector>
#include <string>
#include <json-glib/json-glib.h>
#include <map>
#include "../include/User.h"
#include "../include/fetch.h"



void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);  //will pass both the loop and the callback within the user data pointer 
void fetchChannelMessagesCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void middleLayerChannelMessages(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data); 


void jsonArrayGetUsers(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data); 
void jsonArrayGetChannelMessages(  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data); 

//MOVE DISPLAY HERE