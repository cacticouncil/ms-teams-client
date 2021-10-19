#pragma once

void fetchPollingEndpoint(SoupSession *session, GMainLoop *loop, std::string &skypeToken);
void pollEndpointCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);