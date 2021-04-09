# MS Teams Client

This library helps interact with Microsoft Teams via undocumented APIs.

## Installation

This is not published on npm or any other package registry. Therefore, you must clone this repository to your computer and installing using a file path instead of a package name.

```sh
# Step 1 - clone the repository
git clone https://github.com/cacticouncil/ms-teams-client.git

# Step 2 - navigate to your project
cd my_project

# Step 3 - install into project
npm install /path/to/ms-teams-client
```

## Local development

This library does not support retrieving the required tokens for use. However, for local development, you can use the [dev-login.js](/tests/dev-login.js) script to retrieve the credentials via a small Electron app. This will output credentials to the file `ms-teams-credentials.local.json`, which you can load into tests for real API calls.

```sh
# This will launch the dev-login.js script
npm run login
```
