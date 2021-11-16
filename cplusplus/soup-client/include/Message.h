#pragma once
#include <string>
#include <vector>

class Message{

    public:
    std::string content;
    std::string id;
    std::string parentMessageId;
    std::string sequenceId;
    std::string fromMri; 
    std::string arrivalTime;

    //TO DO:
    //vector of Message pointers to the reply messages
    //Channrl* to parent channel 


    //Could potentially have a User type for sender? 

    Message (std::string content ="", std::string id="", std::string parentMessageId="", std::string sequenceId="", std::string fromMri="", std::string arrivalTime="");
    
    //Accessors
    std::string GetMsgContent();
    std::string GetMsgId();
    std::string GetMsgParentId();
    std::string GetMsgSequenceId();
    std::string GetSenderMri(); 
    std::string GetArrivalTime();
    
    //Modifiers
    void SetMsgContent(std::string);
    void SetMsgId(std::string);
    void SetMsgParentId(std::string);
    void SetMsgSequenceId(std::string);
    void SetSenderMri(std::string); 
    void SetArrivalTime(std::string);

    //topLevelMsg fucntion to check if parent message with children or not




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