#pragma once
#include <string>
#include <vector>
#include "../include/Channel.h"

class Team{
    public:

    std::string displayName;
    std::string id;
    std::string totalMemberCount;
    std::string creatorMri;
    std::vector<Channel> channelList; //change to a map of id to Channel

    std::string groupId;

    Team(){}

};