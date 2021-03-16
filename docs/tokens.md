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

### Currently known uses

1. Sending any type of chat messages ([source](/teams/actions/send-message.js))
2. Retrieving polling endpoint ([source](/teams/actions/poll-endpoints.js))
3. Polling for events ([source](/teams/actions/poll.js))

## Chat Service Token

Known in code as `chatSvcAggToken` (we have yet to figure out what `Agg` is).
