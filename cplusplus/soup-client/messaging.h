#pragma once

void readCredentials(std::string &skypeToken, std::string &chatSvcAggToken);

void fetchTeamsSync(SoupSession *session, std::string &chatSvcAggToken);
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken);

void sendMessage(SoupSession *session, std::string &text, std::string &skypeToken, std::string &params);
void sendMessageCallback(GObject *obj, GAsyncResult *res, gpointer user_data);
void sendChannelMessage(SoupSession *session, std::string &text, std::string &skypeToken, std::string &channelId);
//cpr::AsyncResponse sendMessage(std::string text, std::string& skypeToken, std::string& params);