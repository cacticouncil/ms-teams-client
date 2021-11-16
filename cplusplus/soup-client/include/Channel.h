#pragma once
#include <string>
#include <vector>
#include <map>
#include "../include/Message.h"

class Channel{
   
    //ReplyChain channelMsgs;
    std::vector<Message> channelMgs; //replace with map

    //map<msgId, Message>

    //Having a map so that children can find parents quickly
    //Using the refernce variable

    std::string displayName;
    std::string id;
    std::string parentTeamId;


    //Team* to team it belongs in
    public:
    Channel(std::vector<Message>& channelMsgs, std::string displayName="", std::string id="",  std::string parentTeamId= "");

    //Accessors
    std::string GetChannelDisplayName();
    std::string GetChannelId();
    std::string GetChannelTeamId();
    //Modifiers
    void SetChannelDisplayName(std::string);
    void SetChannelId(std::string);
    void SetChannelTeamId(std::string);

};