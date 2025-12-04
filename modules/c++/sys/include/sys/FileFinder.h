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

#pragma once
#ifndef __SYS_FILE_FINDER_H__
#define __SYS_FILE_FINDER_H__

#include <functional>
#include <vector>
#include <string>
#include <utility>

#include "config/Exports.h"
#include <filesystem>

namespace sys
{

/**
 * Predicate interface for all entries
 */
struct CODA_OSS_API FilePredicate
{
    using argument_type = std::string;
    using result_type = bool;

    virtual ~FilePredicate() = default;
    virtual bool operator()(const std::string& entry) const = 0;
};

/**
 * Predicate interface for existance
 */
struct ExistsPredicate : FilePredicate
{
    virtual ~ExistsPredicate() = default;
    virtual bool operator()(const std::string& entry) const override;
};

/**
 * Predicate that matches files only (no directories)
 */
struct CODA_OSS_API FileOnlyPredicate : public FilePredicate
{
    virtual ~FileOnlyPredicate() = default;
    virtual bool operator()(const std::string& entry) const override;
};

/**
 * Predicate that matches directories only (no files)
 */
struct DirectoryOnlyPredicate: public FilePredicate
{
    virtual ~DirectoryOnlyPredicate() = default;
    virtual bool operator()(const std::string& entry) const override;
};

/**
 * Predicate that matches directories only (no files)
 */
struct FragmentPredicate : public FilePredicate
{
    FragmentPredicate(const std::string& fragment, bool ignoreCase = true);
    bool operator()(const std::string& entry) const override;

private:
    std::string mFragment;
    bool mIgnoreCase;

};


/**
 * Predicate interface for filtering files with a specific extension
 * This method will not match '.xxx.yyy' type patterns, since the 
 * splitting routines will only find '.yyy'.  See re::RegexPredicate
 * for a more useful finder.
 */
struct ExtensionPredicate: public FileOnlyPredicate
{
    ExtensionPredicate(const std::string& ext, bool ignoreCase = true);
    bool operator()(const std::string& filename) const override;

private:
    std::string mExt;
    bool mIgnoreCase;
};

/**
 * Predicate that does logical not of another predicate (ie !)
 */
struct NotPredicate : public FilePredicate
{
    NotPredicate(FilePredicate* filter, bool ownIt = false);
    virtual ~NotPredicate();

    virtual bool operator()(const std::string& entry) const override;

protected:
    typedef std::pair<FilePredicate*, bool> PredicatePair;
    PredicatePair mPredicate;
};


/**
 *  The LogicalPredicate class allows you to chain many 
 *  predicates using the logical && or ||
 */
struct LogicalPredicate : public FilePredicate
{
    LogicalPredicate() = default;
    LogicalPredicate(bool orOperator);
    virtual ~LogicalPredicate();

    sys::LogicalPredicate& addPredicate(FilePredicate* filter, 
                                        bool ownIt = false);

    virtual bool operator()(const std::string& entry) const override;

protected:
    bool mOrOperator = true;
    typedef std::pair<FilePredicate*, bool> PredicatePair;
    std::vector<PredicatePair> mPredicates;
};

/**
 * \class FileFinder
 *
 *  The FileFinder class allows you to search for 
 *  files/directories in a clean way.
 */
struct CODA_OSS_API FileFinder final
{
    FileFinder() = default;
    ~FileFinder() = default;

    /**
     * Perform the search
     * \return a std::vector<std::string> of paths that match
     */
    static std::vector<std::string> search(
        const FilePredicate& filter,
        const std::vector<std::string>& searchPaths, 
        bool recursive = false);
};

// Recurssively search the entire directory structure, starting at "startingDirectory", for the given file.
// If the file isn't found below "startingDirectory", the process is repated using the parent directory
// until either the file is found or we stop at a ".git" directory.
//
// This (obviously) might take a while, so consider whether the result should be cached.
CODA_OSS_API std::filesystem::path findFirstFile(const std::filesystem::path& startingDirectory, const std::filesystem::path& filename);
std::filesystem::path findFirstDirectory(const std::filesystem::path& startingDirectory, const std::filesystem::path& dir);

// This is here most to avoid creating a new module for a few utility routines
namespace test // i.e., sys::test
{
    // Try to find the specified "root" directory starting at the given path.
    // Used by unittest to find sample files.
    CODA_OSS_API std::filesystem::path findRootDirectory(const std::filesystem::path& p, const std::string& rootName,
        std::function<bool(const std::filesystem::path&)> isRoot);

    CODA_OSS_API std::filesystem::path findCMakeBuildRoot(const std::filesystem::path& p);
    bool CODA_OSS_API isCMakeBuild(const std::filesystem::path& p);

    std::filesystem::path findCMakeInstallRoot(const std::filesystem::path& p);
    bool isCMakeInstall(const std::filesystem::path& p);

    // Walk up the directory tree until a .git/ directory is found
    std::filesystem::path find_dotGITDirectory(const std::filesystem::path& p);

    // Starting at "root", find the file: root / modulePath / file
    // If that's not found, insert other "known locations" between "root" and "modulePath"
    // e.g., root / "externals" / [name] / path / file
    //
    // Once modulePath is found, the result is cached to avoid searching again.
    std::filesystem::path findModuleFile(
            const std::filesystem::path& root,
            const std::string& externalsName, const std::filesystem::path& modulePath, const std::filesystem::path& moduleFile);
    CODA_OSS_API std::filesystem::path findGITModuleFile(  // use current_directory() to find_dotGITDirectory()
            const std::string& externalsName, const std::filesystem::path& modulePath, const std::filesystem::path& moduleFile);
}
}
#endif
