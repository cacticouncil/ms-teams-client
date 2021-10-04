import os
import json

jspath = input("Enter path to ms-teams-client directory: ")
os.system("cd " + jspath + " && npm run login")

file = open(jspath + "/ms-teams-credentials.local.json","r")
data = json.load(file)
file.close()

cpppath = input("Enter path to c++ Teams client directory: ")
file = open(cpppath + "/ms-teams-credentials.local.txt","w")
file.write(data["authSkype"]["skypeToken"] + "\n" + data["chatSvcAggToken"]["token"])
file.close()

print("skypeToken:",data["authSkype"]["skypeToken"])
print("chatToken:",data["chatSvcAggToken"]["token"])