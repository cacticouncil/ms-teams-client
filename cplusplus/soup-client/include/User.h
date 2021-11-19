#pragma once
#include <string>

class User{
    
    std::string email;
    std::string displayName;
    std::string objectId;
    std::string mri;

    public:
    //eventually thes fields will be private, rn public temporarily

    //mri is just 8:orig:objectId, worth saving it since the mri matches the "from field in the message class"

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

   
    
    //These are all future potential properties

    // "userPrincipalName": "olga.cabrerahern@ufl.edu",
    // "givenName": "Olga",
    // "surname": "Cabrera Hernandez",
    // "email": "olga.cabrerahern@ufl.edu",
    // "userType": "Member",
    // "isShortProfile": true,
    // "displayName": "Cabrera Hernandez,Olga D",
    // "type": "ADUser",
    // "mri": "8:orgid:7b30ff05-51b2-490a-b28b-2d8ac36cad8e",
    // "objectId": "7b30ff05-51b2-490a-b28b-2d8ac36cad8e"
                            

};