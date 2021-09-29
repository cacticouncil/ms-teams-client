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

	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

	fetchTeams(chatSvcAggToken);
	fetchChannelMessages(chatSvcAggToken, channelId, channelId, 5);
	
	//sendChannelMessage("changing last message user...", skypeToken, channelId);

	std::string messageId = "1632939393328";
	sendReplyMessage("testing reply message...", skypeToken, channelId, messageId);

	//John
	std::string senderUserId = "fdb3a4e9-675d-497e-acfe-4fd208f8ad89";
	//Olga
	std::string receiverUserId = "7b30ff05-51b2-490a-b28b-2d8ac36cad8e";
	//sendDirectMessage("testing DM...", skypeToken, senderUserId, receiverUserId);

	return 0;
}

//test libcpr to ensure proper request/response function
void testConnectionAPI() {
	cpr::AsyncResponse fr = cpr::GetAsync(cpr::Url{ "http://www.httpbin.org/get" });
	cpr::Response r = fr.get();
	std::cout << r.text << std::endl;
}

//temp function to acquire credentials after login (ms-teams-credentials.local.json processed via Python to extract tokens from json)
//reads credentials from local text file and save to string buffers (FILE FORMAT [.TXT]: skypeToken\nchatSvcToken)
//if 4xx error when querying API endpoints, user not authenticated and must fetch new credentials from login
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

//fetch list of messages in channel and print code to stdout
//pageSize determines max number of messages returned (I think)
//if success code, write response text to file, else write reason to stdout
void fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize) {
	cpr::AsyncResponse fr = cpr::GetAsync(
		cpr::Url{ "https://teams.microsoft.com/api/csa/api/v2/teams/" + teamId + "/channels/" + channelId },
		cpr::Parameters{ {"pageSize",std::to_string(pageSize)},{"filterSystemMessage",teamId==channelId ? "true" : ""}},
		cpr::Bearer{ chatSvcAggToken }
	);

	cpr::Response r = fr.get();
	std::cout << "Code: " << r.status_code << std::endl;

	if (r.status_code >= 400) {
		std::cout << r.reason << std::endl;
	}
	else if (r.status_code >= 200 && r.status_code < 300) {
		std::ofstream msgFile("message-info.txt");
		if (msgFile.is_open()) {
			msgFile << r.text;

			msgFile.close();
		}
	}
}

//send message (destination determined by params string [printed to stdout])
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
//NOTE: existing JS reply API params incorrect -> requires ${receiverId};messageid=${senderId}
void sendReplyMessage(std::string text, std::string& skypeToken, std::string& channelId, std::string& messageId) {
	std::string params = channelId + ";" + "messageid=" + messageId;
	sendMessage(text, skypeToken, params);
}

//send direct message to user
//NOTE: DM CHAT MUST ALREADY BE CREATED (receiver and user based on initial message)
void sendDirectMessage(std::string text, std::string& skypeToken, std::string& senderUserId, std::string& receiverUserId) {
	std::string params = "19:" + receiverUserId + "_" + senderUserId + "@unq.gbl.spaces";
	sendMessage(text, skypeToken, params);
}