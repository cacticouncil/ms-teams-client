#pragma once

void initPolling(SoupSession *session, GMainLoop *loop, std::string &skypeToken, SoupSessionCallback initPollingCallback);
void poll(SoupSession *session, GMainLoop *loop, std::string &skypeToken, std::string &endpointUrl, SoupSessionCallback pollingCallback);