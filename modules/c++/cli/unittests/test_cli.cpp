/* =========================================================================
 * This file is part of cli-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cli-c++ is free software; you can redistribute it and/or modify
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

#include <stdio.h>

#include <sstream>
#include <fstream>
#include <span>

#include <import/cli.h>
#include <import/mem.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("testValue")
{
    cli::Value v("data");
    CHECK("data" == v.get<std::string>());

    v.set(3.14f);
    CHECK_THAT(3.14f, Catch::Matchers::WithinRel(v.get<float>(), std::numeric_limits<float>::epsilon()));
    CHECK(3 == v.get<int>());

    std::vector<float> floats;
    std::vector<std::string> strings;
    for(int i = 0; i < 10; ++i)
    {
        floats.push_back(10.0f * i);
        strings.push_back(std::to_string(i));
    }

    // floats
    v.setContainer(floats);
    for(int i = 0; i < 10; ++i)
    {
        CHECK_THAT(v.at<float>(i), Catch::Matchers::WithinRel(10.0f * i, std::numeric_limits<float>::epsilon()));
    }
    CHECK(std::ssize(v) == 10);

    // strings
    v.setContainer(strings);
    for(int i = 0; i < 10; ++i)
    {
        CHECK(v.at<std::string>(i) == std::to_string(i));
    }
    CHECK(std::ssize(v) == 10);
}

TEST_CASE("testChoices")
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser.addArgument("-t --type", "Specify a type to use", cli::STORE)->addChoice(
            "type1")->addChoice("type2")->addChoice("type3");
    parser.addArgument("-m --many", "Specify a type to use", cli::STORE, "choices", "CHOICES", 0)->addChoice(
            "type1")->addChoice("type2")->addChoice("type3");
    parser.addArgument("images", "Input images", cli::STORE);
    parser.setDescription("This program is kind of pointless, but have fun!");
    parser.setProlog("========= (c) COPYRIGHT BANNER ========= ");
    parser.setEpilog("And that's the usage of the program!");
    std::ostringstream buf;
    parser.printHelp(buf);

    std::unique_ptr<cli::Results> results(parser.parse(str::split("-v", " ")));
    CHECK(results->hasValue("verbose"));
    CHECK(results->get<bool>("verbose", 0));

    results.reset(parser.parse(str::split("", " ")));
    CHECK_FALSE(results->get<bool>("verbose", 0));

    results.reset(parser.parse(str::split("-t type2", " ")));
    CHECK(results->get<std::string>("type", 0) == std::string("type2"));

    CHECK_THROWS_AS(results.reset(parser.parse(str::split("-t type2 -t type1", " "))), except::Exception);

    results.reset(parser.parse(str::split("-t type2", " ")));

    results.reset(parser.parse(str::split("-m type2 --many type1 -m type3", " ")));
}

TEST_CASE("testMultiple")
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose --loud -l", "Toggle verbose", cli::STORE_TRUE);

    std::unique_ptr<cli::Results> results(parser.parse(str::split("-v")));
    CHECK(results->hasValue("verbose"));
    CHECK(results->get<bool>("verbose"));

    results.reset(parser.parse(str::split("-l")));
    CHECK(results->get<bool>("verbose"));
    results.reset(parser.parse(str::split("--loud")));
    CHECK(results->get<bool>("verbose"));
    results.reset(parser.parse(str::split("")));
    CHECK_FALSE(results->get<bool>("verbose"));
}

TEST_CASE("testSubOptions")
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser.addArgument("-c --config", "Specify a config file", cli::STORE);
    parser.addArgument("-x --extra", "Extra options", cli::SUB_OPTIONS);
    parser.addArgument("-c --config", "Config options", cli::SUB_OPTIONS);
    std::ostringstream buf;
    parser.printHelp(buf);

    std::unique_ptr<cli::Results> results(parser.parse(str::split("-x:special")));
    CHECK(results->hasSubResults("extra"));
    CHECK(results->getSubResults("extra")->get<bool>("special"));

    results.reset(parser.parse(str::split("--extra:arg=something -x:arg2 1")));
    CHECK(results->hasSubResults("extra"));
    CHECK(results->getSubResults("extra")->get<std::string>("arg") == "something");
    CHECK(results->getSubResults("extra")->get<int>("arg2") == 1);

    results.reset(parser.parse(str::split("--config /path/to/file --config:flag1 -c:flag2=true --config:flag3 false")));
    CHECK(results->get<std::string>("config") == "/path/to/file");
    CHECK(results->hasSubResults("config"));
    CHECK(results->getSubResults("config")->get<bool>("flag1"));
    CHECK(results->getSubResults("config")->get<bool>("flag2"));
    CHECK_FALSE(results->getSubResults("config")->get<bool>("flag3"));
}

TEST_CASE("testIterate")
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser.addArgument("-c --config", "Specify a config file", cli::STORE);

    std::unique_ptr<cli::Results>
            results(parser.parse(str::split("-v -c config.xml")));
    std::vector<std::string> keys;
    for(auto it = results->begin(); it != results->end(); ++it)
        keys.push_back(it->first);
    CHECK(std::ssize(keys) == 2);
    // std::map returns keys in alphabetical order...
    CHECK(keys[0] == "config");
    CHECK(keys[1] == "verbose");
}

TEST_CASE("testRequired")
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-c --config", "Specify a config file", cli::STORE)->setRequired(true);

    const std::string program("test");
    const auto results = parser.parse(program, str::split("-c configFile"));
    CHECK(results->get<std::string>("config") == "configFile");
}

TEST_CASE("testRequiredThrows")
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-c --config", "Specify a config file", cli::STORE)
            ->setRequired(true);

    const std::string program("test");
    CHECK_THROWS(parser.parse(program, str::split("")));
    CHECK_THROWS(parser.parse(program, str::split("-c")));
}

TEST_CASE("testUnknownArgumentsOptions")
{
    std::ostringstream outStream;
    cli::ArgumentParser parser(true, &outStream);
    parser.setProgram("tester");
    parser.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser.addArgument("-x --extra", "Extra options", cli::SUB_OPTIONS);

    // Use a flag that is incorrect
    std::unique_ptr<cli::Results> results(parser.parse(str::split("-z")));
    CHECK_FALSE(results->get<bool>("verbose"));
    CHECK(outStream.str() == std::string("Unknown arg: -z\n"));

    std::ostringstream outStream2;
    parser.setIgnoreUnknownArgumentsOutputStream(&outStream2);
    results.reset(parser.parse(str::split("-z")));
    CHECK_FALSE(results->get<bool>("verbose"));
    CHECK(outStream2.str() == std::string("Unknown arg: -z\n"));

    // Test a file
    std::string testFilename = "test_failed_parser_arg.log";
    std::ofstream outFStream(testFilename);
    parser.setIgnoreUnknownArgumentsOutputStream(&outFStream);
    results.reset(parser.parse(str::split("-z")));
    outFStream.close();
    // Open the file and make sure it has the appropriate line
    std::ifstream inFStream(testFilename);
    std::string line;
    if (inFStream.is_open())
    {
        std::getline(inFStream, line);
        CHECK(line.compare("Unknown arg: -z") == 0);
    }
    // Close the stream and remove the file.
    inFStream.close();
    if (remove(testFilename.c_str()) != 0)
    {
        std::cerr << "Error deleting file: " << testFilename << std::endl;
    }

    // Test setting flag
    parser.setIgnoreUnknownArgumentsFlag(false);
    CHECK_THROWS(results.reset(parser.parse(str::split("-z"))));

    // Test default with more complex arguments
    cli::ArgumentParser parser2;
    parser2.setProgram("tester");
    parser2.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    CHECK_THROWS(results.reset(parser2.parse(str::split("-f C:/Data/File.txt"))));

    // Test ignoreUnknownArguments with more complex arguments
    std::ostringstream outStream3;
    cli::ArgumentParser parser3(true, &outStream3);
    parser3.setProgram("tester");
    parser3.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser3.addArgument("-c --config", "Specify a config file", cli::STORE);
    parser3.addArgument("-t --type", "Type", cli::STORE_TRUE);
    results.reset(parser3.parse(str::split("-v --badarg1 -c config.txt --filename=file.txt -z")));
    CHECK(results->get<bool>("verbose"));
    CHECK_FALSE(results->get<bool>("type"));
    CHECK(results->get<std::string>("config") == "config.txt");
    CHECK(outStream3.str() == std::string("Unknown arg: --badarg1\nUnknown arg: --filename\nUnknown arg: -z\n"));
    CHECK(results->get<bool>("verbose"));
    CHECK_FALSE(results->get<bool>("type"));
    CHECK(results->get<std::string>("config") == "config.txt");
}

