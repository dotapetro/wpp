#pragma once

#include <vector>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <string>
#include <regex>

using namespace std;


vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream tokenStream;
    tokenStream << s;
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

template<class C, class T>
auto contains(const C& v, const T& x) -> decltype(end(v), true)
{
    return end(v) != std::find(begin(v), end(v), x);
}

class Tag{
private:
    friend class Parser;
protected:
    string type = ""; // types: div, head, a, link, header. tags: <div>, </div>, <a>, ...
    vector<string> classes;
    vector<string> ids;
    unordered_map<string, vector<string>> props;
    vector<Tag*> children;
public:
    // Type name for raw <link src="abc.html" deprecated> -> link
    static string typeName(string tag){
        static regex rgx("(\\w+)", regex::optimize);
        auto type = std::sregex_iterator(tag.begin(), tag.end(), rgx); // Type is the first one
        return static_cast<std::smatch>(*type).str();
    }

    // <html> -> </html>
    static string closureFor(string tag) {
        return string("<") + "/" + typeName(tag) + string(">");
    }

    // <link src="abc.html" content-type="idk" something> -> <link>
    static string withoutProps(const string& tag){
        if (containsClosure(tag))
            return string("</") + typeName(tag) + string(">");
        return  string("<") + typeName(tag) + string(">");
    }

    // <div> -> false, </div> - true
    // Use only with withoutProps! Otherwise might not work correctly with links

    static bool containsClosure(const string& tag){
        // return (tag.find('/') != string::npos);
        static regex closure_before("(<\\s*/.+>)", regex::optimize);
        return regex_match(tag, closure_before);
    }


    // title="What a fancy title", class=[is-outlined, is-red, is-primary]
    static bool needToSplitPropsValue(const string& prop){
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
            p.insert(make_pair(name, vector<string>{s}));
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

    // resolves props for tag <div class="is-outlined is-fancy"> -> div.is-outlined.is-fancy
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

    // resolves children for tags
    void resolve_children(deque<string> main_deq){
        vector<deque<string>> children_tags; // vector -> children -> their tags

        deque<string> curr_deq;

        while(!main_deq.empty()) {
            children_tags.emplace_back();
            do{
                if(main_deq.empty()){ // something went horrifically wrong for some reason, this hack works fine, needs debugging
                    main_deq.clear();
                    curr_deq.clear();
                    break;
                }
                string curr_tag = main_deq.front();
                children_tags[children_tags.size()-1].push_back(curr_tag);
                main_deq.pop_front();

                if(!curr_deq.empty() and closureFor( withoutProps(curr_deq.back())) == curr_tag)
                    curr_deq.pop_back();
                else
                    curr_deq.push_back(curr_tag);
            } while(!curr_deq.empty());
        }

        for(auto t: children_tags){
            children.push_back(new Tag(t));
        }

    }

    Tag(deque<string> tags){
        // props = new unordered_map<string, string>();

        // Tag is 'embraced' in itself <a> <b> </b> <c> </c> </a> a is an embracing class with children: c, d
        type = typeName(tags[0]);
        resolve_props(tags[0]);

        // Another hack that needs to be resolved
        if(tags.size() < 2) return;

        tags.erase(tags.begin());
        tags.erase(tags.end());

        resolve_children(tags);
    }

    ~Tag(){
        for(auto child: children) delete child;
    }

    // Small util func for fancy printing
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

    // Make search
    void search(vector<Tag*>& res, const unordered_map<string, vector<string>>& query){
        for(auto c: children)
            c->search(res, query);
        for(auto p: query){
            if(p.first == "tag") if(type != p.second[0]) return;
            if(p.first == "class") for (const auto& cl: p.second) if(!contains(classes, cl)) return;
        }
        res.push_back(this);
    }
};
