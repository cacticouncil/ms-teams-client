#pragma once

void createTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, std::string &description);
void createTeamCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void createChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &teamId, std::string &name, std::string &description);