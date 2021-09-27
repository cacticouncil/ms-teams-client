#pragma once

void testConnectionAPI();

void readCredentials(std::string& skypeToken, std::string& chatSvcAggToken);

void fetchTeams(std::string& chatSvcAggToken);

void sendMessage(std::string text, std::string& skypeToken, std::string& params);
void sendChannelMessage(std::string text, std::string& skypeToken, std::string& channelId);
void sendReplyMessage(std::string text, std::string& skypeToken, std::string& channelId, std::string& messageId);