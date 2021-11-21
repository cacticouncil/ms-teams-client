#pragma once
#include <string>

class User{
    
    std::string email;
    std::string displayName;
    std::string objectId;
    std::string mri;     //mri is just 8:orig:objectId, worth saving it since the mri matches the "from field in the message class"

    public:
    User(std::string email ,  std::string displayName , std::string objectId , std::string mri );

    User(){
        email = "";
        displayName = "";
        objectId = "";
        mri = "";
    }

    //Accessors
    std::string GetUserEmail();
    std::string GetUserDisplayName();
    std::string GetUserOid();
    std::string GetUserMri();
    
    //Modifiers
    void SetUserEmail(std::string);
    void SetUserDisplayName(std::string);
    void SetUserOid(std::string);
    void SetUserMri(std::string);


    std::string GetUserSummary();
    
};