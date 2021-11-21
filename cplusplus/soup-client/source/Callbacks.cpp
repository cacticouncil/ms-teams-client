#include "../include/Callbacks.h"

void fetchUsersInfoCallback(SoupSession *session, SoupMessage *msg, gpointer user_data){

    //Need to write to the file at least once in order to be able to use the json parsing stuff 
    //Alternatively parse from data rather than the file
    //Pass true vs false to printo to console as well
    displayResponseInfo( msg, false, "fetchUsersInfo.local.json"); 
    
    std::string credFilename = "fetchUsersInfo.local.json";
    JsonParser *parser = json_parser_new();
    GError *err;

    GPtrArray *data_arr = (GPtrArray*)user_data;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        //using JsonObject* to read the array from here rather than from a JsonNode* since array is a complex type
        JsonNode* root= json_parser_get_root(parser);
        JsonObject* rootObj= json_node_get_object(root); 
        JsonArray* arr= json_object_get_array_member(rootObj, "value");

        json_array_foreach_element(arr, jsonArrayGetUsers, user_data);
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

    GPtrArray *data_arr = (GPtrArray*)user_data;
    int i = (int) index_;
    //std::map<std::string*, User*>* userMap = (std::map<std::string, User*>*)g_ptr_array_index(data_arr,2); //getting the user map

    std::vector<User*>* userVect = (std::vector<User*>*)g_ptr_array_index(data_arr,0);
    

    JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected


    JsonNode* userInfo =json_object_get_member(currObj, "objectId");
    std::string objectId = json_node_get_string(userInfo);


    userInfo =json_object_get_member(currObj, "displayName");
    std::string displayName = json_node_get_string(userInfo);

    userInfo =json_object_get_member(currObj, "email");
    std::string email = json_node_get_string(userInfo);

    userInfo =json_object_get_member(currObj, "mri");
    std::string mri = json_node_get_string(userInfo);

    // userVect->at(i)->SetUserOid(objectId); //don't need thi ssince oid is passed through the User object
    userVect->at(i)->SetUserDisplayName(displayName);
    userVect->at(i)->SetUserEmail(email);
    userVect->at(i)->SetUserMri(mri);

    
    // std::cout<< "User object version: " + userVect->at(i)->GetUserDisplayName() + " \n\n";

}
