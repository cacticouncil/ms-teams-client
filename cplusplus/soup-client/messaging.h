#pragma once

void sendMessageSync(SoupSession *session, std::string &text, std::string &skypeToken, std::string &params);
void sendMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &params);
void queueMessageCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void sendChannelMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId);
void sendReplyMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId, std::string &messageId);
void sendDirectMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &senderUserId, std::string &receiverUserId);