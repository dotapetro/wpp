#pragma once

#include <vector>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <string>
#include <regex>

using namespace std;


std::vector<std::string> split(const std::string& s, char delimiter) {
    vector<std::string> tokens;
    string token;
    stringstream tokenStream;
    tokenStream << s;
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

class Tag{
public:
    string type = ""; // types: div, head, a, link, header. tags: <div>, </div>, <a>, ...
    vector<string> classes;
    vector<string> ids;
    unordered_map<string, vector<string>> props;
    vector<Tag*> children;

    string tag(){
        return string("<") + type + string(">");
    }

    string closure() { return string("<") + "/" + type + string(">"); }

    static string getTypeName(string tag){
        static regex rgx("(\\w+)", regex::optimize);
        auto type = std::sregex_iterator(tag.begin(), tag.end(), rgx); // Type is the first one
        return static_cast<std::smatch>(*type).str();
    }

    static string closureForTag(string tag) {
        return string("<") + "/" + getTypeName(tag) + string(">");
    }


    static string tagWithoutProps(string tag){
        if (containsClosure(tag))
            return string("</") + getTypeName(tag) + string(">");
        return  string("<") + getTypeName(tag) + string(">");
    }

    static bool containsClosure(string tag){
        return (tag.find('/') == string::npos);
    }

    static bool needToSplitPropsValue(string prop){
        string noNeed[] = {"content", "title", "src"};
        for(auto i: noNeed)
            if (i == prop)
                return false;
        return true;
    }

    void resolve_prop_values(string s, vector<string>& v){
        static regex rgx("(\\S+)", regex::optimize);
        auto values_begin = std::sregex_iterator(s.begin(), s.end(), rgx);
        auto values_end = std::sregex_iterator();
        for (std::sregex_iterator i = values_begin; i != values_end; ++i) {
            std::smatch match = *i;
            std::string match_str = match.str();
            v.push_back(match_str);
        }
    }

    void resolve_prop_values(string s, string name, unordered_map<string, vector<string>>& p){
        if(!needToSplitPropsValue(name)){
            vector<string> v;
            v.push_back(s);
            p.insert(make_pair(name, v));
            return;
        }
        static regex rgx("(\\S+)", regex::optimize);
        auto values_begin = std::sregex_iterator(s.begin(), s.end(), rgx);
        vector<string> v;
        auto values_end = std::sregex_iterator();
        for (std::sregex_iterator i = values_begin; i != values_end; ++i) {
            std::smatch match = *i;
            std::string match_str = match.str();
            v.push_back(match_str);
        }
        p.insert(make_pair(name, v));
    }

    void resolve_props(string tag){
        static regex rgx("(\\w+\\s*=\\s*\"[^\"]+)", regex::optimize);
        auto props_begin = std::sregex_iterator(tag.begin(), tag.end(), rgx);
        auto props_end = std::sregex_iterator();
        for (std::sregex_iterator i = props_begin; i != props_end; ++i) {
            std::smatch match = *i;
            std::string match_str = match.str() + "\"";
            string name = split(match_str, '=')[0];
            while(name.c_str()[name.size()-1] == ' ') name = name.substr(0, name.size()-1);
            string values = split(match_str, '=')[1];
            while(values.c_str()[0] == ' ') values = values.substr(1, values.size());
            values = values.substr(1, values.size()-2);
            if(name == "class") resolve_prop_values(values, classes);
            else if(name == "id") resolve_prop_values(values, ids);
            else resolve_prop_values(values, name, props);
        }
    }

    void resolve_children(vector<string> tags){
        vector<vector<string>> children_tags; // vector -> children -> their tags

        deque<string> main_deq;
        for(auto t: tags) main_deq.push_back(t);
        deque<string> curr_deq;

        while(!main_deq.empty()) {
            children_tags.emplace_back();
            do{
                string curr_tag = main_deq.front();
                children_tags[children_tags.size()-1].push_back(curr_tag);
                main_deq.pop_front();
                // after
                if(!curr_deq.empty() && closureForTag( tagWithoutProps(curr_deq.back())) == curr_tag)
                    curr_deq.pop_back();
                else
                    curr_deq.push_back(curr_tag);
            } while(!curr_deq.empty());
        }

        for(auto t: children_tags){
            children.push_back(new Tag(t));
        }

    }

    Tag(vector<string> tags){
        // props = new unordered_map<string, string>();

        // Tag is 'embraced' in itself <a> <b> </b> <c> </c> </a> a is an embracing class with children: c, d
        type = getTypeName(tags[0]);
        resolve_props(tags[0]);

        tags.erase(tags.begin());
        tags.erase(tags.end());

        resolve_children(tags);
    }

    void repr(int spaceLevel = 0){
        if(type != "plaintext") {
            cout << string(spaceLevel * 2, ' ') << type;

            if (!ids.empty()) for (const auto &id: ids) cout << "#" << id;
            if (!classes.empty()) for (const auto &cl: classes) cout << "." << cl;
            if (!props.empty()) {
                cout << " {";
                for (const auto &prop: props) {
                    cout << prop.first << "= [";
                    for (const auto &val: prop.second)
                        cout << "\"" << val << "\"" << ", ";
                    cout << "], ";
                }
                cout << "}";

            }

            cout << endl;
            for (auto c: children)
                c->repr(spaceLevel + 1);
        } else {
            if(props["content"].size() > 0) cout << string(spaceLevel * 2, ' ') << "(pt): " << props["content"][0] << endl;
        }
    }

    Tag* lastKid(){
        return children[0];
    }

    ~Tag(){
        for(auto child: children) delete child;
    }
};
