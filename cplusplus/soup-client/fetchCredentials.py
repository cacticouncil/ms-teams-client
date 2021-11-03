import os
import json

jspath = input("Enter path to ms-teams-client directory: ")
os.system("cd " + jspath + " && npm run login && cp ms-teams-credentials.local.json ./cplusplus/soup-client/")

#export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
#sudo apt-get install libgtk-3.0
#export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
#export LIBGL_ALWAYS_INDIRECT=true

#npm install
#export LIBGL_ALWAYS_INDIRECT=1
#sudo apt-get install -y mesa-utils libgl1-mesa-glx