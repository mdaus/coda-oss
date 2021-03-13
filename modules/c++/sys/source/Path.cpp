/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <algorithm>

#include <sys/Path.h>

namespace sys
{
Path::Path()
{
}

Path::Path(const Path& parent, const std::string& child) :
    mPathName(joinPaths(parent.mPathName, child))
{
}

Path::Path(const std::string& parent, const std::string& child) :
    mPathName(joinPaths(parent, child))
{
}

Path::Path(const std::string& pathName) :
    mPathName(pathName)
{
}

Path& Path::operator=(const Path& path)
{
    if (this != &path)
    {
        mPathName = path.mPathName;
    }
    return *this;
}

Path::Path(const Path& path) :
    mPathName(path.mPathName)
{
}

std::string Path::normalizePath(const std::string& path)
{
    std::string osDelimStr(Path::delimiter());
    std::string delimStr = osDelimStr;

    //if it's not a forward slash, add it as one of the options
    if (delimStr != "/")
        delimStr += "/";

    //get the drive parts, if any -- we will use the drive later
    Path::StringPair driveParts = Path::splitDrive(path);

    std::vector<std::string> parts = str::Tokenizer(path, delimStr);

    int upCount = 0;
    std::deque<std::string> pathDeque;
    for (std::vector<std::string>::iterator it = parts.begin(); it
            != parts.end(); ++it)
    {
        if (*it == ".")
            continue;
        else if (*it == "..")
        {
            //we want to keep the drive, if there is one
            if (pathDeque.size() == 1 && (*pathDeque.begin())
                    == driveParts.first)
                continue;
            if (pathDeque.size() > 0)
                pathDeque.pop_back();
            else
                upCount++;
        }
        else
            pathDeque.push_back(*it);
    }

    //use the OS-specific delimiters
    std::ostringstream out;
    //only apply the beginning up directories if we didn't start at the root (/)
    if (!str::startsWith(path, osDelimStr) && !str::startsWith(path, "/")
            && driveParts.first.empty())
    {
        if (upCount > 0)
            out << "..";
        for (int i = 1; i < upCount; ++i)
            out << osDelimStr << "..";
    }

    //make sure we don't prepend the drive with a delimiter!
    std::deque<std::string>::iterator it = pathDeque.begin();
    if (!driveParts.first.empty())
        out << *it++;
    for (; it != pathDeque.end(); ++it)
        out << osDelimStr << *it;
    return out.str();
}

std::string Path::joinPaths(const std::string& path1,
                                 const std::string& path2)
{
    std::string osDelimStr(Path::delimiter());

    //check to see if path2 is a root path
    if (str::startsWith(path2, osDelimStr) || str::startsWith(path2, "/")
            || !Path::splitDrive(path2).first.empty())
        return path2;

    std::ostringstream out;
    out << path1;
    if (!str::endsWith(path1, osDelimStr) && !str::endsWith(path1, "/"))
        out << osDelimStr;
    out << path2;
    return out.str();
}

std::vector<std::string> Path::separate(const std::string& path)
{
    Path workingPath = path;
    std::vector<std::string> pathList;
    Path::StringPair pair;
    while ((pair = workingPath.split()).first != workingPath.getPath())
    {
        if (!pair.second.empty())
            pathList.push_back(pair.second);
        workingPath = pair.first;
    }

    std::reverse(pathList.begin(), pathList.end());
    return pathList;
}
static bool isPathRooted(const std::string& path)
{
    #if _WIN32
    const auto backslash_pos = path.find_first_of('\\');
    if (backslash_pos == 0)
    {
        return path.find_first_of("\\\\") ==  0;  // "\\server\folder" is rooted on Windows
    }
    else if ((path.find_first_of(':') == 1) && (backslash_pos == 2))
    {
        return  isalpha(path[0]) ? true : false;  // "C:\" is rooted on Windows
    }
    return false;
    #else
    return path.find_first_of('/') == 0; // "/foo" is rooted on *nix
    #endif
}
std::vector<std::string> Path::separate(const std::string& path, bool& isRooted)
{
    isRooted = isPathRooted(path);
    return separate(path);
}

std::string Path::absolutePath(const std::string& path)
{
    std::string osDelimStr(Path::delimiter());

    Path::StringPair driveParts = Path::splitDrive(path);
    if (!str::startsWith(path, osDelimStr) &&
        !str::startsWith(path, "/") &&
        driveParts.first.empty())
    {
        return Path::normalizePath(Path::joinPaths(
            OS().getCurrentWorkingDirectory(), path));
    }
    else
    {
        return Path::normalizePath(path);
    }
}

bool Path::isAbsolutePath(const std::string& path)
{
#if defined(WIN32) || defined(_WIN32)
    return !Path::splitDrive(path).first.empty();
#else
    return (!path.empty() && path[0] == Path::delimiter()[0]);
#endif
}

Path::StringPair Path::splitPath(const std::string& path)
{
    std::string delimStr(Path::delimiter());

    //if it's not a forward slash, add it as one of the options
    if (delimStr != "/")
        delimStr += "/";

    std::string::size_type pos = path.find_last_of(delimStr);
    if (pos == std::string::npos)
        return Path::StringPair("", path);
    else if (!path.empty() && pos == path.length() - 1)
    {
        // Just call ourselves again without the delimiter
        return Path::splitPath(path.substr(0, path.length() - 1));
    }

    std::string::size_type lastRootPos = path.find_last_not_of(delimStr, pos);
    std::string root;
    if (lastRootPos == std::string::npos)
        root = path.substr(0, pos + 1);
    else
        root = path.substr(0, lastRootPos + 1);
    std::string base = path.substr(path.find_first_not_of(delimStr, pos));
    return Path::StringPair(root, base);
}

Path::StringPair Path::splitExt(const std::string& path)
{
    std::string::size_type pos = path.rfind(".");
    if (pos == std::string::npos)
        return Path::StringPair(path, "");
    return Path::StringPair(path.substr(0, pos), path.substr(pos));
}

std::string Path::basename(const std::string& path, bool removeExt)
{
    std::string baseWithExtension = Path::splitPath(path).second;
    if (removeExt)
    {
        return Path::splitExt(baseWithExtension).first;
    }
    return baseWithExtension;

}

Path::StringPair Path::splitDrive(const std::string& path)
{
#if defined(WIN32) || defined(_WIN32)
    std::string::size_type pos = path.find(":");
#else
    std::string::size_type pos = std::string::npos;
#endif

    if (pos == std::string::npos)
        return Path::StringPair("", path);
    return Path::StringPair(path.substr(0, pos + 1), path.substr(pos + 1));
}

const char* Path::delimiter()
{
#if defined(WIN32) || defined(_WIN32)
    return "\\";
#else
    return "/";
#endif
}

const char* Path::separator()
{
#if defined(WIN32) || defined(_WIN32)
    return ";";
#else
    return ":";
#endif
}

std::vector<std::string> Path::list(const std::string& path)
{
    OS os;
    if (!os.exists(path) || !os.isDirectory(path))
    {
        std::ostringstream oss;
        oss << "'" << path
                << "' does not exist or is not a valid directory";
        throw except::Exception(Ctxt(oss.str()));
    }
    std::vector<std::string> listing;
    Directory directory;
    std::string p = directory.findFirstFile(path.c_str());
    while (!p.empty())
    {
        listing.push_back(p);
        p = directory.findNextFile();
    }
    return listing;
}

std::ostream& operator<<(std::ostream& os, const Path& path)
{
    os << path.getPath().c_str();
    return os;
}
std::istream& operator>>(std::istream& is, Path& path)
{
    std::string str;
    is >> str;
    path.reset(str);
    return is;
}

using path_components = std::vector<std::string>;
class separate_result final
{
    path_components components_;

public:
    bool absolute = false;
    separate_result() = default;

