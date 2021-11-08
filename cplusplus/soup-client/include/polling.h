#pragma once

void initPolling(SoupSession *session, GMainLoop *loop, std::string &skypeToken);
void pollEndpointCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void ArrayCallback(JsonArray* arr,guint index,JsonNode *elem,gpointer user_data);
void poll(SoupSession *session, GMainLoop *loop, std::string &skypeToken, std::string &endpointUrl);
void pollCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);