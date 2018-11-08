#ifndef __JJC__XML__WRAPPER__H__
#define __JJC__XML__WRAPPER__H__

#include "../rapidxml.hpp"
#include "../rapidxml_utils.hpp"

#include <vector>
#include <list>
#include <fstream>
#include <map>

#include "XmlNode.h"

using namespace rapidxml;

class XmlWrapper {
private:
    xml_document<> doc;
    std::vector<char> buf;

public:
    // default constructor
    XmlWrapper(void);

    // provide file as argument
    XmlWrapper(const std::string& filename);

    // open an xml file and determine is successful
    void open(const std::string& filename);

    // release all dynamic memory this 
    // object has instantiated
    void clean(void);

    // get the root node with given name
    XmlNode rootNode(const std::string& root_name);
};

XmlNode XmlWrapper::rootNode(const std::string& root_name = "") {
    if(root_name == "") {
        // empty string
        return XmlNode(this->doc.first_node());
    }
    else {
        return XmlNode(this->doc.first_node(root_name.c_str()));
    }
}

XmlWrapper::XmlWrapper(void) { }
XmlWrapper::XmlWrapper(const std::string& filename) {
    this->open(filename);
}

// heavy duty work opening and parsing the file
void XmlWrapper::open(const std::string& filename) {
    std::ifstream is(filename);
    std::vector<char> buf((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    buf.push_back('\0');

    this->buf = buf;
    this->doc.parse<0>(&this->buf[0]);
}



void XmlWrapper::clean(void) {
    
}

#endif // __JJC__XML__WRAPPER__H__