#pragma once
#include <string>
#include <vector>
#include <map>
#include "../include/Message.h"

class Channel{
   
    //ReplyChain channelMsgs;
    std::vector<Message> channelMgs; 
    std::string displayName;
    std::string id;
    std::string parentTeamId;
    std::string creatorMri;


    //Team* to team it belongs in
    public:
    
    Channel () {
        this->displayName="";
        this->id="";
        this->parentTeamId="" ;
        this->creatorMri = "";  
    }

    Channel(std::vector<Message> channelMgs, std::string displayName, std::string ,  std::string parentTeamId, std::string creatorMri);
    
    //Accessors
    std::string GetChannelDisplayName();
    std::string GetChannelId();
    std::string GetChannelTeamId();
    std::vector<Message>& GetChannelMgs();
    std::string GetChannelCreatorMri();

    //Modifiers
    void SetChannelDisplayName(std::string);
    void SetChannelId(std::string);
    void SetChannelTeamId(std::string);
    void SetChannelMgs(std::vector<Message>&);
    void SetChannelCreatorMri(std::string);

    std::string GetChannelSummary();

};