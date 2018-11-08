#ifndef __JJC__XML__NODE__H__
#define __JJC__XML__NODE__H__

#include <string>

#include "../rapidxml.hpp"
#include "../rapidxml_utils.hpp"

using namespace rapidxml;

class XmlNode {
private:
    // this shouldnt be used from user program 
    // directly. use access methods
    xml_node<>* node_name;
    xml_attribute<>* attr_name;

public:

    XmlNode(void) {
        this->node_name = NULL;
        this->attr_name = NULL;
    }

    XmlNode(xml_node<>* ptr) {
        this->node_name = ptr;
        this->attr_name = NULL;
    }

    XmlNode(xml_attribute<>* ptr) {
        this->attr_name = ptr;
        this->node_name = NULL;
    }

    // copy constructor
    XmlNode(const XmlNode& node) {
        this->node_name = node.node_name;
        this->attr_name = node.attr_name;
    }

    XmlNode nextSibling(void) {
        return XmlNode(this->node_name->next_sibling());
    }

    XmlNode child(const std::string& name) {
        return XmlNode(this->node_name->first_node(name.c_str()));
    }

    XmlNode child(void) {
        return XmlNode(this->node_name->first_node());
    }

    const char* attribute(const std::string& name) {
        return this->node_name->first_attribute(name.c_str())->value();
    }

    const char* name() {
        return this->node_name->name();
    }

    bool nameIs(const std::string& node_name) {
        if(this->attr_name != NULL) 
            return (node_name == this->attr_name->name());
        else if(this->node_name != NULL)
            return (node_name == this->node_name->name());
        else
            throw std::runtime_error("XmlNode::nameIs -> no allocated node pointers");
    }

    friend bool operator==(XmlNode& lhs, XmlNode& rhs) {
        return (lhs.attr_name == rhs.attr_name || 
                lhs.node_name == rhs.node_name);
    }

    friend bool operator==(const std::string& str, XmlNode& node) {
        return (str == node.name());
    }

    friend bool operator==(XmlNode& node, const std::string& str) {
        return (str == node); // call other overloaded method
    }

    friend bool operator!=(const std::string& str, XmlNode& node) {
        return !(str == node);
    }

    friend bool operator!=(XmlNode& node, const std::string& str) {
        reutrn !(str == node);
    }

};

// initialization of static class variable

#endif // __JJC__XML__NODE__H__