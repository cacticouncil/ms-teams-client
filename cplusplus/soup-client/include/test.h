#pragma once
#include <string>

void readCredentials(std::string &skypeToken, std::string &chatSvcAggToken);

int testPolling();
int testMessaging();
int testSoup();
int testFetching();
int testFetchChannelMessage();
int testingJson(std::string filename);
int testingFetchUsers();