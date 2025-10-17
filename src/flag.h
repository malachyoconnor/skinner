#ifndef SKINNER_FLAG_H
#define SKINNER_FLAG_H
#include <map>
#include <string>
#include <vector>

#include "utils.h"

// --flag
// -flag
// -flag=x
// -flag x   !NOT IMP

namespace flag {
   using std::string;

   enum flagTypes {
      Int,
      String,
      Boolean,
      Double
   };

   template<typename T>
   struct flagToParse {
      flagToParse() = delete;

      flagToParse(string &name, T defaultValue, string &helpMessage)
         : name(name), defaultValue(defaultValue), helpMessage(helpMessage) {
         parsedValue = new T(defaultValue);
      }

      string name;
      T defaultValue;
      string helpMessage;

      T *parsedValue;
   };

   inline std::vector<flagToParse<int> > intFlagsToParse{};
   inline std::vector<flagToParse<double> > doubleFlagsToParse{};
   inline std::vector<flagToParse<string> > stringFlagsToParse{};
   inline std::vector<flagToParse<bool> > booleanFlagsToParse{};

   inline int *intFlag(string flagName, int defaultValue, string helpMessage) {
      intFlagsToParse.emplace_back(flagName, defaultValue, helpMessage);
      return intFlagsToParse.back().parsedValue;
   }

   inline double *doubleFlag(string flagName, double defaultValue, string helpMessage) {
      doubleFlagsToParse.emplace_back(flagName, defaultValue, helpMessage);
      return doubleFlagsToParse.back().parsedValue;
   }

   inline string *stringFlag(string flagName, string defaultValue, string helpMessage) {
      stringFlagsToParse.emplace_back(flagName, defaultValue, helpMessage);
      return stringFlagsToParse.back().parsedValue;
   }

   inline bool *booleanFlag(string flagName, bool defaultValue, string helpMessage) {
      booleanFlagsToParse.emplace_back(flagName, defaultValue, helpMessage);
      return booleanFlagsToParse.back().parsedValue;
   }

   void populateNameValueMap(int argc, const char *argv[], std::map<string, string> &name_value_map);

   void outputHelpMessageAndExit();

   inline void parseFlags(int argc, const char *argv[]) {
      std::map<string, string> name_value_map{};

      populateNameValueMap(argc, argv, name_value_map);

      for (auto &flag: intFlagsToParse) {
         if (name_value_map.find(flag.name) == name_value_map.end()) continue;

         string unparsed_value = name_value_map[flag.name];
         *flag.parsedValue = std::stoi(unparsed_value);
      }
      intFlagsToParse.clear();

      for (auto &flag: doubleFlagsToParse) {
         if (name_value_map.find(flag.name) == name_value_map.end()) continue;

         string unparsed_value = name_value_map[flag.name];
         *flag.parsedValue = std::stod(unparsed_value);
      }
      doubleFlagsToParse.clear();

      for (auto &flag: stringFlagsToParse) {
         if (name_value_map.find(flag.name) == name_value_map.end()) continue;

         *flag.parsedValue = name_value_map[flag.name];
      }
      stringFlagsToParse.clear();

      for (auto &flag: booleanFlagsToParse) {
         if (name_value_map.find(flag.name) == name_value_map.end()) continue;

         *flag.parsedValue = true;
      }
      stringFlagsToParse.clear();
   }

   inline void populateNameValueMap(int argc, const char *argv[], std::map<string, string> &name_value_map) {
      for (int argNum = 1; argNum < argc; ++argNum) {
         string argument = string(argv[argNum]);

         if (!argument.starts_with("-")) {
            continue;
         }

         if (argument.starts_with("--")) {
            string name = argument.substr(2, argument.length());
            if (name == "help") outputHelpMessageAndExit();
            name_value_map[name] = "";
            continue;
         }

         if (argument.find('=') == string::npos) {
            string name = argument.substr(1, argument.length());
            name_value_map[name] = "";
            continue;
         }

         string name = argument.substr(1, argument.find('=') - 1);
         string value = argument.substr(argument.find('=') + 1);

         if (name_value_map.contains(name)) {
            PRINTLN("Flag entered twice: ", RED);
            printf("%s\n", name.c_str());
            continue;
         }

         name_value_map[name] = value;
      }
   }

   inline void outputHelpMessageAndExit() {
      string all_commands = "";
      for (auto &command: COMMAND_LIST) {
         all_commands += command + " | ";
      }
      all_commands.pop_back();
      all_commands.pop_back();
      all_commands.pop_back();

      std::printf("usage: skinner [%s] [flags]:\n", all_commands.c_str());

      if (intFlagsToParse.size() + doubleFlagsToParse.size() +
          stringFlagsToParse.size() + booleanFlagsToParse.size() == 0) {
         exit(EXIT_SUCCESS);
      }

      std::printf("Available flags:\n");

      for (auto &flag: intFlagsToParse) {
         std::printf("   -%s: %s\n", flag.name.c_str(), flag.helpMessage.c_str());
      }
      for (auto &flag: doubleFlagsToParse) {
         std::printf("   -%s: %s\n", flag.name.c_str(), flag.helpMessage.c_str());
      }
      for (auto &flag: stringFlagsToParse) {
         std::printf("   -%s: %s\n", flag.name.c_str(), flag.helpMessage.c_str());
      }
      for (auto &flag: booleanFlagsToParse) {
         std::printf("   -%s OR --%s: %s\n", flag.name.c_str(), flag.name.c_str(), flag.helpMessage.c_str());
      }

      exit(EXIT_SUCCESS);
   }
}
#endif //SKINNER_FLAG_H
