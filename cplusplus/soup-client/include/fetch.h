#pragma once
#include <iostream>
#include <fstream>
#include <libsoup/soup.h>

void fetchTeamsSync(SoupSession *session, std::string &chatSvcAggToken);
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop_param);
void fetchTeamsCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);//(GObject *obj, GAsyncResult *res, gpointer user_data);
