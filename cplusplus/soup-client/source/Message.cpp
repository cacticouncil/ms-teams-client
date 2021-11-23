#include "../include/Message.h"

Message::Message (std::string content , std::string id, std::string parentMessageId, int sequenceId, std::string fromMri, std::string arrivalTime, std::string containerId){

    this->content = content;
    this->id = id;
    this->parentMessageId = parentMessageId;
    this->sequenceId =sequenceId;
    this->fromMri =fromMri; 
    this->arrivalTime =arrivalTime;
    this->containerChannelId = containerId;

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

int Message::GetMsgSequenceId(){
    return sequenceId;
}

std::string Message::GetSenderMri(){
    return fromMri;
} 

std::string Message::GetArrivalTime(){
    return arrivalTime;
}

std::string Message::GetMsgContainerChannelId(){
    return this->containerChannelId;
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

void Message:: SetMsgSequenceId(int seq){
    sequenceId=seq;
}

void Message:: SetSenderMri(std::string sender){
    fromMri=sender;
} 

void Message:: SetArrivalTime(std::string time){
    arrivalTime=time;
}

void Message::SetMsgContainerChannelId(std::string container){
    this->containerChannelId = container;
}

bool Message::IsTopLevelMsg(){

    //When id and parent are not the same OR tey ar both empty, we assume this message is NOT a top level message
    if(this->id.compare(parentMessageId) != 0 || (id == "" && parentMessageId == "") ){
        return false;
    }
    else {
        return true;
    }
    
}

std::string Message::GetSenderOid(){

    std::stringstream stream(fromMri);
    std::string value = "";
    getline(stream, value , ':');
    getline(stream, value , ':');
    getline(stream, value );

    return value;
}

std::string Message::GetMessageSummary(){
    std::string result= "Message Summary: \n";
    
    result+= "msgcontent: " + this->GetMsgContent()+ "\n";
    result+= "msgid: " + this->GetMsgId()+ "\n";
    result+= "parentmsgId: " + this->GetMsgParentId()+ "\n";
    result+= "msgseqId: " + std::to_string(this->GetMsgSequenceId())+ "\n";
    result+= "senderMri: " + this->GetSenderMri()+ "\n"; 
    result+= "oid from Mri: " + this->GetSenderOid()+ "\n"; 
    result+= "arrrival time: " + this->GetArrivalTime()+ "\n";
    result+= "containedx in channel id: " + this->GetMsgContainerChannelId()+ "\n";

    return result;
}
    