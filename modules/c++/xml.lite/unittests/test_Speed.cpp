/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <stdio.h>
#include <import/io.h>
#include <xml/lite/Speed.h>
#include "TestCase.h"


//#define __TEST_XML_LITE_SPEED_DEBUG__

std::string writexml(const xml::lite::Speed &xml)
{
  io::StringStream ss_buf;
  xml.getRootElement()->print(ss_buf);

  std::string out = ss_buf.stream().str();
#ifdef  __TEST_XML_LITE_SPEED_DEBUG__
  std::cout<<out<<std::endl;
#endif

  return out;
}

// Start with an empty root node called "root"
#define SETUP_XML  xml::lite::Speed xml; xml.initRoot("root");

/////
/// Begin Test Cases
//
//-----------------------------------------------------------------
TEST_CASE(make_empty_root_node)
{
  SETUP_XML;
  
  std::string truth = "<root/>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(set_root_node_attribute)
{
  SETUP_XML;
  
  xml.addRootAttribute("title", "de rerum natura");

  std::string truth = "<root title=\"de rerum natura\"/>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(make_one_empty_child)
{
  SETUP_XML;
  xml.setData("foo","");

  std::string truth = "<root><foo/></root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(make_one_child_with_data)
{
  SETUP_XML;
  xml.setData("foo", "yeah!");

  std::string truth = "<root><foo>yeah!</foo></root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(child_data_type_conversion_ok)
{
  SETUP_XML;
  xml.setData("foo", 3.14159f);;

  std::string truth = "<root><foo>3.14159</foo></root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(make_child_node_with_bad_index)
{
  SETUP_XML;
  TEST_EXCEPTION(xml.setData("foo&0", "oh no!"));

}
//-----------------------------------------------------------------
TEST_CASE(make_child_node_with_bad_syntax)
{
  SETUP_XML;
  TEST_EXCEPTION(xml.setData("21345l&barf", "oh no!"));

}
//-----------------------------------------------------------------
TEST_CASE(make_simple_nested)
{
  SETUP_XML;
  xml.setData("foo>bar>baz>wuz>hooba>dooba","");

  std::string truth =
    "<root><foo><bar><baz><wuz><hooba><dooba/>"
    "</hooba></wuz></baz></bar></foo></root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(make_same_name_same_level)
{
  SETUP_XML;
  xml.setData("foo", 1);
  xml.setData("foo&2", 2);
  xml.setData("foo&3", 3);

  std::string truth = "<root><foo>1</foo><foo>2</foo><foo>3</foo></root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(make_same_name_same_level_autovivify)
{
  SETUP_XML;
  xml.setData("foo&2", 2);

  std::string truth = "<root><foo/><foo>2</foo></root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(mulitple_levels_same_names)
{
  SETUP_XML;
  xml.setData("foo&3>bar&2", "A");
  xml.setData("foo&2>bar&3", "B");

  std::string truth = 
    "<root>"
    "<foo/>"
    "<foo><bar/><bar/><bar>B</bar></foo>"
    "<foo><bar/><bar>A</bar></foo>"
    "</root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(set_an_attribute_existing_element)
{
  SETUP_XML;

  xml.setData("foo>bar","3+j5");
  xml.addAttribute("foo>bar", "type", "complex");

  std::string truth =
    "<root>"
    "<foo><bar type=\"complex\">3+j5</bar></foo>"
    "</root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(set_an_attribute_new_element)
{
  SETUP_XML;

  xml.addAttribute("foo>bar", "type", "complex");

  std::string truth =
    "<root>"
    "<foo><bar type=\"complex\"/></foo>"
    "</root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(set_next_data_element_existing)
{
  SETUP_XML;

  xml.setData("foo>bar", "yee");
  xml.setNextData("foo>bar", "haw");

  std::string truth =
    "<root>"
    "<foo><bar>yee</bar><bar>haw</bar></foo>"
    "</root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(set_next_data_element_not_existing)
{
  SETUP_XML;

  xml.setNextData("foo>bar", "yee");
  xml.setNextData("foo>bar", "haw");

  std::string truth =
    "<root>"
    "<foo><bar>yee</bar><bar>haw</bar></foo>"
    "</root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(declare_namespace_in_root_node)
{
  SETUP_XML;
  xml.declareNamespace("","Hilbert", "http://mathworld.wolfram.com/HilbertSpace.html");

  std::string truth = 
    "<root xmlns:Hilbert=\"http://mathworld.wolfram.com/HilbertSpace.html\"/>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(declare_namespace_in_child_node)
{
  SETUP_XML;
  xml.declareNamespace("foo","Hilbert", "http://mathworld.wolfram.com/HilbertSpace.html");

  std::string truth = 
    "<root>"
    "<foo xmlns:Hilbert=\"http://mathworld.wolfram.com/HilbertSpace.html\"/>"
    "</root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(set_namespace_in_child_node)
{
  SETUP_XML;
  xml.declareNamespace("","Hilbert", "http://mathworld.wolfram.com/HilbertSpace.html");
  xml.setData("foo>bar>oh>yeah","");

  xml.setNamespace("foo", "Hilbert");

  std::string truth = 
    "<root xmlns:Hilbert=\"http://mathworld.wolfram.com/HilbertSpace.html\">"
    "<Hilbert:foo>"
    "<Hilbert:bar><Hilbert:oh><Hilbert:yeah/></Hilbert:oh></Hilbert:bar>"
    "</Hilbert:foo>"
    "</root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(set_namespace_in_child_node_no_recurse)
{
  SETUP_XML;
  xml.declareNamespace("","Hilbert", "http://mathworld.wolfram.com/HilbertSpace.html");
  xml.setData("foo>bar>oh>yeah","");

  xml.setNamespace("foo", "Hilbert", false);

  std::string truth = 
    "<root xmlns:Hilbert=\"http://mathworld.wolfram.com/HilbertSpace.html\">"
    "<Hilbert:foo>"
    "<bar><oh><yeah/></oh></bar>"
    "</Hilbert:foo>"
    "</root>";

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(setCDATA)
{
  SETUP_XML;
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

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------
TEST_CASE(normative_use_case)
{
  SETUP_XML;
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

  TEST_ASSERT_EQ(writexml(xml), truth);
}
//-----------------------------------------------------------------
TEST_CASE(create_from_existing)
{
  SETUP_XML;
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

  TEST_ASSERT_EQ(writexml(foo), truth);
  // xml is still a view of the same document, it just doesn't own the root
  // node anymore. This means that it should see all the updates done to foo.
  TEST_ASSERT_EQ(writexml(xml), truth);
}
//=================================================================
TEST_CASE(created_from_existing_no_stealing)
{
  SETUP_XML;
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

  TEST_ASSERT_EQ(writexml(foo), truth);
}
//=================================================================
int main (int argc, char** argv)
{
  TEST_CHECK(make_empty_root_node);
  TEST_CHECK(make_one_empty_child);
  TEST_CHECK(make_one_child_with_data);
  TEST_CHECK(child_data_type_conversion_ok);
  TEST_CHECK(make_child_node_with_bad_index);
  TEST_CHECK(make_child_node_with_bad_syntax);
  TEST_CHECK(make_simple_nested);
  TEST_CHECK(make_same_name_same_level);
  TEST_CHECK(make_same_name_same_level_autovivify);
  TEST_CHECK(mulitple_levels_same_names);
  TEST_CHECK(set_an_attribute_existing_element);
  TEST_CHECK(set_an_attribute_new_element);
  TEST_CHECK(set_root_node_attribute);
  TEST_CHECK(set_next_data_element_existing);
  TEST_CHECK(set_next_data_element_not_existing);
  TEST_CHECK(declare_namespace_in_root_node);
  TEST_CHECK(declare_namespace_in_child_node);
  TEST_CHECK(set_namespace_in_child_node);
  TEST_CHECK(set_namespace_in_child_node_no_recurse);
  TEST_CHECK(setCDATA);
  TEST_CHECK(normative_use_case);
  TEST_CHECK(create_from_existing);
  TEST_CHECK(created_from_existing_no_stealing);

  return 0;
}
