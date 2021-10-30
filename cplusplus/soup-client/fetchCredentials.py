import os
import json

jspath = input("Enter path to ms-teams-client directory: ")
os.system("cd " + jspath + " && npm run login")
""" if jspath != "l":
    
else:
    jspath = "." """

""" cpppath = input("Enter absolute path to C++ soup-client Teams directory: ")
os.system("cp ms-teams-credentials.local.json " + cpppath + "/ms-teams-credentials.local.json")
 """
""" file = open(jspath + "/ms-teams-credentials.local.json","r")
data = json.load(file)
file.close()

cpppath = input("Enter path to c++ Teams client directory: ")
file = open(cpppath + "/ms-teams-credentials.local.txt","w")
file.write(data["authSkype"]["skypeToken"] + "\n" + data["chatSvcAggToken"]["token"])
file.close()

print("skypeToken:",data["authSkype"]["skypeToken"])
print("\nchatToken:",data["chatSvcAggToken"]["token"]) """

#export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
#sudo apt-get install libgtk-3.0
#export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
#export LIBGL_ALWAYS_INDIRECT=true