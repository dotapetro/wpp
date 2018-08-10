#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <deque>
#include "tag.h"

class Parser{
public:
    Parser(){
        page = nullptr;
    };
    ~Parser(){
        delete page;
    }
    Tag* page;
    void make_stack(const string& s, vector<string>& tags){
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
            } else if(*it !='\n' and *it!=' ') {
                // We are inside the tag itself
                // Adding the plaintext
                ss.str("");
                stringstream content;
                ss << "<plaintext content=\"";
                while(it < s.end() && *it != '<'){ // new tag is about to open
                    content << *it;
                    it++;
                }
                string res = content.str();
                while(res.c_str()[0] == '>') res = res.substr(1, res.size()-1);
                static regex contains_words("(\\w+)", regex::optimize);
                if(!regex_search(res, contains_words) or Tag::getTypeName(tags[tags.size()-1]) == "style" or Tag::getTypeName(tags[tags.size()-1]) == "script" ) continue; // Smth weird happening here
                ss << res;
                ss << "\">";
                tags.emplace_back(ss.str());
                tags.emplace_back("</plaintext>");
            }
        }
    }
    void parse(const string& s){

        static const string BANNED_TAGS[]  = {"meta", "br", "link", "base", "hr", "wbr", "area", "img", "param", "input"};

        vector<string> tags;

        cout << "\nRequest recieved, working\n";

        make_stack(s, tags);
        cout << "Stack made, cleaning...\n";

        tags.erase(remove_if(tags.begin(), tags.end(), [](string s) {
            for(const auto& el: BANNED_TAGS){
                if(Tag::getTypeName(s) == el) return true;
            }
            return false;
        }), tags.end());

        cout << "Cleaning complete, building the tree\n\n";
        page = new Tag(tags);
    }
};