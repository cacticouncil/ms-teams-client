#include <cpr/cpr.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include "base64.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

int main(int argc, char** argv) {
	bool debug = 1;
	
	unordered_map<string, string> details;
	//cpr::Response r = cpr::Get(cpr::Url{ "https://teams.microsoft.com" }); //Basic example from libcpr
	/*cpr::Authentication{ "user", "pass" },
		cpr::Parameters{ {"anon", "true"}, {"key", "value"} });*/


	//Ideally we would get this url, we wouldn't hard code it. This is just temporay as we figure out SSO and 2FA.
	string url= "https://teams.microsoft.com/go#id_token=eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImtpZCI6Imwzc1EtNTBjQ0g0eEJWWkxIVEd3blNSNzY4MCJ9.eyJhdWQiOiI1ZTNjZTZjMC0yYjFmLTQyODUtOGQ0Yi03NWVlNzg3ODczNDYiLCJpc3MiOiJodHRwczovL2xvZ2luLm1pY3Jvc29mdG9ubGluZS5jb20vMGQ0ZGEwZjgtNGEzMS00ZDc2LWFjZTYtMGE2MjMzMWUxYjg0L3YyLjAiLCJpYXQiOjE2MzMyMDczNTQsIm5iZiI6MTYzMzIwNzM1NCwiZXhwIjoxNjMzMjExMjU0LCJhY2N0IjowLCJhaW8iOiJBVFFBeS84VEFBQUFGZjJpS2ZyS3V0bFovMWJSWnIvL2M1VkxEK05lMnljWmJXSFM3amYwbHdPSWJtbzN0dlJESnlKNjNkamhHVEU4IiwibmFtZSI6IkNhYnJlcmEgSGVybmFuZGV6LE9sZ2EgRCIsIm5vbmNlIjoiN2JmODE4YzAtYzFlMC00OGE0LWI4ZTYtZTQyZjcyM2NjZjFkIiwib2lkIjoiN2IzMGZmMDUtNTFiMi00OTBhLWIyOGItMmQ4YWMzNmNhZDhlIiwicHJlZmVycmVkX3VzZXJuYW1lIjoib2xnYS5jYWJyZXJhaGVybkB1ZmwuZWR1IiwicHVpZCI6IjEwMDNCRkZEQTg3MzgzRjIiLCJyaCI6IjAuQVRVQS1LQk5EVEZLZGsyczVncGlNeDRiaE1EbVBGNGZLNFZDalV0MTduaDRjMFkxQU8wLiIsInN1YiI6Im5lU1BSX0l6bzNreEdvenZ5N2FOMHd5cW1haTluNWFlWjB2YS1fbFR5dm8iLCJ0aWQiOiIwZDRkYTBmOC00YTMxLTRkNzYtYWNlNi0wYTYyMzMxZTFiODQiLCJ1cG4iOiJvbGdhLmNhYnJlcmFoZXJuQHVmbC5lZHUiLCJ1dGkiOiJONDJqWnQxdUFFLU1IV0hraGo4LUFBIiwidmVyIjoiMi4wIiwieG1zX21wY2kiOjI1OTIwMCwieG1zX3BjaSI6MzYwMH0.Cw1BlIqyzRY-5ufvjOto4At883IHB3BoqSr2wYH1ZYCfBKE05dUxl7a3C2q_98TxvSeWWsW2-33ZoBIyNUWiJcjiUAMjiRO_aSTIKCXPUbji_ZPS95BJuzf53cWvrH7sTGhtyjaofZ2bMaO2Q9x1Fp4u0okC_tYipF9qs5YYhXq7AZ37BeNOBFQDseujBaWAfONP-_K9lpYXs3MJQRFGNzfknUQXS0eqbqXuyeGs5YZyA7pW3AXPnQ7aHOo9MltvUqbVJ5IhyedyNc43wyKER8JN5Yyx1zy7afmemXmEU0-Qldkj46HSINzowr8XM9ix8sB95kXPSj0O2pOZPEZ-PQ&client_info=eyJ1aWQiOiI3YjMwZmYwNS01MWIyLTQ5MGEtYjI4Yi0yZDhhYzM2Y2FkOGUiLCJ1dGlkIjoiMGQ0ZGEwZjgtNGEzMS00ZDc2LWFjZTYtMGE2MjMzMWUxYjg0In0&state=eyJpZCI6IjgzYmUwZWIxLTY3MTMtNGViZC04MmJhLWU0OWExNmZlMTBlNCIsInRzIjoxNjMzMjA3NTk2LCJtZXRob2QiOiJyZWRpcmVjdEludGVyYWN0aW9uIn0%3d&session_state=15da4ea0-b62d-4f43-a746-34dd5fd58014";

	std::smatch match;
	regex expression("go#id_token=([^&]*)");

	if (regex_search(url, match, expression)) {
		//if something was found then work with the populated match container
		if (debug) { 
			cout << " Match[1]: " << match[1] << " \n Match[2]: " << match[2] << "\n Match prefix: " << match.prefix().str() << "\n Match suffix: " << match.suffix().str() << endl; 
		}

		details["idToken"] = match[1];
		
		stringstream s(match[1]);
		string encoded;
		getline(s, encoded, '.'); //skip the first piece
		getline(s, encoded, '.'); //need to decode and parse this section in order to get the oid
		
		string decoded = base64_decode(encoded, true); //see https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/

		if (debug) {
			cout << "\n Encoded Version: " << encoded << endl;
			cout << "\n Decoded Version: " << decoded << endl;
		}

		
		auto j = json::parse(decoded); //using external dependency for it
		
		for (json::iterator it = j.begin(); it != j.end(); it++) {
			if (it.key() == "oid") { 
				details["oid"] = it.value();
			}
		}
		
	}
	
}
