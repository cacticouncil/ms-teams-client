#include "../include/Callbacks.h"

void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    displayResponseInfo( msg, false, "fetchUsersInfo.local.json"); //Need to write to the file at least once in order to be able to use the json parsing stuff 
    
    std::string credFilename = "fetchUsersInfo.local.json";
    JsonParser *parser = json_parser_new();
    GError *err;

    GPtrArray *data_arr = (GPtrArray*)user_data;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        //using JsonObject* to read the array from here rather than from a JsonNode* since array is a complex type
        JsonNode* root= json_parser_get_root(parser);
        JsonObject* rootObj= json_node_get_object(root); 
        JsonArray* arr= json_object_get_array_member(rootObj, "value");

        //JsonArrayForeach callB = (JsonArrayForeach)g_ptr_array_index(data_arr,0); //give it the index of the callback

        //std::map<std::string&, User&>* userMap = (std::map<std::string&, User&>*)g_ptr_array_index(data_arr,2); //getting the user map


        //std::map<std::string*, User*>* userMap = (std::map<std::string*, User*>*)g_ptr_array_index(data_arr,2);
        // auto iter = userMap->begin();
        //vector<users>

        // g_ptr_array_add(user_data,iter);
                

        json_array_foreach_element(arr, jsonArrayGetUsers, user_data);  //jsonArrayGetUsers
    }
     else{
        g_print ("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
    }

    


   GMainLoop *loop = (GMainLoop*)g_ptr_array_index(data_arr,1); //give it the index of the loop

   g_main_loop_quit(loop);
}


//Function that gets executed for each element of the Json Array
void jsonArrayGetUsers (  JsonArray* array,  guint index_,  JsonNode* element_node,  gpointer user_data) {  
    //std::cout<<"Currently in iteration " + std::to_string((int)index_); //Debug Statement
    GPtrArray *data_arr = (GPtrArray*)user_data;
    int i = (int) index_;
    //std::map<std::string*, User*>* userMap = (std::map<std::string, User*>*)g_ptr_array_index(data_arr,2); //getting the user map

    std::vector<User*>* userVect = (std::vector<User*>*)g_ptr_array_index(data_arr,0);
    

    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected


    JsonNode* userInfo =json_object_get_member(currObj, "objectId");
    std::string objectId = json_node_get_string(userInfo);


    userInfo =json_object_get_member(currObj, "displayName"); //member name here
    std::string displayName = json_node_get_string(userInfo);

    userInfo =json_object_get_member(currObj, "email");
    std::string email = json_node_get_string(userInfo);

    userInfo =json_object_get_member(currObj, "mri");
    std::string mri = json_node_get_string(userInfo);

    // userVect->at(i)->SetUserOid(objectId);
    userVect->at(i)->SetUserDisplayName(displayName);
    userVect->at(i)->SetUserEmail(email);
    userVect->at(i)->SetUserMri(mri);

    
    // std::cout<< "\nThis is the principal name: " + displayName + "\n";

    // std::cout<< "Vect Size: " << userVect->size()<<std::endl;


    // std::cout<< "User object version: " + userVect->at(i)->GetUserDisplayName() + " \n\n";
/*
    std::string email;
    std::string displayName;
    std::string objectId;
    std::string mri;



            "userPrincipalName": "jhook1@ufl.edu",
            "givenName": "John",
            "surname": "Hook",
            "email": "jhook1@ufl.edu",
            "userType": "Member",
            "isShortProfile": true,
            "displayName": "Hook,John W",
            "type": "ADUser",
            "mri": "8:orgid:fdb3a4e9-675d-497e-acfe-4fd208f8ad89",
            "objectId": "fdb3a4e9-675d-497e-acfe-4fd208f8ad89"
*/

}
