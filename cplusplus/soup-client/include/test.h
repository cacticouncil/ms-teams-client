#pragma once

bool readCredentials(std::string &skypeToken, std::string &chatSvcAggToken, std::string &skypeSpacesToken);

int testCred();
int testPolling();
int testMessaging();
int testSoup();
int testFetching();
int testFetchChannelMessage();
int testCreateTeam();
