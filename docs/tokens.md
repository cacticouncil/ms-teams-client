# Tokens

Throught this reverse engineering effort, we have found multiple tokens in use by the Microsoft Teams webapp. However, there are only a few that are required for basic operation.

## Skype Token

This token is provided in a JWT format.

### Example payload

```json
{
  "iat": 1615856732,
  "exp": 1615943131,
  "skypeid": "<uuid>",
  "scp": 780,
  "csi": "1615856431",
  "tid": "<uuid>",
  "rgn": "amer"
}
```

Typical usage of this token is through the `Authentication` header, as shown below:

```js
Authentication: `skypetoken=${token}`
```

> Please note that the header key is `Authentication`, not the more commonly used header `Authorization`

### Currently known uses

1. Sending any type of chat messages ([source](/teams/actions/send-message.js))
2. Retrieving polling endpoint ([source](/teams/actions/poll-endpoints.js))
3. Polling for events ([source](/teams/actions/poll.js))

## Chat Service Token

Known in code as `chatSvcAggToken` (we have yet to figure out what `Agg` stands for). This token is also in a JWT format.

### Example Payload

```json
{
  "aud": "https://chatsvcagg.teams.microsoft.com",
  "iss": "https://sts.windows.net/<uuid>/",
  "iat": 1616026315,
  "nbf": 1616026315,
  "exp": 1616030215,
  "amr": ["pwd"],
  /* ... */
  "family_name": "Last name",
  "given_name": "First name",
  "ipaddr": "aa.bb.cc.dd",
  "name": "Last name, First name",
  "oid": "<uuid>",
  "scp": "user_impersonation",
  "tenant_region_scope": "NA",
  "tid": "<uuid>",
  "unique_name": "email@domain.com",
  "upn": "email@domain.com",
  "ver": "1.0"
}
```

Typical usage of this token is through the `Authorization` header, as shown below:

```js
Authorization: `Bearer ${token}`
```

### Currently known uses

1. Retrieving list of teams in account ([source](/teams/actions/list-teams.js))
2. Retrieving messages from a channel ([source](/teams/actions/get-channel-messages.js))
