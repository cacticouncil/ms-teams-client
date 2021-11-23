#pragma once

void createTeamName(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, SoupSessionCallback nameValidationCallback);
void createTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, SoupSessionCallback createTeamCallback);

void createChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &teamId, std::string &name, std::string &skypeToken);

void validateNameCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);
void teamCreateCallback(SoupSession *session, SoupMessage *msg, gpointer user_data);