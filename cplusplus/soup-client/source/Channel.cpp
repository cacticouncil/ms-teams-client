#include "../include/Channel.h"

Channel::Channel(std::vector<Message*> channelMgs, std::string displayName, std::string id, std::string parentTeamId, std::string creatorMri, std::string groupId, bool isMember){
    this->channelMgs=channelMgs;
    this->displayName=displayName;
    this->id=id;
    this->parentTeamId= parentTeamId;  
    this->creatorMri=creatorMri;
    this->groupId=groupId;
    this->isMember= isMember;
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

std::vector<Message*>& Channel::GetChannelMgs(){
   return this->channelMgs;
}

std::string Channel::GetChannelCreatorMri(){
    return this->creatorMri;
}

std::string Channel::GetChannelGroupId(){
    return this->groupId;
}


bool Channel::GetIsChannelMember(){
    return this->isMember;
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

void Channel::SetChannelMgs(std::vector<Message*>& vect){
    this->channelMgs = vect;
}

void Channel::SetChannelCreatorMri(std::string mri){
    this->creatorMri=mri;
}

void Channel::SetChannelGroupId(std::string gId){
    this->groupId=gId;
}

void Channel::SetIsChannelMember(bool member){
    this->isMember=member;
}

std::string Channel::GetChannelSummary(){

    std::string result = "Channel Summary for " + this->GetChannelDisplayName() + ": \n";
    
    result += "id :" + id + "\n";
    result += "parentTeamId :" + parentTeamId + "\n";

    if (isMember){
        result += "creatorMri: " + creatorMri + "\n";
        result += "group id: " + groupId + "\n";

        result += "Content of Message List:\n";

        result += "msgId \t|\t content";

        for(Message *m : this->channelMgs){
            result +=  m->GetMsgId() + " | " + m->GetMsgContent() + "\n";
        }

    }
    else{
        result += "User is not a member of this channel\n";
    }

    result += "\n";

    return result;

}