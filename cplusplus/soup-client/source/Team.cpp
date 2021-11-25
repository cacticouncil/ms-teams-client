#include "../include/Team.h"

Team::Team(std::string displayName, std::string id, int totalMemberCount, std::string creatorMri, std::string groupId, std::vector<Channel> channels){
    this->displayName = displayName;
    this->id = id;
    this->totalMemberCount = totalMemberCount;
    this->creatorMri = creatorMri;
    this->groupId = groupId;
}

//Accessors
std::string Team::GetTeamDisplayName(){
    return displayName;
}

std::string Team::GetTeamId(){
    return id;
}

int Team::GetTotalMemberCount(){
    return totalMemberCount;
}

std::string Team::GetCreatorMri(){
    return creatorMri;
}

std::string Team::GetTeamGroupId(){
    return groupId;
}

std::vector<Channel>& Team::GetChannelList(){
    return this->channelList;
}

//Modifiers
void Team::SetTeamDisplayName(std::string name){
    displayName = name;
}

void Team::SetTeamId(std::string teamId){
    id=teamId;
}

void Team::SetTotalMemberCount(int total){
    totalMemberCount=total;
}

void Team::SetCreatorMri(std::string mri){
    creatorMri=mri;
}

void Team::SetTeamGroupId(std::string group){
    groupId=group;
}

void Team::SetChannelList(std::vector<Channel>& channels){
    this->channelList=channels;
}

std::string Team::GetTeamSummary(){

    std::string result = "Team Summary for " + this->GetTeamDisplayName() + ": \n";

    result += "team id: " + this->id + "\n";
    result += "total member count (including bots): " + std::to_string(this->totalMemberCount) + "\n";
    result += "creator mri: " + this->creatorMri + "\n";
    result += "group id:" + this->groupId + "\n";

    result += "Content of Channel List:\n";

    //result += "isMember \t|\t channel id \t|\t channel name";

    for(Channel c : this->channelList){
       result += c.GetChannelSummary();// c.GetChannelId() + " | " + c.GetChannelDisplayName() + "\n";
       result += "\n";
    }

    result += "\n";

    return result;
}