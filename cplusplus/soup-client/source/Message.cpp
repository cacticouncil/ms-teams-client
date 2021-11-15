#include "../include/Message.h"

Message::Message (std::string content , std::string id, std::string parentMessageId, std::string sequenceId, std::string fromMri, std::string arrivalTime){

    this->content = content;
    this->id = id;
    this->parentMessageId = parentMessageId;
    this->sequenceId =sequenceId;
    this->fromMri =fromMri; 
    this->arrivalTime =arrivalTime;

}