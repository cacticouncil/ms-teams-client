#pragma once

void readCredentials(std::string& skypeToken, std::string& chatSvcAggToken);

void sendMessage(SoupSession *session, std::string text, std::string& skypeToken, std::string& params);
void sendMessageCallback(GObject *obj, GAsyncResult *res, gpointer user_data);
//cpr::AsyncResponse sendMessage(std::string text, std::string& skypeToken, std::string& params);