    separate_result(path_components&& components) :
        components_(std::move(components)) { }
    separate_result(const path_components& components) :
        components_(components)  {  }
    void push_back(const std::string& s)
    {
        components_.push_back(s);
    }
    void push_back(std::string&& s)
    {
        components_.push_back(std::move(s));
    }
    const path_components& components() const
    {
        return components_;
    }
};
static separate_result separate_path(const std::string& path)
{
    bool absolute;
    separate_result retval(Path::separate(path, absolute));
    retval.absolute = absolute;
    return retval;
}

std::string Path::merge(const std::vector<std::string>& components, bool isAbsolute)
{
    std::string retval(isAbsolute ? Path::delimiter() : "");

    // Be sure size()-1 doesn't wrap-around
    if (components.empty())
    {
        return retval;
    }

    // We need to know where we are in the path to properly add separaters
    for (size_t i = 0; i < components.size(); i++)
    {
        const auto component = components[i];

        if ((i == 0) && (isAbsolute))
        {
            // don't want \C:\dir on Windows, but need \\server\dir
            const auto colon_pos = component.find_first_of(':');
            if (colon_pos == 1)  // yea, there are devices like PRN:
            {
                retval.clear();  // get rid of delimiter from initialization
            }
            else
            {
                #if _WIN32
                retval += Path::delimiter();  // \\server
                #endif
            }
        }
        
        retval += component;

        if (i < components.size() - 1)
        {
            // various manipulations can result in empty components, espeically at the end
            if (!components[i+1].empty())
            {
                retval += Path::delimiter();  // no trailing delimiter after last component
            }
        }
    }
    return retval;
}
static std::string merge_path(const separate_result& components)
{
    return Path::merge(components.components(), components.absolute);
}

struct ExtractedEnvironmentVariable final
{
    std::string begin; // "foo" of "foo$(BAR)baz"
    std::string variable; // "BAR" of "foo$(BAR)baz"
    std::string end; // "baz" of "foo$(BAR)baz"
};

static ExtractedEnvironmentVariable extractEnvironmentVariable_dollar(std::string component, size_t pos) // make a copy for manipulation
{
    assert(pos != std::string::npos);
    ExtractedEnvironmentVariable retval;
    retval.variable = component;  // assume this really isn't an env. var

    retval.begin = component.substr(0, pos);
    str::replace(component, retval.begin + "$", ""); // don't want to find "(" before "$"
    auto paren = component.find_first_of('(');
    char paren_match = ')';
    if (paren == std::string::npos)
    {
        paren = component.find_first_of('{');
        paren_match = '}';
    }

    if (paren == 0) // ${FOO} or $(FOO)
    {
        const auto paren_match_pos = component.find_first_of(paren_match); // "$(FOO)bar)" will get $(FOO), leaving "bar)"
        if ((paren_match_pos != std::string::npos) && (paren_match_pos > paren))
        {
            retval.variable = component.substr(paren + 1, paren_match_pos - 1);
            retval.end = component.substr(paren_match_pos + 1);
            return retval;
        }
    }

    // not ${FOO} or $(FOO), maybe $FOO-bar ($FOO_BAR is a real name)
    const auto delim = component.find_first_of("-(){}$%");
    if (delim != std::string::npos)
    {
        retval.variable = component.substr(0, delim);
        retval.end = component.substr(delim);
        return retval;
    }

    // must be just "$FOO"
    retval.variable = component;
    return retval;
}

static ExtractedEnvironmentVariable extractEnvironmentVariable_percent(std::string component, size_t pos) // make a copy for manipulation
{
    assert(pos != std::string::npos);
    ExtractedEnvironmentVariable retval;
    retval.variable = component;  // assume this really isn't an env. var

    retval.begin = component.substr(0, pos); // foo%BAR%
    str::replace(component, retval.begin + "%", ""); // %FOO%bar% -> foo_bar% for FOO=foo_
    auto percent_pos = component.find_first_of('%');
    if (percent_pos == std::string::npos) // "foo%BAR"
    {
        retval.begin.clear();
        return retval;
    }

    retval.variable = component.substr(0, percent_pos);
    retval.end = component.substr(percent_pos+1);
    return retval;
}

static ExtractedEnvironmentVariable extractEnvironmentVariable(const std::string& component)
{
    ExtractedEnvironmentVariable retval;
    retval.variable = component; // assume this really isn't an env. var

    const auto dollar_pos = component.find_first_of('$');
    if (dollar_pos != std::string::npos)  // foo$BAR -> "foo_bar" for BAR=_bar
    {
        return extractEnvironmentVariable_dollar(component, dollar_pos);
    }

    #if _WIN32 // %FOO% only on Windows
    const auto percent_pos = component.find_first_of('%');
    if (percent_pos != std::string::npos)
    {
        return extractEnvironmentVariable_percent(component, percent_pos);
    }
    #endif

    return retval;
}

static path_components expandEnvironmentVariable(const std::string& component)
{
    const auto extractedEnvVar = extractEnvironmentVariable(component);
    path_components retval;
    if (extractedEnvVar.variable == component)
    {
        // no env-var syntax found; don't even bother with osSplitEnv()
        retval.push_back(component);
        return retval;
    }

    // Can't check whether the expansions exist as this could just be a piece
    // of a longer path: /foo/$BAR/baz/file.txt
    static const sys::OS os;
    std::string value;
    if (!os.getEnvIfSet(extractedEnvVar.variable, value))
    {
        // No value for the purported "environment variable," assume it's just a
        // path with some funky characters: $({})
        retval.push_back(component);
        return retval;
    }
    const auto paths = str::split(value, sys::Path::separator());

    // Add back the other pieces: "foo$(BAR)baz" -> "foo_bar_baz" for BAR=_bar_
    //
    // The "end" piece could be another env-var: foo$BAR$BAZ
    const auto endExtpandedEnvVar = expandEnvironmentVariable(extractedEnvVar.end);

    path_components updated_paths;
    for (const auto& path : paths)
    {
        for (const auto& endVar : endExtpandedEnvVar)
        {
            auto p = extractedEnvVar.begin + path + endVar;
            updated_paths.push_back(std::move(p));
        }
    }
    return updated_paths;
}

static path_components join(const std::string& v1, const std::string& v2)
{
    // put two strings into a new list
    return path_components{ {v1, v2} };
}
static path_components join(path_components v1, const std::string& v2)
{
    // add a string onto an existing list
    v1.push_back(v2);
    return v1;
}
template <typename C3, typename C1, typename C2>
C3& joined_cartesian_product(const C1& c1, const C2& c2, C3& result)
{
    for (const auto& v1 : c1)
    {
        for (const auto& v2 : c2)
        {
            // Rather than returning a list of (v1_n, v2_n) pairs, we'll "join" the
            // pair into a list [v1_n, v2_n].  That list will in turn be the input for
            // a subsequent cartesian product where we can just add to the end.
            //
            // Input: [a, b], [1, 2]; "normal" output: [(a, 1), (a, 2), (b, 1), (b, 2)]
            // Input: [(a, 1), (b, 2)], [X, Y]; "normal" output: [((a, 1), X), ((a, 1), Y), ((b, 2), X), ((b, 2), Y)]
            // By turning the pairs into a list, we simply products-of-products (and products-of-products-of-products)
            //    Input: [a, b], [1, 2]; output: [[a, 1], [a, 2], [b, 1], [b, 2]]
            //    Input: [[a, 1], [b, 2]], [X, Y]; output: [[a, 1, X], [a, 1, Y], [b, 2,, X], [b, 2, Y]]
            result.push_back(join(v1, v2));
        }
    }
    return result;
}

struct expanded_component final
{
    std::string component;
    std::vector<std::string> value;
};
std::vector<expanded_component> expand_components(const separate_result& components)
{
    std::vector<expanded_component> retval;
    for (const auto& component : components.components())
    {
        expanded_component e{component};
        e.value = expandEnvironmentVariable(component);
        assert(e.value.size() >= 1);  // the component itself should always be there

        retval.push_back(std::move(e));
    }
    return retval;
}

// Generate all the different ways the expansions can be combined.
std::vector<path_components> joined_cartesian_product(const expanded_component& ec1, const expanded_component& ec2)
{
    std::vector<path_components> retval;
    return joined_cartesian_product(ec1.value, ec2.value, retval);
}
std::vector<path_components> joined_cartesian_product(const std::vector<path_components>& ec1, const expanded_component& ec2)
{
    std::vector<path_components> retval;
    return joined_cartesian_product(ec1, ec2.value, retval);
}
std::vector<path_components> expand(const std::vector<expanded_component>& expanded_components)
{
    std::vector<path_components> retval;
    if (expanded_components.empty())
    {
        return retval;
    }
    if (expanded_components.size() == 1)
    {
        // cartesian product of <anything> with <empty> is empty; we want <anything> instead
        for (const auto& s : expanded_components[0].value)
        {
            retval.push_back(join(s, ""));
        }
        return retval;
    }

    retval = joined_cartesian_product(expanded_components[0], expanded_components[1]);
    for (size_t i = 2; i < expanded_components.size(); i++)
    {
        retval = joined_cartesian_product(retval, expanded_components[i]);
    }
    return retval;
}

static std::string expandEnvironmentVariables(const separate_result& components, sys::Filesystem::FileType type)
{
    const auto expanded_components = expand_components(components);
    const auto all_expansions = expand(expanded_components);
    for (const auto& unmerged_path_ : all_expansions)
    {
        separate_result unmerged_path(unmerged_path_);
        unmerged_path.absolute = components.absolute;
        auto path = merge_path(unmerged_path);

        // If the type matches, we're done
        if ((type == Filesystem::FileType::Regular) && Filesystem::is_regular_file(path))
        {
            return path;
        }
        if ((type == Filesystem::FileType::Directory) && Filesystem::is_directory(path))
        {
            return path;
        }
    }

    return "";
}

static std::string expandEnvironmentVariables(const separate_result& components, bool checkIfExists)
{
    const auto expanded_components = expand_components(components);
    const auto all_expansions = expand(expanded_components);
    for (const auto& unmerged_path_ : all_expansions)
    {
        separate_result unmerged_path(unmerged_path_);
        unmerged_path.absolute = components.absolute;
        auto path = merge_path(unmerged_path);
        if (!checkIfExists)
        {
            // not checking for existence, just grab the first one
            return path;
        }
        if (Filesystem::exists(path))
        {
            return path;
        }
    }

    return "";
}

// a single "~" is a bit of a special case
static std::string expandTilde()
{
    static const sys::OS os;

    #ifdef _WIN32
    constexpr auto home = "USERPROFILE";
    #else  // assuming *nix
    // Is there a better way to support ~ on *nix than $HOME ?
    constexpr auto home = "HOME";
    #endif

    std::vector<std::string> paths;
    if (!os.splitEnv(home, paths, sys::Filesystem::FileType::Directory))
    {
        // something is horribly wrong
        throw except::FileNotFoundException(Ctxt(home));
    }

    if (paths.size() != 1)
    {
        // somebody set HOME to multiple directories ... why?
        throw except::FileNotFoundException(Ctxt(home));
    }
    return paths[0];
}

static std::string expandEnvironmentVariables_(std::string path, sys::Filesystem::FileType type) // make a copy for str::replace
{
    constexpr auto tilde_slash = "~/"; // ~\ would be goofy on Windows, so only support ~/
    if (path.find_first_of(tilde_slash) == 0)
    {
        // Don't have to worry about goofy things like ~ expanding to /home/${FOO}
        // expandTilde() ensures the directory exists.
        str::replace(path, tilde_slash, expandTilde() + "/");
    }

    const auto components = separate_path(path);  // "This splits on both '/' and '\\'."
    return expandEnvironmentVariables(components, type);
}

static std::string expandEnvironmentVariables_(std::string path, bool checkIfExists) // make a copy for str::replace
{
    constexpr auto tilde_slash = "~/"; // ~\ would be goofy on Windows, so only support ~/
    if (path.find_first_of(tilde_slash) == 0)
    {
        // Don't have to worry about goofy things like ~ expanding to /home/${FOO}
        // expandTilde() ensures the directory exists.
        str::replace(path, tilde_slash, expandTilde() + "/");
    }

    const auto components = separate_path(path);  // "This splits on both '/' and '\\'."
    return expandEnvironmentVariables(components, checkIfExists);
}

std::string Path::expandEnvironmentVariables(const std::string& path, bool checkIfExists)
{
    // Avoid pathalogical cases where the first env-variable expands to escape or ~
    #ifdef _WIN32
    // https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
    constexpr auto escape = R"(\\?\)";
    #else // assuming *nix
    constexpr auto escape = R"(//)";
    #endif
    if (path.find_first_of(escape) == 0)
    {
        return path;
    }
    if (path == "~")
    {
        return expandTilde();
    }
    return expandEnvironmentVariables_(path, checkIfExists);
}
std::string Path::expandEnvironmentVariables(const std::string& path, sys::Filesystem::FileType type)
{
    // Avoid pathalogical cases where the first env-variable expands to escape or ~
    #ifdef _WIN32
    // https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
    constexpr auto escape = R"(\\?\)";
    #else // assuming *nix
    constexpr auto escape = R"(//)";
    #endif
    if (path.find_first_of(escape) == 0)
    {
        return path;
    }
    if (path == "~")
    {
        return expandTilde();
    }
    return expandEnvironmentVariables_(path, type);
}

}
