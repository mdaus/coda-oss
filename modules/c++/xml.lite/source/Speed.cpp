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
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <import/str.h>
#include <import/except.h>
#include "xml/lite/Element.h"
#include "xml/lite/Speed.h"

//=================================================================
void xml::lite::Speed::fromDocument(xml::lite::Document *doc, bool steal)
{
  setRootElement(doc->getRootElement(steal), steal);
  buildTOC();
}
//=================================================================
void xml::lite::Speed::buildTOC(const std::string &parent,
                         xml::lite::Element const * element)
{
  // make a smaller, local map for bookkeeping.
  std::map<std::string, size_t> local_registry;
  // either both or niether parent and element must be specified
  if (parent.empty() ^ (element == NULL))
  {
    throw except::Exception(
        "xml::lite::Speed::buildTOC() called with only one of"
        " parent path or current element."
        " Only neither or both are allowed.");
  }

  if (element == NULL)
  {
    element = mRootNode;
    // If we are starting at the top, clear out the old TOC before rebuilding.
    mTOC.clear();
  }

  std::vector<xml::lite::Element *> childs = element->getChildren();
  size_t sz = childs.size();
  for (size_t ii = 0; ii < sz; ii++)
  {
    std::string lname = childs[ii]->getLocalName();
    if (local_registry.count(lname) == 0)
    {
      local_registry[lname] = 1;
    }
    else
    {
      ++local_registry[lname];
    }
    std::string toc_key = parent+">"
                          +lname+"&"
                          +str::toString<size_t>(local_registry[lname]);
    mTOC[toc_key] = childs[ii];
    buildTOC(toc_key, childs[ii]);
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

  size_t ord_del_idx;
  for(size_t ii = 0; ii<chunks.size(); ++ii)
  { 
    ord_del_idx = chunks[ii].find_last_of("&");
    if (ord_del_idx == std::string::npos) chunks[ii] += "&1";
  }
  
  parts.proper_name = str::join(chunks, ">");

  size_t delim_idx = parts.proper_name.find_last_of('>');
  // grab string up to but not including the last '>' delimiter
  if (delim_idx != std::string::npos)
  {
    parts.parent = parts.proper_name.substr(0, delim_idx);
  }
  else
  {
    parts.parent = "";
  }
  // grab the string of everything past the last '>' delimiter
  // the +1 works here even if delim_idx is std::string::npos, since that
  // value is essentially -1.
  parts.child = parts.proper_name.substr(delim_idx+1, std::string::npos);

  size_t ord_delim_idx = parts.child.find_last_of('&');
  parts.child_name = parts.child.substr(0, ord_delim_idx);
  if (ord_delim_idx != std::string::npos)
  {
    parts.child_ord = atoi(parts.child.substr(ord_delim_idx+1).c_str());
  }
  else
  {
    // this branch should never happen- above code ensures presence of
    // ordinal delimiter.
    std::cout<<"NOT PROPER NAME! :"<<parts.child<<std::endl;
    parts.child_ord = 1;
  }

  if (parts.child_ord < 1)
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
  if (mTOC.count(parts.proper_name) == 0)
  {
    // No element of that name exists already

    // TODO: Should we have an associated URI? Doesn't seem to make a
    // difference.
    xml::lite::Element* const child_elem = createElement(parts.child_name, "", "");

    if (parts.parent.size() == 0)
    {
      // requested element is at the top level (no parents)

      // Ensure existence of lower ordinality siblings of the same name
      if (parts.child_ord > 1)
      {

        std::string leftsib = 
          parts.child_name+"&"+str::toString<int>(parts.child_ord - 1);

        if (mTOC.count(leftsib) == 0)
        {
          vivify(leftsib);
        }
      }

      insert(child_elem, mRootNode);
      mTOC[parts.proper_name] = child_elem;
      
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
      if (parts.child_ord > 1)
      {

        std::string leftsib = 
          parts.child_name+"&"+str::toString<int>(parts.child_ord - 1);

        if (mTOC.count(leftsib) == 0)
        {
          vivify(parts.parent + ">" + leftsib);
        }
      }
      insert(child_elem, mTOC[parts.parent]);
      mTOC[parts.proper_name] = child_elem;
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
  if (nsPrefix.size() == 0)
  {
    nsSep = "";
  }

  if (elname.size() == 0)
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
  std::string toc_key = parts.proper_name;


  if (mNamespaces.count(nsPrefix) == 1)
  {
    mTOC[toc_key]->setQName(nsPrefix+":"+parts.child_name);
    mTOC[toc_key]->setUri(mNamespaces[nsPrefix]);
  }
  if (recurse)
  {
    // Set for children, too.
    std::map<std::string, xml::lite::Element*>::iterator toc_it;
    for (toc_it = mTOC.begin(); toc_it != mTOC.end(); ++toc_it)
    {

      if ((toc_it->first).find(parts.proper_name) == 0
          && toc_it->first != parts.proper_name)
      {
        setNamespace(toc_it->first, nsPrefix);
      }
    }
  }
  return;
}
//=================================================================
void xml::lite::Speed::setCDATA(const std::string &elname,
                         const std::string &cdata)
{
  std::string toc_key = elname;
  if (mTOC.count(elname) == 0)
  {
    // 0 and 1 are the only options for std::map.count()
    xml::lite::Speed::KeyParts parts = vivify(elname);
    toc_key = parts.proper_name;
  }
  mTOC[toc_key]->setCharacterData("<![CDATA["+cdata+"]]>");

  return;
}
