#pragma once

int runConsoleApp();

void displayMain(SoupSession *session, GMainLoop *loop, std::string &skypeToken);

struct Auth{
    std::string skypeToken;
    std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
};

bool checkCredentialsValid();
bool readCredentialsOnly(std::string &skypeToken,std::string &chatSvcAggToken,std::string &skypeSpacesToken, std::string &currUserId);

void sendMessageCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void initCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void newEventCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);

void populateUserData(SoupSession *session, SoupMessage *msg, gpointer user_data);

void teamNameValidatedCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void teamCreatedCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);