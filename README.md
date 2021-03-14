# MS Teams Electron

This is a tool to help reverse engineer the Microsoft Teams web client.

## Usage

This is a very manual process

### Step 1

Run the electron app to kick start login process.

```sh
npm run login
```

This will navigate to the Microsoft Teams web app. Once the appropriate tokens have been detected, the app will close automatically.

Upon exit, a file should be generated: `ms-teams-credentials.local.json`. This contains the local storage of the web app.

### Step 2

Build, if needed, then run an action file.

```sh
# Build for first use or source code changes
npm run build

# Ex: List teams
node dist/private-api/list-teams.js
```
