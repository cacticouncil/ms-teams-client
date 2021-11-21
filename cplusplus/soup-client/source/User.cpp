#include "../include/User.h"


User::User(std::string email,  std::string display, std::string objectId, std::string mri){
    this->email =email;
    this->displayName = displayName;
    this->objectId = objectId;
    this->mri = mri;
}

//Accessors
std::string User::GetUserEmail(){
    return email;
}

std::string User::GetUserDisplayName(){
    return displayName;
}

std::string User::GetUserOid(){
    return objectId;
}

std::string User::GetUserMri(){
    return mri;
}

//Modifiers
void User::SetUserEmail(std::string emailParam){
    email = emailParam;
}

void User::SetUserDisplayName(std::string name){
    displayName = name;
}

void User::SetUserOid(std::string oid){
    objectId = oid;
}

void User::SetUserMri(std::string mriParam){
    mri=mriParam;
}

std::string User::GetUserSummary(){
    std::string result= "User Summary for " + this->GetUserDisplayName() + ":\n";
    result+= "email: " + this->GetUserEmail() + "\nmri: " + this->GetUserMri() + "\noid: " + this->GetUserOid() + "\n";

    return result;
}