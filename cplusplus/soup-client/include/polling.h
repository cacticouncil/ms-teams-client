#pragma once

void initPolling(SoupSession *session, GMainLoop *loop, std::string &skypeToken, SoupSessionCallback initPollingCallback);
void poll(SoupSession *session, GMainLoop *loop, std::string &skypeToken, std::string &endpointUrl, SoupSessionCallback pollingCallback);

void initPollCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void ArrayCallback(JsonArray* arr,guint index,JsonNode *elem,gpointer user_data);
void pollCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);