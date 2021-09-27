#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include <cpr/cpr.h>

#include "message.h"

int main() {
	std::string skypeToken;
	std::string chatSvcAggToken;

	readCredentials(skypeToken, chatSvcAggToken);

	fetchTeams(chatSvcAggToken);

	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";
	//sendChannelMessage("another general test, next up replies", skypeToken, channelId);

	std::string messageId = "1632772793231";
	sendReplyMessage("testing reply message...", skypeToken, channelId, messageId);

	return 0;
}

//test libcpr to ensure proper request/response function
void testConnectionAPI() {
	cpr::AsyncResponse fr = cpr::GetAsync(cpr::Url{ "http://www.httpbin.org/get" });
	cpr::Response r = fr.get();
	std::cout << r.text << std::endl;
}

//read credentials from local text file and save to string buffer
//if 4xx error for user not authenticated when querying API endpoints, fetch new credentials from login
//temp function to read credentials after login (ms-teams-credentials.local.json processed via Python to extract tokens from json)
//FILE FORMAT (TXT): skypeToken\nchatSvcAggToken
void readCredentials(std::string& skypeToken,std::string& chatSvcAggToken) {
	std::ifstream credFile("ms-teams-credentials.local.txt");
	if (credFile.is_open()) {
		getline(credFile, skypeToken);
		getline(credFile, chatSvcAggToken);

		credFile.close();
	}
}

//fetch list of teams for current user (w/ additional info) and print code to stdout
//if success code, write response text to file, else write reason to stdout
void fetchTeams(std::string& chatSvcAggToken) {
	cpr::AsyncResponse fr = cpr::GetAsync(
		cpr::Url{ "https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true" },
		cpr::Bearer{ chatSvcAggToken }
	);

	cpr::Response r = fr.get();
	std::cout << "Code: " << r.status_code << std::endl;

	if (r.status_code >= 400) {
		std::cout << r.reason << std::endl;
	}
	else if (r.status_code >= 200 && r.status_code < 300) {
		std::ofstream teamFile("teams-info.txt");
		if (teamFile.is_open()) {
			teamFile << r.text;
			
			teamFile.close();
		}
	}
}

//send message (destination determined by params string)
//print response and status code (and reason if 4xx error) to stdout
void sendMessage(std::string text, std::string& skypeToken, std::string& params){
	std::cout << "Params: " << params << std::endl;

	cpr::AsyncResponse fr = cpr::PostAsync(
		cpr::Url{ "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages" },
		cpr::Payload{ {"content",text},{"messagetype","Text"},{"contenttype","text"},{"asmreferences","[]"},{"properties","{importance:'',subject:null}"} },
		cpr::Header{ {"Authentication","skypetoken=" + skypeToken} }
	);

	cpr::Response r = fr.get();
	std::cout << "Code: " << r.status_code << " Text: " << r.text << std::endl;
	if (r.status_code >= 400) {
		std::cout << r.reason << std::endl;
	}
}

//send message in channel
void sendChannelMessage(std::string text, std::string& skypeToken, std::string& channelId) {
	sendMessage(text, skypeToken, channelId);
}

//reply to message in channel
void sendReplyMessage(std::string text, std::string& skypeToken, std::string& channelId, std::string& messageId) {
	std::string params = channelId + ";" + messageId;
	sendMessage(text, skypeToken, params);
}