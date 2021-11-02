#pragma once

void createTeamName(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name);
void validateNameCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);

void createTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name);
void createTeamCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void createChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &teamId, std::string &name);