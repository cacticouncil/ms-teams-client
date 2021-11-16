#include "../include/Channel.h"

Channel::Channel(std::vector<Message>&  channelMgs, std::string displayName, std::string id, std::string parentTeamId){
    this->channelMgs=channelMgs;
    this->displayName=displayName;
    this->id=id;
    this->parentTeamId= parentTeamId;  
}

//Accessors
std::string Channel::GetChannelDisplayName(){
    return displayName;
}

std::string Channel::GetChannelId(){
    return id;
}

std::string Channel::GetChannelTeamId(){
    return parentTeamId;
}

//Modifiers
void Channel::SetChannelDisplayName(std::string name){
    displayName= name;
}

void Channel::SetChannelId(std::string channId){
    id=channId;
}

void Channel::SetChannelTeamId(std::string teamId){
    parentTeamId= teamId;
}