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
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <str/Convert.h>
#include <str/Manip.h>
#include <except/Exception.h>
#include <sys/Conf.h>
#include <xml/lite/Element.h>
#include <xml/lite/Speed.h>

//=================================================================
void xml::lite::Speed::fromDocument(xml::lite::Document *doc, bool steal)
{
  setRootElement(doc->getRootElement(steal), steal);
  buildTOC();
}
//=================================================================
void xml::lite::Speed::buildTOC(const std::string &parent,
                                const xml::lite::Element * element)
{
  // make a smaller, local map for bookkeeping.
  std::map<std::string, size_t> localRegistry;
  // either both or neither parent and element must be specified
  if (parent.empty() ^ (element == NULL))
  {
    throw except::Exception(Ctxt(
        "Called with only one of parent path or current element."
        " Only neither or both are allowed."));
  }

  if (element == NULL)
  {
    element = mRootNode;
    // If we are starting at the top, clear out the old TOC before rebuilding.
    mTOC.clear();
  }

  std::vector<xml::lite::Element *> childs = element->getChildren();
  for (size_t ii = 0; ii < childs.size(); ii++)
  {
    const std::string lname = childs[ii]->getLocalName();
    if (localRegistry.count(lname) == 0)
    {
      localRegistry[lname] = 1;
    }
    else
    {
      ++localRegistry[lname];
    }
    std::string tocKey = parent+">"
                          +lname+"&"
                          +str::toString<size_t>(localRegistry[lname]);
    mTOC[tocKey] = childs[ii];
    buildTOC(tocKey, childs[ii]);
  }

}
//=================================================================
std::string xml::lite::Speed::getData(const std::string &elname)
{
  return mTOC[elname]->getCharacterData();
}
//=================================================================
void xml::lite::Speed::initRoot(const std::string &rootName)
{
  setRootElement(createElement(rootName,"",""));
}
//=================================================================
xml::lite::Speed::KeyParts xml::lite::Speed::extractParts(const std::string &longname)
{
  xml::lite::Speed::KeyParts parts;
  // make this name a proper name by ensuring zero-ordinal names have '&1' 
  std::vector<std::string> chunks = str::split(longname, ">");

  for(size_t ii = 0; ii<chunks.size(); ++ii)
  { 
    size_t ordDelIdx;
    ordDelIdx = chunks[ii].find_last_of("&");
    if (ordDelIdx == std::string::npos)
    {
      chunks[ii] += "&1";
    }
  }
  
  parts.properName = str::join(chunks, ">");

  size_t delimIdx = parts.properName.find_last_of('>');
  // grab string up to but not including the last '>' delimiter
  if (delimIdx != std::string::npos)
  {
    parts.parent = parts.properName.substr(0, delimIdx);
  }
  else
  {
    parts.parent.clear();
  }
  // grab the string of everything past the last '>' delimiter
  // the +1 works here even if delimIdx is std::string::npos, since that
  // value is essentially -1.
  parts.child = parts.properName.substr(delimIdx+1, std::string::npos);

  size_t ordDelimIdx = parts.child.find_last_of('&');
  parts.childName = parts.child.substr(0, ordDelimIdx);
  if (ordDelimIdx != std::string::npos)
  {
    parts.childOrd = atoi(parts.child.substr(ordDelimIdx+1).c_str());
  }
  else
  {
    // this branch should never happen- above code ensures presence of
    // ordinal delimiter.
    throw except::Exception(Ctxt(
          "Improper name encountered: '"+parts.child+"'. "
          +"This should never happen here."));
  }

  if (parts.childOrd < 1)
  {
        throw except::Exception(
                    "xml::lite::Speed element ordinality starts at 1. "
                    "Name " +parts.child+" not allowed.");
  }
  return parts;
}

//=================================================================
xml::lite::Speed::KeyParts xml::lite::Speed::vivify(const std::string &elname)
{

  xml::lite::Speed::KeyParts parts = extractParts(elname);
  if (mTOC.count(parts.properName) == 0)
  {
    // No element of that name exists already

    // TODO: Should we have an associated URI? Doesn't seem to make a
    // difference.
    xml::lite::Element* const childElem = createElement(parts.childName, "", "");

    if (parts.parent.empty())
    {
      // requested element is at the top level (no parents)

      // Ensure existence of lower ordinality siblings of the same name
      if (parts.childOrd > 1)
      {

        std::string leftsib = 
          parts.childName+"&"+str::toString<size_t>(parts.childOrd - 1);

        if (mTOC.count(leftsib) == 0)
        {
          vivify(leftsib);
        }
      }

      insert(childElem, mRootNode);
      mTOC[parts.properName] = childElem;
      
    }
    else
    {
      // requested element has parents

      if (mTOC.count(parts.parent) == 0)
      {
        // parent doesn't exist yet.
        // create parents which do not exist yet.
        vivify(parts.parent);
      }

      // Ensure existence of lower ordinality siblings of the same name
      if (parts.childOrd > 1)
      {

        std::string leftsib = 
          parts.childName+"&"+str::toString<size_t>(parts.childOrd - 1);

        if (mTOC.count(leftsib) == 0)
        {
          vivify(parts.parent + ">" + leftsib);
        }
      }
      insert(childElem, mTOC[parts.parent]);
      mTOC[parts.properName] = childElem;
    }
  }
  return parts;
}

//=================================================================
void xml::lite::Speed::declareNamespace(const std::string &elname,
                                 const std::string &nsPrefix,
                                 const std::string &nsURI)
{

  std::string nsSep = ":";
  // No prefix means default namespace
  if (nsPrefix.empty())
  {
    nsSep = "";
  }

  if (elname.empty())
  {
    // set for the root node
    addRootAttribute("xmlns"+nsSep+nsPrefix, nsURI);
  }
  else
  {
    addAttribute(elname, "xmlns"+nsSep+nsPrefix, nsURI);
  }
  mNamespaces[nsPrefix] = nsURI;

}
//=================================================================
void xml::lite::Speed::setNamespace(const std::string &elname,
                             const std::string &nsPrefix,
                             const bool recurse)
{

  // We are going to have to parse the elname anyways... just call vivify.
  xml::lite::Speed::KeyParts parts = vivify(elname);
  std::string tocKey = parts.properName;


  if (mNamespaces.count(nsPrefix) == 1)
  {
    mTOC[tocKey]->setQName(nsPrefix+":"+parts.childName);
    mTOC[tocKey]->setUri(mNamespaces[nsPrefix]);
  }
  if (recurse)
  {
    // Set for children, too.
    std::map<std::string, xml::lite::Element*>::iterator tocIt;
    for (tocIt = mTOC.begin(); tocIt != mTOC.end(); ++tocIt)
    {

      if ((tocIt->first).find(parts.properName) == 0
          && tocIt->first != parts.properName)
      {
        setNamespace(tocIt->first, nsPrefix);
      }
    }
  }
}
//=================================================================
void xml::lite::Speed::setCDATA(const std::string &elname,
                         const std::string &cdata)
{
  std::string tocKey = elname;
  if (mTOC.count(elname) == 0)
  {
    // 0 and 1 are the only options for std::map.count()
    xml::lite::Speed::KeyParts parts = vivify(elname);
    tocKey = parts.properName;
  }
  mTOC[tocKey]->setCharacterData("<![CDATA["+cdata+"]]>");
}
