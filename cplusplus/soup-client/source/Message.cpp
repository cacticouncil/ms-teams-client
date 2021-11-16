#include "../include/Message.h"

Message::Message (std::string content , std::string id, std::string parentMessageId, std::string sequenceId, std::string fromMri, std::string arrivalTime){

    this->content = content;
    this->id = id;
    this->parentMessageId = parentMessageId;
    this->sequenceId =sequenceId;
    this->fromMri =fromMri; 
    this->arrivalTime =arrivalTime;

}

//Accessors 
std::string Message::GetMsgContent(){
    return content;
}

std::string Message::GetMsgId(){
    return id;
}

std::string Message::GetMsgParentId(){
    return parentMessageId;
}

std::string Message::GetMsgSequenceId(){
    return sequenceId;
}

std::string Message::GetSenderMri(){
    return fromMri;
} 

std::string Message::GetArrivalTime(){
    return arrivalTime;
}

//Modifiers
void Message:: SetMsgContent(std::string msg){
    content=msg;
}

void Message:: SetMsgId(std::string msgId){
    id=msgId;
}

void Message:: SetMsgParentId(std::string parentMsg){
    parentMessageId=parentMsg;
}

void Message:: SetMsgSequenceId(std::string seq){
    sequenceId=seq;
}

void Message:: SetSenderMri(std::string sender){
    fromMri=sender;
} 

void Message:: SetArrivalTime(std::string time){
    arrivalTime=time;
}