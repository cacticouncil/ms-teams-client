#pragma once

void createTeamName(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, SoupSessionCallback nameValidationCallback);
void createTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &name, SoupSessionCallback createTeamCallback);
void createChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &teamId, std::string &name, std::string &skypeToken, std::string &groupId, SoupSessionCallback channelCreateCallback);

void deleteChannel(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &skypeToken, std::string &teamId, std::string &channelId, SoupSessionCallback deleteCallback);
void deleteTeam(SoupSession *session, GMainLoop *loop, std::string &skypeSpacesToken, std::string &skypeToken, std::string &teamId, SoupSessionCallback deleteCallback);