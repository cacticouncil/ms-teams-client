#pragma once
#include <string>
#include <vector>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>
#include <fstream>
#include <iostream>
#include "../include/Channel.h"
#include "../include/fetch.h"


class Team{
    
    std::string displayName;
    std::string id;
    std::string totalMemberCount;
    std::string creatorMri;
    std::string groupId;
    std::vector<Channel> channelList; //change to a map of id to Channel
    
    
    public:

    Team(){}
    
    //Accessors
    std::string GetTeamDisplayName();
    std::string GetTeamId();
    std::string GetTotalMemberCount();
    std::string GetCreatorMri();
    std::string GetTeamGroupId();

    //Modifiers
    void SetTeamDisplayName(std::string);
    void SetTeamId(std::string);
    void SetTotalMemberCount(std::string);
    void SetCreatorMri(std::string);
    void SetTeamGroupId(std::string);


    //void FetchUsersTest(SoupSession *session, std::string &chatSvcAggToken, GMainLoop* loop, std::vector<std::string>& userIds, SoupSessionCallback callback, JsonArrayForeach jArrCallback);
   
    //{
    //     JsonObject* currObj =json_array_get_object_element(array, index_);  //current array object being disected
    //     JsonNode* userInfo =json_object_get_member(currObj, "displayName"); //member name here
    //     std::string userInfoStr = json_node_get_string(userInfo);
    //     std::cout<< "\nThis is the principal name: " + userInfoStr + "\n\n";
    // }

};