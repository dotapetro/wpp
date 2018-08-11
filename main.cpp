#include "client/connection_pool.h"
#include "client/parser.h"
using namespace std;

int main(int argc, char** argv) {
    //string testHtml = getHttp("www.vseinstrumenti.ru", "/instrument/shlifmashiny/bolgarka_ushm/", "80");
    // cout << "thtml: " << testHtml << endl;
    HttpsConnectionPool pool("en.wikipedia.org");
    auto testHtml = pool.get("/wiki/JQuery");
    Parser parser;
    parser.parse(testHtml);
    parser.page->repr();
    cout << "\nParsed\n\n";

    //vector<Tag*> t =  parser.query().tag("a")._class("interlanguage-link-target").search().tag("plaintext").search().get();
    //for(auto a:  t)
        //a->repr();
}