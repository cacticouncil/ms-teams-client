#pragma once
#include <string>

bool readCredentials(std::string &skypeToken, std::string &chatSvcAggToken, std::string &skypeSpacesToken);

int testCred();
int testPolling();
int testMessaging();
int testSoup();
int testFetchTeams();
int testFetchChannelMessage();
int testingJson();
int testingFetchUsers();
int testCreateTeam();
