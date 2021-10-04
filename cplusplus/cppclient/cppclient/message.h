#pragma once

void testConnectionAPI();

void readCredentials(std::string& skypeToken, std::string& chatSvcAggToken);

cpr::AsyncResponse fetchTeams(std::string& chatSvcAggToken);
cpr::AsyncResponse fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize);

cpr::AsyncResponse sendMessage(std::string text, std::string& skypeToken, std::string& params);
cpr::AsyncResponse sendChannelMessage(std::string text, std::string& skypeToken, std::string& channelId);
cpr::AsyncResponse sendReplyMessage(std::string text, std::string& skypeToken, std::string& channelId, std::string& messageId);
cpr::AsyncResponse sendDirectMessage(std::string text, std::string& skypeToken, std::string& senderUserId, std::string& receiverUserId);

void writeResponseFile(cpr::AsyncResponse& fr, std::string filename);
void writeResponseConsole(cpr::AsyncResponse& fr);