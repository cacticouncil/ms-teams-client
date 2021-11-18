#include "../include/Team.h"


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



// void Team::FetchUsersTest(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, std::vector<std::string>& userIds, SoupSessionCallback fetch,  JsonArrayForeach jArrCallback){

//     fetchUsersInfo(session,chatSvcAggToken, loop, userIds, fetch , jArrCallback); //GetUsersInfoCallback pass as the callback

// }

   

//API Fucntions



