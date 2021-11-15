#pragma once
#include <string>
#include <vector>
#include <map>
#include "../include/Message.h"

class Channel{
    public:
    //ReplyChain channelMsgs;
    std::vector<Message> channelMgs; //replace with map

    //map<msgId, Message>

    //Having a map so that children can find parents quickly
    //Using the refernce variable

    std::string displayName;
    std::string id;
    std::string parentTeamId;

    //Team* to team it belongs in

    Channel(std::vector<Message>& channelMsgs, std::string displayName="", std::string id="",  std::string parentTeamId= "");
};