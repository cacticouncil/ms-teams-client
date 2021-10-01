#pragma once

void testConnectionAPI();

void readCredentials(std::string& skypeToken, std::string& chatSvcAggToken);

void fetchTeams(std::string& chatSvcAggToken);
void fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize);

void sendMessage(std::string text, std::string& skypeToken, std::string& params);
void sendChannelMessage(std::string text, std::string& skypeToken, std::string& channelId);
void sendReplyMessage(std::string text, std::string& skypeToken, std::string& channelId, std::string& messageId);
void sendDirectMessage(std::string text, std::string& skypeToken, std::string& senderUserId, std::string& receiverUserId);