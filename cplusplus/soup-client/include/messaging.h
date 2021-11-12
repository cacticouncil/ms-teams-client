#pragma once

void sendMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &params, SoupSessionCallback msgCallback);
void sendChannelMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId, SoupSessionCallback msgCallback);
void sendReplyMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &channelId, std::string &messageId, SoupSessionCallback msgCallback);
void sendDirectMessage(SoupSession *session, GMainLoop *loop, std::string &text, std::string &skypeToken, std::string &senderUserId, std::string &receiverUserId, SoupSessionCallback msgCallback);

void queueMessageCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);