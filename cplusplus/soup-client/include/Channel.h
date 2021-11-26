#pragma once
#include <string>
#include <vector>
#include <map>
#include "../include/Message.h"

class Channel{
   
    //ReplyChain channelMsgs;
    std::vector<Message*> channelMgs; 
    std::string displayName;
    std::string id;
    std::string parentTeamId;
    std::string creatorMri;
    std::string groupId;
    bool isMember;
    bool messagesRetrieved;

    //Team* to team it belongs in
    public:
    
    Channel () {
        this->displayName="";
        this->id="";
        this->parentTeamId="" ;
        this->creatorMri = "";  
        this->groupId="";
        this->isMember= false;
        this->messagesRetrieved = false;
    }

    Channel(std::vector<Message*> channelMgs, std::string displayName, std::string ,  std::string parentTeamId, std::string creatorMri, std::string groupId,  bool isMember);

    ~Channel();
    
    //Accessors
    std::string GetChannelDisplayName();
    std::string GetChannelId();
    std::string GetChannelTeamId();
    std::vector<Message*>& GetChannelMgs();
    std::string GetChannelCreatorMri();
    std::string GetChannelGroupId();
    bool GetIsChannelMember();
    bool GetMessagesRetrievedStatus();

    //Modifiers
    void SetChannelDisplayName(std::string);
    void SetChannelId(std::string);
    void SetChannelTeamId(std::string);
    void SetChannelMgs(std::vector<Message*>&);
    void SetChannelCreatorMri(std::string);
    void SetChannelGroupId(std::string);
    void SetIsChannelMember(bool);
    void SetMessagesRetrievedStatus(bool);

    std::string GetChannelSummary();

};