#include <string>
#include <iostream>

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
    std::cout << "Exit: [q]\n";

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
        std::cout << "Starting login...\n";
        bool status = readCredentials(skypeToken, chatSvcAggToken, skypeSpacesToken, currUserId);
        if(!status) return 1;
        std::cout << "Signed in as " << currUserId << "\n";
    }

    

    return 0;
}