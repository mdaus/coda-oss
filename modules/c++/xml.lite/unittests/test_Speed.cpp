/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <string>
#include <iostream>
#include <io/StringStream.h>
#include <xml/lite/Speed.h>
#include "TestCase.h"


//#define __TEST_XML_LITE_SPEED_DEBUG__

std::string writeXML(const xml::lite::Speed &xml)
{
  io::StringStream ss_buf;
  xml.getRootElement()->print(ss_buf);

  std::string out = ss_buf.stream().str();
#ifdef  __TEST_XML_LITE_SPEED_DEBUG__
  std::cout<<out<<std::endl;
#endif

  return out;
}

namespace
{
/////
/// Begin Test Cases
//
//-----------------------------------------------------------------
TEST_CASE(testMakeEmptyRootNode)
{
  // Start with an empty root node called "root"
  xml::lite::Speed xml;
  xml.initRoot("root");
  
  std::string truth = "<root/>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testSetRootNodeAttribute)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  
  xml.addRootAttribute("title", "de rerum natura");

  std::string truth = "<root title=\"de rerum natura\"/>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testMakeOneEmptyChild)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.setData("foo","");

  std::string truth = "<root><foo/></root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testMakeOneChildWithData)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.setData("foo", "yeah!");

  std::string truth = "<root><foo>yeah!</foo></root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testChildDataTypeConversionOk)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.setData("foo", 3.14159f);

  std::string truth = "<root><foo>3.14159</foo></root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testMakeChildNodeWithBadIndex)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  TEST_EXCEPTION(xml.setData("foo&0", "oh no!"));

}
//-----------------------------------------------------------------
TEST_CASE(testMakeChildNodeWithBadSyntax)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  TEST_EXCEPTION(xml.setData("21345l&barf", "oh no!"));

}
//-----------------------------------------------------------------
TEST_CASE(testMakeSimpleNested)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.setData("foo>bar>baz>wuz>hooba>dooba","");

  std::string truth =
    "<root><foo><bar><baz><wuz><hooba><dooba/>"
    "</hooba></wuz></baz></bar></foo></root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testMakeSameNameSameLevel)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.setData("foo", 1);
  xml.setData("foo&2", 2);
  xml.setData("foo&3", 3);

  std::string truth = "<root><foo>1</foo><foo>2</foo><foo>3</foo></root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testMakeSameNameSameLevelAutovivify)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.setData("foo&2", 2);

  std::string truth = "<root><foo/><foo>2</foo></root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testMulitpleLevelsSameNames)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.setData("foo&3>bar&2", "A");
  xml.setData("foo&2>bar&3", "B");

  std::string truth = 
    "<root>"
    "<foo/>"
    "<foo><bar/><bar/><bar>B</bar></foo>"
    "<foo><bar/><bar>A</bar></foo>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testSetAnAttributeExistingElement)
{
  xml::lite::Speed xml;
  xml.initRoot("root");

  xml.setData("foo>bar","3+j5");
  xml.addAttribute("foo>bar", "type", "complex");

  std::string truth =
    "<root>"
    "<foo><bar type=\"complex\">3+j5</bar></foo>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testSetAnAttributeNewElement)
{
  xml::lite::Speed xml;
  xml.initRoot("root");

  xml.addAttribute("foo>bar", "type", "complex");

  std::string truth =
    "<root>"
    "<foo><bar type=\"complex\"/></foo>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testSetNextDataElementExisting)
{
  xml::lite::Speed xml;
  xml.initRoot("root");

  xml.setData("foo>bar", "yee");
  xml.setNextData("foo>bar", "haw");

  std::string truth =
    "<root>"
    "<foo><bar>yee</bar><bar>haw</bar></foo>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testSetNextDataElementNotExisting)
{
  xml::lite::Speed xml;
  xml.initRoot("root");

  xml.setNextData("foo>bar", "yee");
  xml.setNextData("foo>bar", "haw");

  std::string truth =
    "<root>"
    "<foo><bar>yee</bar><bar>haw</bar></foo>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testDeclareNamespaceInRootNode)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.declareNamespace("","Hilbert", "http://mathworld.wolfram.com/HilbertSpace.html");

  std::string truth = 
    "<root xmlns:Hilbert=\"http://mathworld.wolfram.com/HilbertSpace.html\"/>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testDeclareNamespaceInChildNode)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.declareNamespace("foo","Hilbert", "http://mathworld.wolfram.com/HilbertSpace.html");

  std::string truth = 
    "<root>"
    "<foo xmlns:Hilbert=\"http://mathworld.wolfram.com/HilbertSpace.html\"/>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testSetNamespaceInChildNode)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.declareNamespace("","Hilbert", "http://mathworld.wolfram.com/HilbertSpace.html");
  xml.setData("foo>bar>oh>yeah","");

  xml.setNamespace("foo", "Hilbert");

  std::string truth = 
    "<root xmlns:Hilbert=\"http://mathworld.wolfram.com/HilbertSpace.html\">"
    "<Hilbert:foo>"
    "<Hilbert:bar><Hilbert:oh><Hilbert:yeah/></Hilbert:oh></Hilbert:bar>"
    "</Hilbert:foo>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testSetNamespaceInChildNodeNoRecurse)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.declareNamespace("","Hilbert", "http://mathworld.wolfram.com/HilbertSpace.html");
  xml.setData("foo>bar>oh>yeah","");

  xml.setNamespace("foo", "Hilbert", false);

  std::string truth = 
    "<root xmlns:Hilbert=\"http://mathworld.wolfram.com/HilbertSpace.html\">"
    "<Hilbert:foo>"
    "<bar><oh><yeah/></oh></bar>"
    "</Hilbert:foo>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testSetCDATA)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.setData("foo","");
  xml.setCDATA("foo", "XML cannot see this! <<><<><><><<>><><><>><><>>>><<");

  std::string truth = 
    "<root>"
    "<foo>"
    "<![CDATA["
    "XML cannot see this! <<><<><><><<>><><><>><><>>>><<"
    "]]>"
    "</foo>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------
TEST_CASE(testNormativeUseCase)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.declareNamespace("","","http://zombo.com");
  xml.setData("A>one", 1);
  xml.setData("A>three", 3);
  xml.setData("A>two", 2); // order matters (but XML doesn't care)
  xml.declareNamespace("A", "num", "urn:numbers/counting");
  xml.setNamespace("A", "num");
  std::string bpoly("B>razzle>poly2d");
  xml.addAttribute(bpoly, "order", 2);
  xml.setData(bpoly+">x", 6.123);
  xml.setNextData(bpoly+">x", 1.354);
  xml.setData(bpoly+">y", 5.098);
  xml.setNextData(bpoly+">y", 0.415);
  xml.setData("B>razzle>name", "soopah razzle");


  std::string truth = 
    "<root xmlns=\"http://zombo.com\">"
    "<num:A xmlns:num=\"urn:numbers/counting\">"
    "<num:one>1</num:one><num:three>3</num:three><num:two>2</num:two></num:A>"
    "<B><razzle>"
    "<poly2d order=\"2\">"
    "<x>6.123</x><x>1.354</x><y>5.098</y><y>0.415</y>"
    "</poly2d>"
    "<name>soopah razzle</name>"
    "</razzle></B>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(testCreateFromExisting)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.declareNamespace("","","http://zombo.com");
  xml.setData("A>one", 1);
  xml.setData("A>three", 3);
  xml.setData("A>two", 2); 

  xml::lite::Speed foo;
  foo.fromDocument(&xml, true);
  foo.setData("lorem>ipsum>dolor>sit>amet", "consectetur adipiscing elit");

  std::string truth = 
    "<root xmlns=\"http://zombo.com\">"
    "<A><one>1</one><three>3</three><two>2</two></A>"
    "<lorem><ipsum><dolor><sit><amet>"
    "consectetur adipiscing elit"
    "</amet></sit></dolor></ipsum></lorem>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(foo), truth);
  // xml is still a view of the same document, it just doesn't own the root
  // node anymore. This means that it should see all the updates done to foo.
  TEST_ASSERT_EQ(writeXML(xml), truth);
}
//=================================================================
TEST_CASE(testCreatedFromExistingNoStealing)
{
  xml::lite::Speed xml;
  xml.initRoot("root");
  xml.declareNamespace("","","http://zombo.com");
  xml.setData("A>one", 1);
  xml.setData("A>three", 3);
  xml.setData("A>two", 2); 

  xml::lite::Speed foo;
  foo.fromDocument(&xml, false);
  foo.setData("lorem>ipsum>dolor>sit>amet", "consectetur adipiscing elit");

  std::string truth = 
    "<root xmlns=\"http://zombo.com\">"
    "<A><one>1</one><three>3</three><two>2</two></A>"
    "<lorem><ipsum><dolor><sit><amet>"
    "consectetur adipiscing elit"
    "</amet></sit></dolor></ipsum></lorem>"
    "</root>";

  TEST_ASSERT_EQ(writeXML(foo), truth);
}
}
//=================================================================
int main ()
{
  TEST_CHECK(testMakeEmptyRootNode);
  TEST_CHECK(testMakeOneEmptyChild);
  TEST_CHECK(testMakeOneChildWithData);
  TEST_CHECK(testChildDataTypeConversionOk);
  TEST_CHECK(testMakeChildNodeWithBadIndex);
  TEST_CHECK(testMakeChildNodeWithBadSyntax);
  TEST_CHECK(testMakeSimpleNested);
  TEST_CHECK(testMakeSameNameSameLevel);
  TEST_CHECK(testMakeSameNameSameLevelAutovivify);
  TEST_CHECK(testMulitpleLevelsSameNames);
  TEST_CHECK(testSetAnAttributeExistingElement);
  TEST_CHECK(testSetAnAttributeNewElement);
  TEST_CHECK(testSetRootNodeAttribute);
  TEST_CHECK(testSetNextDataElementExisting);
  TEST_CHECK(testSetNextDataElementNotExisting);
  TEST_CHECK(testDeclareNamespaceInRootNode);
  TEST_CHECK(testDeclareNamespaceInChildNode);
  TEST_CHECK(testSetNamespaceInChildNode);
  TEST_CHECK(testSetNamespaceInChildNodeNoRecurse);
  TEST_CHECK(testSetCDATA);
  TEST_CHECK(testNormativeUseCase);
  TEST_CHECK(testCreateFromExisting);
  TEST_CHECK(testCreatedFromExistingNoStealing);

  return 0;
}
