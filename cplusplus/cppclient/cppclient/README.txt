main: executes API functions with static values for testing
testConnectionAPI: synchronous function which makes async get request (testing async library) to test libcpr library installation
readCredentials: reads auth tokens from reformatted token file into string buffers for use by API functions
fetchTeams: fetches list of Teams (w/ additional info) for current user (asyncronous - returns AsyncResponse object)
fetchChannelMessages: fetches list of messages for specified channel (asyncronous - returns AsyncResponse object)
sendMessage: generalized function for sending a message (asyncronous - returns AsyncResponse object)
- sendChannelMessage: send a message in a given channel
- sendReplyMessage: send a message in reply to a specified message
- sendDirectMessage: send a direct message to another user (DM CHAT MUST ALREADY BE CREATED)
writeResponseFile: resolves AsyncResponse and writes text to specified file
writeResponseConsole: resolves AsyncResponse and writes text to console

fetchCredentials: python fetchCredentials.py
