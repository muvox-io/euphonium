#include "JSONObject.h"
#include <stdlib.h>

bell::JSONValue::JSONValue(cJSON *body, std::string key)
{
    this->body = body;
    this->key = key;
}

void bell::JSONValue::operator=(const std::string val)
{
    this->operator=(val.c_str());
}
void bell::JSONValue::operator=(const char *val)
{
    cJSON_AddStringToObject(this->body, this->key.c_str(), val);
}
void bell::JSONValue::operator=(int val)
{
    cJSON_AddNumberToObject(this->body, this->key.c_str(), val);
}

bell::JSONObject::JSONObject()
{
    this->body = cJSON_CreateObject();
}

bell::JSONObject::~JSONObject()
{
    cJSON_Delete(this->body);
}

bell::JSONValue bell::JSONObject::operator[](std::string index)
{
    return bell::JSONValue(this->body, index);
}

std::string bell::JSONObject::toString()
{
    char *body = cJSON_Print(this->body);
    std::string retVal = std::string(body);
    free(body);
    return retVal;
    
}

std::vector<uint8_t> bell::JSONObject::toVector() {
    char *body = cJSON_Print(this->body);
    std::vector<uint8_t> res(body, body + strlen(body));
    free(body);
    return res;
}
