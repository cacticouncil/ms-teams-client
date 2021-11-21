#include "../include/Team.h"

Team::Team(std::string displayName, std::string id, std::string totalMemberCount, std::string creatorMri, std::string groupId){
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

std::string Team::GetTotalMemberCount(){
    return totalMemberCount;
}

std::string Team::GetCreatorMri(){
    return creatorMri;
}

std::string Team::GetTeamGroupId(){
    return groupId;
}

std::vector<Channel*>& Team::GetChannelList(){
    return this->channelList;
}

//Modifiers
void Team::SetTeamDisplayName(std::string name){
    displayName = name;
}

void Team::SetTeamId(std::string teamId){
    id=teamId;
}

void Team::SetTotalMemberCount(std::string total){
    totalMemberCount=total;
}

void Team::SetCreatorMri(std::string mri){
    creatorMri=mri;
}

void Team::SetTeamGroupId(std::string group){
    groupId=group;
}

void Team::SetChannelList(std::vector<Channel*>& channels){
    this->channelList=channels;
}
