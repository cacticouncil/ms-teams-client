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

void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop_param,  SoupSessionCallback callback, GPtrArray* callback_data); //need to add the class parameters here etc
void fetchChannelMessages(SoupSession* session, std::string& chatSvcAggToken, GMainLoop* loop, /* Team* team */std::string &teamId, /* Channel* channel */std::string &channelId, int pageSize,  SoupSessionCallback callback, GPtrArray* callback_data);
void fetchUsersInfo(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, /* std::vector<User*>* userVect */std::vector<std::string> &userIds,SoupSessionCallback callback,GPtrArray* callback_data);