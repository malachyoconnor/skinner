#ifndef SKINNER_FLAG_H
#define SKINNER_FLAG_H
#include <map>
#include <string>
#include <vector>

#include "utils.h"

namespace flag {
   using std::string;

   enum flagTypes {
      Int,
      String
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
   inline std::vector<flagToParse<string> > stringFlagsToParse{};

   inline int *intFlag(string &flagName, int defaultValue, string &helpMessage) {
      intFlagsToParse.emplace_back(flagName, defaultValue, helpMessage);
      return intFlagsToParse.back().parsedValue;
   }

   inline string *stringFlag(string flagName, string defaultValue, string helpMessage) {
      stringFlagsToParse.emplace_back(flagName, defaultValue, helpMessage);
      return stringFlagsToParse.back().parsedValue;
   }

   // -flag   NOT IMP
   // --flag  NOT IMP
   // -flag=x
   // -flag x NOT IMP

   inline void parse(int argc, char *argv[]) {
      std::map<string, string> name_value_map {};

      for (int argNum = 1; argNum < argc; ++argNum) {
         string argument = string(argv[argNum]);

         if (argument.starts_with("--"))
            assert(false && "TODO: Boolean flags");

         if (!argument.starts_with("-")) {
            PRINT("Incorrect formatting of arguments\n", RED);
            exit(1);
         }

         string name = argument.substr(1, argument.length() - 1);

         if (argument.find('=') == string::npos)
            assert(false && "TODO: Boolean flags");

         string value = argument.substr(argument.find('=') + 1);

         if (name_value_map.find(name) != name_value_map.end()) {
            PRINT("Flag entered twice: ", RED);
            printf("%s\n", name.c_str());
            return;
         }

         name_value_map[name] = value;
      }

      for (auto& flag : intFlagsToParse) {
         if (name_value_map.find(flag.name) == name_value_map.end()) continue;

         string unparsed_value = name_value_map.find(flag.name);
         *flag.parsedValue = atoi(unparsed_value.c_str());
      }

   }
}
#endif //SKINNER_FLAG_H
