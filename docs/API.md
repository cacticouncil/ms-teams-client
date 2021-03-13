# Teams API Documentation

> Each table is sorted by HTTP protocol ([**GET**](#get)</a>, [**POST**](#post), **PUT**, **DELETE**, **PATCH**).

## <u>GET</u> {#get}
| **Host**                    | **Endpoint**                                            | **Description**                                | **Query Parameters**                               | **Authorization**              |
| --------------------------- | ------------------------------------------------------- | ---------------------------------------------- | -------------------------------------------------- | ------------------------------ |
| https://teams.microsoft.com | `/api/csa/api/v2/teams/${teamId}/channels/${channelId}` | This is used to fetch channel messages.        | `filterSystemMessage=true`, `pageSize={number}`    | **Bearer** `{chatSvcAggToken}` |
| https://teams.microsoft.com | `/api/csa/api/v1/teams/users/me`                        | Fetch all available "teams" for a single user. | `isPrefetch=false`, `enableMembershipSummary=true` | **Bearer** `{chatSvcAggToken}` |
<br><br/>
## <u>POST</u> {#post}
| **Host**                                | **Endpoint**                                                                                                | **Description**                                                                                                                             | Authentication                                   |
| --------------------------------------- | ----------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------ |
| https://amer.ng.msg.teams.microsoft.com | `/v1/users/ME/conversations/${sampleChannelId}/messages`                                                    | This is used to send a normal message in a target channel.                                                                                  | `skypeToken=${credentials.authSkype.skypeToken}` |
| https://amer.ng.msg.teams.microsoft.com | `/v1/users/ME/conversations/${sampleChannelId};${messageId}/messages`                                       | This is used to reply to a message in a target channel.                                                                                     | `skypeToken=${credentials.authSkype.skypeToken}` |
| https://amer.ng.msg.teams.microsoft.com | `/v1/users/ME/conversations/${sampleChannelId}:${primaryUserId}_${secondaryUserId}@unq.gbl.spaces/messages` | This is used to send a DM message to a target user. `primaryUserId` and `secondaryUserId` are the two parties involved in direct messaging. | `skypeToken=${credentials.authSkype.skypeToken}` |
<br><br/>
> `Content-type: application/json`
## Messaging Payload
| **Key**       | **Value**                           |
| ------------- | ----------------------------------- |
| content       | `"your message"`                    |
| messagetype   | `"Text"`                            |
| contenttype   | `"text"`                            |
| amsreferences | `[]`                                |
| properties    | `{ importance: "", subject: null }` |