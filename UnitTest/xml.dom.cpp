#include "pch.h"
#include "CppUnitTest.h"

#include <xml/lite/DOMParser.h>
#include <xml/lite/DOMDocument.h>
#include <xml/lite/DOMSerializer.h>

namespace xml_dom
{

TEST_CLASS(test_xmldom){ public:
#include "xml.dom/unittests/test_xmldom.cpp"
};

}