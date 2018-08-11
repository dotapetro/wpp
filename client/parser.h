#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <deque>
#include "tag.h"
#include "parser_query.h"


// An object to parse an html string into a tag object and perform queries on it
// Note: it takes a single tag string as an argument (<html>...</html>),
// If string contains two or more tags (<html>...</html><html>...</html>) on the first 'level' it will fail to parse it

bool matchStr(const char* str1, const char* str2, int size){
    for(int i = 0; i < size; i++) if(str1[i] != str2[i]) return false;
    return true;
}

class Parser{
public:
    Parser(){
        page = nullptr;
    };
    ~Parser(){
        delete page;
    }
    Tag* page;
    void makeStack(const string& s, deque<string>& tags){
        stringstream ss;

        for(auto it = s.begin(); it < s.end();){
            if(*it =='<'){
                if(*(it+1) == '!') { // Ran into a comment
                    while (*it != '>'){it++; };

                } else {
                    // We are inside the tag brackets
                    ss.str("");
                    while (*it != '>') {
                        ss << *it;
                        it++;
                    }
                    ss << *it;
                    tags.push_back(ss.str());
                }
            } else if(*it !='\n' and *it!=' ') { // We are inside the tag itself, adding the plaintext
                ss.str("");
                stringstream content;
                ss << "<plaintext content=\"";
                while(it < s.end() && *it != '<'){ // new tag is about to open
                    content << *it;
                    it++;
                }

                if(!tags.empty()) if(Tag::typeName(tags[tags.size()-1]) == "style") continue;

                string res = content.str();
                while(res.c_str()[0] == '>') res = res.substr(1, res.size()-1);
                static regex contains_words("(\\w+)", regex::optimize);
                if(!regex_search(res, contains_words)) continue;
                ss << res;
                ss << "\">";
                tags.emplace_back(ss.str());
                tags.emplace_back("</plaintext>");
            }
        }
    }
    void parse(const string& s){

        static const string BANNED_TAGS[]  = {"meta", "br", "link", "base", "hr", "wbr", "area", "img", "param", "input"};

        deque<string> tags;

        cout << "\nRequest recieved, working\n";

        makeStack(s, tags);
        cout << "Stack made, cleaning...\n";

        tags.erase(remove_if(tags.begin(), tags.end(), [](string s) {
            for(const auto& el: BANNED_TAGS){
                if(Tag::typeName(s) == el) return true;
            }
            return false;
        }), tags.end());

        for(auto tag: tags)
            cout << tag << endl;

        cout << "Cleaning complete, building the tree\n\n";
        page = new Tag(tags);
    }
    ParserQuery query(const string& tagName){ return ParserQuery(tagName, {page}); }
    ParserQuery query(){ return ParserQuery({page}); }
};