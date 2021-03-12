# Teams API Documentation

> You can use https://tableconvert.com/ if you want to add more information to the tables. Just import the table by copy/pasting it in via the `Import` button. I feel sorry if you tried formatting this by hand.

> Each table is sorted by HTTP protocol (**GET**, **POST**, **PUT**, **DELETE**, **PATCH**).

## <u>GET</u>
| **Host**                    | **Endpoint**                                            | **Description**                                | **Query Parameters**                               | **Authorization**              |
| --------------------------- | ------------------------------------------------------- | ---------------------------------------------- | -------------------------------------------------- | ------------------------------ |
| https://teams.microsoft.com | `/api/csa/api/v2/teams/${teamId}/channels/${channelId}` | This is used to fetch channel messages.        | `filterSystemMessage=true`, `pageSize={number}`    | **Bearer** `{chatSvcAggToken}` |
| https://teams.microsoft.com | `/api/csa/api/v1/teams/users/me`                        | Fetch all available "teams" for a single user. | `isPrefetch=false`, `enableMembershipSummary=true` | **Bearer** `{chatSvcAggToken}` |