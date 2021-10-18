#pragma once
#include <libsoup/soup.h>

void readCredentials(std::string &skypeToken, std::string &chatSvcAggToken);

void fetchTeamsSync(SoupSession *session, std::string &chatSvcAggToken);
void fetchTeams(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop_param);//gpointer);

void sendMessageSync(SoupSession *session, std::string &text, std::string &skypeToken, std::string &params);
void sendMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &params);
void queueMessageCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void sendChannelMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId);
void sendReplyMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId, std::string &messageId);
void sendDirectMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &senderUserId, std::string &receiverUserId);
