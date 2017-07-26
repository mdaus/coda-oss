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

#ifndef __XML_LITE_SPEED_H__
#define __XML_LITE_SPEED_H__

#include <string>
#include <vector>
#include <map>
#include <import/str.h>
#include "xml/lite/Document.h"
#include "xml/lite/Element.h"

namespace xml 
{
namespace lite
{
//=================================================================
/*
 * \class xml::lite::Speed
 * \brief An efficient interface for creating XML documents
 *
 * This class is designed to make building XML documents simple and fast for
 * developers. It features direct access to any element in the document and
 * autovivification of elements and any necessary parents or siblings. 
 * Direct access is achieved using a simple, compact syntax:
 *
 *  '>' shows hierarchy
 *  '&' shows ordinality (starting with one)
 *  example:
 *         XML                 xml::lite::Speed Syntax
 *  ----------------------------------------------------
 *       <root>             |                
 *           <foo>          | "foo"             
 *               <bar/>     | "foo&1>bar&1" OR "foo>bar"             
 *               <bar>      | "foo&1>bar&2" OR "foo>bar&2"
 *                   <aye/> | "foo&1>bar&2>aye&1" OR "foo>bar&2>aye" 
 *               </bar>     |
 *           </foo>         |                
 *       </root>            |                
 *
 *  As shown above, the name of the root element is not used when referring to
 *  other elements in the document to prevent dull repetition. To set, modify,
 *  or access the root element itself, use initRoot(), addRootAttribute(), and
 *  the inherited getRootElement() method.
 *
 *  Apologia: The author here attempts to justify the creation of this class
 *  and the meager but novel syntax it introduces in the presence of existing
 *  standardized XML expression and query languages such as XPath and XQuery
 *  (See http://www.w3.org/TR/xpath ; http://www.w3.org/TR/xpath-31 ;
 *  http://www.w3.org/TR/xquery-3). This class seeks to solve the problem of
 *  creating an XML document where none exists, whereas the languages mentioned
 *  above are focused on accessing and manipulating elements in pre-existing
 *  XML documents. In order to facilitate autovivification the syntax must
 *  unambiguously refer to single elements only, which would mean severely
 *  restricting allowable XPath syntax for this use. It is hoped that the
 *  utility and shallow learning curve will excuse the introduction of a new
 *  syntax.
 */
class Speed : public xml::lite::Document
{
  public:
    /*!
     * \func initRoot
     * \brief Creates the root element of the XML document.
     *
     * \param[in] rootName The name of the root element of the document.
     */
    void initRoot(const std::string &rootName);

    /*!
     * \func addRootAttribute
     * \brief Adds an XML attribute to the root element of the document.
     *
     * \param[in] attrname A string that is the name of the new attribute.
     * \param[in] value    Value of the attribute.
     *                     If value is not a string it will be turned into one internally.
     */
    template <typename T>
      void addRootAttribute(const std::string &attrname, T value);

    /*!
     * \func setData
     * \brief Sets the value of an element.
     *
     * If the element does not exist, it is created.
     * If the parent elements do not exist, they are created.
     * If the sibling elements to the left do not exist, they are created.
     * A value of "" makes an empty element.
     *
     * \param[in] elname The full name of the element being set. Specified using
     *                   the syntax above.
     *
     * \param[in] value The value for the element elname. If it is not a string, it
     *                  will be converted internally.
     */
    template <typename T>
      void setData(const std::string &elname, const T &value);

    /*!
     * \func addAttribute
     * \brief Adds an XML attribute to an element.
     *
     * \param[in] elname   The full name of the element being modified. See
     *                     setData for syntax.
     * \param[in] attrname The name of the attribute to add to element elname
     * \param[in] value    The value of the attribute attrname.If it is not a
     *                     string, it will be converted internally.
     */
    template <typename T>
      void addAttribute(const std::string &elname,
                        const std::string &attrname,
                        T value);
    /*!
     * \func setNextData
     * \brief Creates and sets the next element with the name given at the path
     *        specified.
     *
     *        For example, if the XML document looks like this:
     *            <root>
     *                <foo/>
     *            </root>
     *        calling  setNextdata("foo","new")  would create another <foo> after
     *        the currently existing one resulting in:
     *            <root>
     *                <foo/>
     *                <foo>new</foo>
     *            </root>
     *
     * \param[in] elname The full name of the element getting a new sibling.
     * \param[in] value  The value of the attribute attrname. If value is not
     *                   a string, it will be converted internally.
     *
     * \return A string containing the full name of the new element.
     */
    template <typename T>
      std::string setNextData(const std::string &elname, const T &value);

    /*!
     * \func declareNamespace
     * \brief Declares an XML namespace on a given element.
     *
     * Note that this method merely declares a namespace. To set the namespace
     * of elements, see setNamespace()
     *
     * \param[in] elname The full name of the element where the namespace will
     *                    be declared.
     * \param[in] nsPrefix The namespace prefix being declared.
     * \param[in] nsURI The URI associated with the namespace.
     */
    void declareNamespace(const std::string &elname,
                          const std::string &nsPrefix,
                          const std::string &nsURI);

    /*!
     * \func setNamespace
     * \brief Sets a namespace prefix on an element and possibly all children.
     *
     * The recurse option is true by default.
     * The element will be created if it does not already exist, but don't use
     * this function for that. If there are going to be child nodes also in
     * this namespace, consider creating them all using setData, then using
     * setNamespace to set them all at once. Calling setNamespace for leaf
     * nodes (as in the case where the element does not yet exist) with
     * recursion enabled will perform a needless search of the existing nodes.
     *
     * \param[in] elname The full name of the element being modified.
     * \param[in] nsPrefix 
     */
    void setNamespace(const std::string &elname,
                      const std::string &nsPrefix,
                      const bool recurse=true);
    
