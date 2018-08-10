#include "client/connection_pool.h"
#include "client/parser.h"
using namespace std;



int main(int argc, char** argv) {
    string testHtml = Pool::retrieve_single("en.wikipedia.org", "443", "/wiki/Main_Page");
    Parser parser;
    parser.parse(testHtml);
    parser.page->repr();
}