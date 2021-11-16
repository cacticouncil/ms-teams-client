#pragma once
#include <string>
#include <vector>
#include "../include/Channel.h"

class Team{
    
    std::string displayName;
    std::string id;
    std::string totalMemberCount;
    std::string creatorMri;
    std::string groupId;
    std::vector<Channel> channelList; //change to a map of id to Channel
    
    
    public:

    Team(){}
    
    //Accessors
    std::string GetTeamDisplayName();
    std::string GetTeamId();
    std::string GetTotalMemberCount();
    std::string GetCreatorMri();
    std::string GetTeamGroupId();

    //Modifiers
    void SetTeamDisplayName(std::string);
    void SetTeamId(std::string);
    void SetTotalMemberCount(std::string);
    void SetCreatorMri(std::string);
    void SetTeamGroupId(std::string);

};