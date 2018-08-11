#include "client/connection_pool.h"
#include "client/parser.h"
using namespace std;


int main(int argc, char** argv) {
    //string testHtml = getHttp("www.vseinstrumenti.ru", "/instrument/shlifmashiny/bolgarka_ushm/", "80");
    //cout << "thtml: " << testHtml << endl;
     //HttpsConnectionPool pool("en.wikipedia.org");
     //auto testHtml = pool.get("/wiki/JQuery");

    HttpsConnectionPool pool("www.avito.ru");
    auto testHtml = pool.get("/moskva/lichnye_veschi?view=list");

    Parser parser;
    parser.parse(testHtml);
    parser.page->repr();
    cout << "\nParsed\n\n";

    vector<Tag*> t =  parser.query().tag("div")._class("item").search().tag("a")._class("description-title-link").search().tag("span").search().tag("plaintext").search().get();
    for(auto a:  t)
        a->repr(0);
}