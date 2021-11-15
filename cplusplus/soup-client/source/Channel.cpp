#include "../include/Channel.h"

Channel::Channel(std::vector<Message>&  channelMgs, std::string displayName, std::string id, std::string parentTeamId){
    this->channelMgs=channelMgs;
    this->displayName=displayName;
    this->id=id;
    this->parentTeamId= parentTeamId;  
}
