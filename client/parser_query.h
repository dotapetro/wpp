#pragma once
#include "tag.h"

class Parser;

// This is a simple query implemented using the Builder pattern
class ParserQuery{
private:
    ParserQuery(unordered_map<string, vector<string>> query, vector<Tag*> query_from){
        this->query = query;
        this->query_from = query_from;
    }
public:

    unordered_map<string, vector<string>> query;
    vector<Tag*> query_from;

    ParserQuery(const string& tagName, const vector<Tag*>& query_from){
        this->query_from = query_from;
        query.insert(make_pair("tag", vector<string>{tagName}));
    }


    ParserQuery _class (initializer_list<string> tagNames){
        query.insert(make_pair("class", tagNames));
        ParserQuery newQ = ParserQuery(query, query_from);
        return newQ;
    }

    ParserQuery _class (const string& tagName){
        query.insert(make_pair("class", vector<string>{tagName}));
        ParserQuery newQ = ParserQuery(query, query_from);
        return newQ;
    }


    explicit ParserQuery(const vector<Tag*>& query_from){
        this->query_from = query_from;
    }

    ParserQuery tag(const string& tagName){
        query.insert(make_pair("tag", vector<string>{tagName}));
        ParserQuery newQ = ParserQuery(query, query_from);
        return newQ;
    }

    ParserQuery search() {
        vector<Tag *> result;
        for(auto tag: query_from){
            tag->search(result, query);
        }
        return ParserQuery(result);
    }

    vector<Tag*> get(){
        return query_from;
    }
};