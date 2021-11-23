#pragma once
#include <string>
#include <vector>
#include <sstream>

class Message{

  std::string content;
  std::string id;
  std::string parentMessageId;
  int sequenceId;
  std::string fromMri; //Mri of user who sent message
  std::string arrivalTime;
  std::string containerChannelId; //the messages and the channel can be related based on the containerChannelId propety

  public:
    
    Message (std::string content ="", std::string id="", std::string parentMessageId="", int sequenceId=0, std::string fromMri="", std::string arrivalTime="", std::string containerChannelId="");
    
    //Accessors
    std::string GetMsgContent();
    std::string GetMsgId();
    std::string GetMsgParentId();
    int GetMsgSequenceId();
    std::string GetSenderMri(); 
    std::string GetArrivalTime();
    std::string GetMsgContainerChannelId();
    
    //Modifiers
    void SetMsgContent(std::string);
    void SetMsgId(std::string);
    void SetMsgParentId(std::string);
    void SetMsgSequenceId(int);
    void SetSenderMri(std::string); 
    void SetArrivalTime(std::string);
    void SetMsgContainerChannelId(std::string);

    //others
    bool IsTopLevelMsg();
    std::string GetSenderOid();
    std::string GetMessageSummary();


};

//Decided against the Reply Chain class since it made things more complicated for now
//Leaving it here for future reference
// class ReplyChain{
//     public:
//     std::vector<Message> messages;

//     ReplyChain(){}

//     ReplyChain(std::vector<Message>& v){
//         messages = v;
//     }
// };