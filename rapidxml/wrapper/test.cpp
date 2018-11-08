#include <iostream>
#include "XmlWrapper.h"

using namespace std;

int main(int argc, char* argv[]) {

    XmlWrapper xml_wrapper("config.xml");

    auto node = xml_wrapper.rootNode();
    if(node != "ConfigStart")
        throw std::runtime_error("");

    return 0;
}