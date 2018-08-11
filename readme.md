# W++
#### C++ bindings for web

Wpp is a feature rich (not yet) header-only library for web
It requires **Boost** *(1.67 or higher)* and c++ 11

### Toolset
| Class | Pattern | What for |
| ------ | ------ | -------- |
| Tag | Composite | Html tag represintaion |
| ConnectionPool | Abstract factory | Client connection managment  |
| ParserQuery| Builder | Querying a tag |

### Naming convention
CamelCase: class names
lowerCamelCase: static utility methods, class members, getters/setters
snake_case: important/retrieving methods, functions

method names:
 - **make** modifies given argument passed by a reference | note: sometimes may take a class member by a reference. In that case, retrieve is preferred
 - **retrieve** modifies class members
 - **get/set** only to get/set a class member
 - -class- no need for *classDoSomething* or "doSomethingForClass". It is quite clear when it is used within a class context. Otherwise className::doSomething makes everything clear