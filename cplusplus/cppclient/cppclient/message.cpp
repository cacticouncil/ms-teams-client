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

	//John/Olga channel
	std::string channelId = "19:0MaeOcpNpAX-HchAP2Z8xnw6j_QYsq6htWoAsD94QxY1@thread.tacv2";

	auto res = fetchTeams(chatSvcAggToken);
	writeResponseFile(res,"teams-info.txt");
	res = fetchChannelMessages(chatSvcAggToken, channelId, channelId, 5);
	writeResponseFile(res, "msg-info.txt");
	
	res = sendChannelMessage("channel message", skypeToken, channelId);
	writeResponseConsole(res);

	//last top-level message in channel
	std::string messageId = "1632939393328";
	res = sendReplyMessage("new test reply message", skypeToken, channelId, messageId);
	writeResponseConsole(res);

	//John id
	std::string senderUserId = "fdb3a4e9-675d-497e-acfe-4fd208f8ad89";
	//Olga id
	std::string receiverUserId = "7b30ff05-51b2-490a-b28b-2d8ac36cad8e";
	res = sendDirectMessage("testing DM...", skypeToken, senderUserId, receiverUserId);
	writeResponseConsole(res);

	return 0;
}

//test libcpr to ensure proper request/response function
void testConnectionAPI() {
	cpr::AsyncResponse fr = cpr::GetAsync(cpr::Url{ "http://www.httpbin.org/get" });
	cpr::Response r = fr.get();
	std::cout << r.text << std::endl;
}

//temp function to acquire credentials after login (ms-teams-credentials.local.json processed via Python to extract tokens from json [THIS IS VERY TEMPORARY])
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

//fetch list of teams for current user (w/ additional info) and return async response object
cpr::AsyncResponse fetchTeams(std::string& chatSvcAggToken) {
	cpr::AsyncResponse fr = cpr::GetAsync(
		cpr::Url{ "https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true" },
		cpr::Bearer{ chatSvcAggToken }
	);

	return fr;
}

//fetch list of messages in channel and return async response object
//pageSize determines max number of messages returned (I think)
cpr::AsyncResponse fetchChannelMessages(std::string& chatSvcAggToken, std::string& teamId, std::string& channelId, int pageSize) {
	cpr::AsyncResponse fr = cpr::GetAsync(
		cpr::Url{ "https://teams.microsoft.com/api/csa/api/v2/teams/" + teamId + "/channels/" + channelId },
		cpr::Parameters{ {"pageSize",std::to_string(pageSize)},{"filterSystemMessage",teamId==channelId ? "true" : ""}},
		cpr::Bearer{ chatSvcAggToken }
	);

	return fr;
}

//send message (destination determined by params string) and return async response object
cpr::AsyncResponse sendMessage(std::string text, std::string& skypeToken, std::string& params){
	cpr::AsyncResponse fr = cpr::PostAsync(
		cpr::Url{ "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/" + params + "/messages" },
		cpr::Payload{ {"content",text},{"messagetype","Text"},{"contenttype","text"},{"asmreferences","[]"},{"properties","{importance:'',subject:null}"} },
		cpr::Header{ {"Authentication","skypetoken=" + skypeToken} }
	);

	return fr;
}

//send message in channel
cpr::AsyncResponse sendChannelMessage(std::string text, std::string& skypeToken, std::string& channelId) {
	return sendMessage(text, skypeToken, channelId);
}

//reply to message in channel
//NOTE: existing JS reply API params incorrect -> requires ${receiverId};messageid=${senderId}
cpr::AsyncResponse sendReplyMessage(std::string text, std::string& skypeToken, std::string& channelId, std::string& messageId) {
	std::string params = channelId + ";" + "messageid=" + messageId;
	return sendMessage(text, skypeToken, params);
}

//send direct message to user
//NOTE: DM CHAT MUST ALREADY BE CREATED (receiver and user based on initial message)
cpr::AsyncResponse sendDirectMessage(std::string text, std::string& skypeToken, std::string& senderUserId, std::string& receiverUserId) {
	std::string params = "19:" + receiverUserId + "_" + senderUserId + "@unq.gbl.spaces";
	return sendMessage(text, skypeToken, params);
}

//resolve async request and write response text to file specified by filename param
//print code (and reason if failure) to console
void writeResponseFile(cpr::AsyncResponse& fr, std::string filename) {
	cpr::Response r = fr.get();
	std::cout << "Code: " << r.status_code << std::endl;
	if (r.status_code >= 400) {
		std::cout << r.reason << std::endl;
	}
	else if (r.status_code >= 200 && r.status_code < 300) {
		std::ofstream responseFile(filename);
		if (responseFile.is_open()) {
			responseFile << r.text;
			responseFile.close();
		}
	}
}

//resolve async request and write code and response to console
//write reason to console if failure
void writeResponseConsole(cpr::AsyncResponse& fr) {
	cpr::Response r = fr.get();
	std::cout << "Code: " << r.status_code << " Text: " << r.text << std::endl;
	if (r.status_code >= 400) {
		std::cout << r.reason << std::endl;
	}
}