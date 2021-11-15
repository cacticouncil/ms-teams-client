#pragma once
#include <string>

class User{

    public:
    //eventually thes fields will be private, rn public temporarily
    std::string email;
    std::string displayName;
    std::string objectId;
    std::string mri;

    //mri is just 8:orig:objectId, worth saving it since the mri matches the "from field in the message class"

    User(std::string email = "",  std::string displayName = "", std::string objectId = "", std::string mri = "");
    //Accessors and Modifiers Here for when the fields are made private (very soon)

    
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