    /*!
     * \func getData
     * \brief Gets the character data of an element
     *
     * \param elname The full name of the element.
     * \return A String containing the character data.
     */
    std::string getData(const std::string &elname);

    /*!
     * \func fromDocument
     * \brief Set up this object to use an existing XML document
     *
     * This method is used to take an existing XML document and extend or
     * manipulate it. A reasonable use case would be to parse an XML template
     * file and fill it out or possibly add to it. If the goal is gathering
     * information for processing elsewhere, consider using XPath or XQuery,
     * available in libxml2. CODA provides libxml2 as an optional driver for
     * XML functionality.
     *
     * \param doc Pointer to the xml::lite::Document to now refer to.
     * \param steal Steal ownership of the root element of doc.
     *              Default is true.
     */
    void fromDocument(xml::lite::Document *doc, bool steal=true);

    /*!
     * \func setCDATA
     * \brief Set the value of a CDATA element at at location
     *
     *  setCDATA takes a string value and inserts it at the given location
     *  surrounded by the proper XML syntax: '<![CDATA[' before and ']]>'after.
     *  CDATA elements are not parsed as part of the XML document and so are
     *  suitable for embedding arbitrary strings, e.g. HTML content.
     *
     *  CDATA sections are treated by this library in the same way as text
     *  content of an element. There are limitations inherent in this design
     *  which are meant not to be confining. While it is conceptually possible
     *  to have both text and CDATA, or multiple CDATAs possibly interspersed
     *  with text, etc. in the same tag, this function will not support that. A
     *  call to setCDATA will overwrite any previous text value or CDATA at
     *  that location. In these cases, compose the complete string and then use
     *  setData.
     *
     * \param elname The full name of the element.
     * \param cdata  The contents of the CDATA section.
     */
    void setCDATA(const std::string &elname, const std::string &cdata);
    // TODO: 
    // setComment(const std::string &elname, const std::string &cdata);
    // declareSchema(const std::string &elname,
    //               const std::string &nsPrefix,
    //               const std::string &nsURI);

  protected:
    /*! \func buildTOC
     *  \brief Build the table of contents map
     *
     */
    void buildTOC(const std::string &parent="",
                  xml::lite::Element const * element=NULL);

    struct KeyParts
    {
      std::string proper_name;
      std::string parent;
      std::string child;
      std::string child_name;
      size_t      child_ord;
    };
    
    KeyParts vivify(const std::string &elname);
    KeyParts extractParts(const std::string &longname);

    //! The table of contents for the XML document.
    std::map<std::string, xml::lite::Element*> mTOC;
    //! The namespaces known to exist in the document.
    std::map<std::string, std::string> mNamespaces;

};
} // end namespace "lite"
} // end namespace "xml"
//-----------------------------------------------------------------
  template <typename T>
void xml::lite::Speed::setData(const std::string &elname, const T &value)
{
  std::string toc_key = elname;
  if (mTOC.count(elname) == 0)
  {
    // 0 and 1 are the only options for std::map.count()
    xml::lite::Speed::KeyParts parts = vivify(elname);
    toc_key = parts.proper_name;
  }
  mTOC[toc_key]->setCharacterData(str::toString<T>(value));

};
//-----------------------------------------------------------------
  template <typename T>
std::string xml::lite::Speed::setNextData(const std::string &elname, const T &value)
{
  // Take the hit of parsing the element name into parts.
  xml::lite::Speed::KeyParts parts = extractParts(elname);
  // Create the parents of this element if need be.
  xml::lite::Speed::KeyParts parent_parts = vivify(parts.parent);

  // Find out how many elements by this name there are right now.
  std::vector<xml::lite::Element* > sibs;
  mTOC[parts.parent]->getElementsByTagName(parts.child_name, sibs, false);

  std::string next_name =
    parts.parent+">"+parts.child_name+"&"+str::toString(sibs.size()+1);
  xml::lite::Speed::KeyParts next_parts = vivify(next_name);

  mTOC[next_parts.proper_name]->setCharacterData(str::toString<T>(value));

  return next_parts.proper_name;
};
//-----------------------------------------------------------------
  template <typename T>
void xml::lite::Speed::addAttribute(const std::string &elname,
                             const std::string &attrname,
                             T value)
{
  std::string toc_key = elname;
  if (mTOC.count(elname) == 0)
  {
    xml::lite::Speed::KeyParts parts = vivify(elname);
    toc_key = parts.proper_name;
  }
  xml::lite::Attributes attributes = mTOC[toc_key]->getAttributes();
  xml::lite::AttributeNode node;
  node.setLocalName(attrname);
  node.setValue(str::toString<T>(value));
  attributes.add(node);
  mTOC[toc_key]->setAttributes(attributes);
}
//-----------------------------------------------------------------
  template <typename T>
void xml::lite::Speed::addRootAttribute(const std::string &attrname,
                             T value)
{
  xml::lite::Attributes attributes = mRootNode->getAttributes();
  xml::lite::AttributeNode node;
  node.setLocalName(attrname);
  node.setValue(str::toString<T>(value));
  attributes.add(node);
  mRootNode->setAttributes(attributes);
}
#endif
