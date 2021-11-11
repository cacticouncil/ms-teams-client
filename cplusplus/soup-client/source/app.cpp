#include <string>
#include <iostream>
#include <ctime>

#include <libsoup/soup.h>

#include <json-glib/json-glib.h>

#include "../include/test.h"
#include "../include/polling.h"
#include "../include/messaging.h"
#include "../include/fetch.h"
#include "../include/admin.h"
#include "../include/app.h"

int runConsoleApp(){
    std::cout << "Welcome to MS-Teams!\n";
    std::cout << "[NOTE] If running this app via WSL on Windows, make sure that you are running an active XServer to login!\n";
    std::cout << "\nSign In: [ENTER]\n";
    std::cout << "Quit: [q]\n";

    std::cout << "\nInput: ";
    std::string input;
    std::getline(std::cin,input);

    std::string skypeToken;
	std::string chatSvcAggToken;
    std::string skypeSpacesToken;
    std::string currUserId;
    if(input == "q"){
        std::cout << "Goodbye!\n";
        return 0;
    }
    else{
        bool status = checkCredentialsValid();
        if(!status) system("./trigger-login.sh");
        status = readCredentialsOnly(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);
        if(!status) return 1;
        std::cout << "Signed in as " << currUserId << "\n";
    }

    return 0;
}

bool checkCredentialsValid(){
    std::string credFilename = "../../ms-teams-credentials.local.json"; 

    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        JsonReader *reader = json_reader_new(json_parser_get_root(parser));

        json_reader_read_member(reader,"skypeSpacesToken");
        json_reader_read_member(reader,"expiration");
        int expirationTime = json_reader_get_int_value(reader);
        int currTime = time(0);
        int timeRemaining = currTime - expirationTime;

        g_object_unref(reader);
        g_object_unref(parser);
        return timeRemaining > 0;
    }
    else{
        g_printerr("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
        return false;
    }
}

bool readCredentialsOnly(std::string &skypeToken,std::string &chatSvcAggToken,std::string &skypeSpacesToken, std::string &currUserId){
    std::string credFilename = "../../ms-teams-credentials.local.json"; 

    JsonParser *parser = json_parser_new();
    GError *err = NULL;

    if(json_parser_load_from_file(parser,credFilename.c_str(),&err)){
        JsonReader *reader = json_reader_new(json_parser_get_root(parser));

        json_reader_read_member(reader,"authSkype");
        json_reader_read_member(reader,"skypeToken");
        skypeToken = json_reader_get_string_value(reader);

        json_reader_set_root(reader,json_parser_get_root(parser));
        json_reader_read_member(reader,"chatSvcAggToken");
        json_reader_read_member(reader,"token");
        chatSvcAggToken = json_reader_get_string_value(reader);

        json_reader_set_root(reader,json_parser_get_root(parser));
        json_reader_read_member(reader,"skypeSpacesToken");
        json_reader_read_member(reader,"token");
        skypeSpacesToken = json_reader_get_string_value(reader);

        json_reader_set_root(reader,json_parser_get_root(parser));
        json_reader_read_member(reader,"oid");
        currUserId = json_reader_get_string_value(reader);

        g_object_unref(reader);
        g_object_unref(parser);
        return true;
    }
    else{
        g_printerr("Unable to parse '%s': %s\n", credFilename.c_str(), err->message);
        g_error_free (err);
        g_object_unref (parser);
        return false;
    }
}