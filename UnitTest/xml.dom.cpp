#include "pch.h"
#include "CppUnitTest.h"

#include <xml/dom/DOMParser.h>
#include <xml/dom/DOMDocument.h>
#include <xml/dom/DOMSerializer.h>

namespace xml_dom
{

TEST_CLASS(test_xmldom){ public:
#include "xml.dom/unittests/test_xmldom.cpp"
};

}