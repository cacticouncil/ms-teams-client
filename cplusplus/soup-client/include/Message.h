#pragma once
#include <string>
#include <vector>
#include <sstream>
//#include "../include/Channel.h"

class Message{

    std::string content;
    std::string id;
    std::string parentMessageId;
    std::string sequenceId;
    std::string fromMri; //Mri of user who sent message
    std::string arrivalTime;
    std::string containerChannelId;

  //  Channel* channel; //pointer to the channel this message belongs to

    //TO DO:
    //vector of Message pointers to the reply messages

    public:
    
    Message (std::string content ="", std::string id="", std::string parentMessageId="", std::string sequenceId="", std::string fromMri="", std::string arrivalTime="", std::string containerChannelId="");
    
    //Accessors
    std::string GetMsgContent();
    std::string GetMsgId();
    std::string GetMsgParentId();
    std::string GetMsgSequenceId();
    std::string GetSenderMri(); 
    std::string GetArrivalTime();
    std::string GetMsgContainerChannelId();
    //Channel* GetChannel();
    
    //Modifiers
    void SetMsgContent(std::string);
    void SetMsgId(std::string);
    void SetMsgParentId(std::string);
    void SetMsgSequenceId(std::string);
    void SetSenderMri(std::string); 
    void SetArrivalTime(std::string);
    void SetMsgContainerChannelId(std::string);
    //void SetChannel(Channel*);

    //others
    bool IsTopLevelMsg();
    std::string GetSenderOid();

    //8:orgid:fdb3a4e9-675d-497e-acfe-4fd208f8ad89



    // "messageType": "Text",
    // "content": "Testing event loop (1)",
    // "clientMessageId": "1636757442770",
    // "imDisplayName": null,
    // "properties": {
    //     "importance": ""
    // },
    // "id": "1636757442867",
    // "type": "Message",
    // "composeTime": "2021-11-12T22:50:42.867Z",
    // "originalArrivalTime": "2021-11-12T22:50:42.867Z",
    // "containerId": "19:5c7c73c0315144a4ab58108a897695a9@thread.tacv2",
    // "parentMessageId": "1636757442867",
    // "from": "8:orgid:fdb3a4e9-675d-497e-acfe-4fd208f8ad89",
    // "sequenceId": 15,
    // "version": 1636757442867,
    // "threadType": "topic",
    // "isEscalationToNewPerson": false

};

//Decided against the Reply Chain class foe now but leaving it here for future reference
// class ReplyChain{
//     public:
//     std::vector<Message> messages;

//     ReplyChain(){}

//     ReplyChain(std::vector<Message>& v){
//         messages = v;
//     }
// };