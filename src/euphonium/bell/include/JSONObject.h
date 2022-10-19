#ifndef JSONOBJECT_H
#define JSONOBJECT_H
#include <cJSON.h>
#include <string>
#include <cstring>
#include <vector>

namespace bell {
    class JSONValue
    {
    public:
        JSONValue(cJSON* body, std::string key);
        void operator=(const std::string val);
        void operator=(const char* val);
        void operator=(int val);

    private:
        cJSON* body;
        std::string key;
    };

    class JSONObject
    {
    public:
        JSONObject();
        ~JSONObject();
        JSONValue operator[](std::string index);
        std::string toString();
        std::vector<uint8_t> toVector();

    private:
        cJSON* body;
    };
}

#endif