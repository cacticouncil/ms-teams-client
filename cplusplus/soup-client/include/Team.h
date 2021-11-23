#pragma once
#include <string>
#include <vector>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>
#include <fstream>
#include <iostream>
#include "../include/Channel.h"


class Team{
    
    std::string displayName;
    std::string id;
    std::string totalMemberCount;
    std::string creatorMri;
    std::string groupId;
    std::vector<Channel> channelList; //change to a map of id to Channel
    
    
    public:

    Team(){
        this->displayName = "";
        this->id = "";
        this->totalMemberCount = "";
        this->creatorMri = "";
        this->groupId = "";
    }

    Team(std::string displayName, std::string id, std::string totalMemberCount, std::string creatorMri, std::string groupId, std::vector<Channel> channels);
    
    //Accessors
    std::string GetTeamDisplayName();
    std::string GetTeamId();
    std::string GetTotalMemberCount();
    std::string GetCreatorMri();
    std::string GetTeamGroupId();
    std::vector<Channel>& GetChannelList();

    //Modifiers
    void SetTeamDisplayName(std::string);
    void SetTeamId(std::string);
    void SetTotalMemberCount(std::string);
    void SetCreatorMri(std::string);
    void SetTeamGroupId(std::string);
    void SetChannelList(std::vector<Channel>&);

    std::string GetTeamSummary();

    //fetchUsers Info e=her for something in the future??
};