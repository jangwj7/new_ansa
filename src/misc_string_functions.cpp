#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "misc_string_functions.h"
#include <unordered_map>

void misc::trim(std::string& string){
    const auto strBegin = string.find_first_not_of(" ");
    if (strBegin == std::string::npos){
        return;
    }
    const auto strEnd = string.find_last_not_of(" ");
    const auto strRange = strEnd - strBegin + 1;
    string = string.substr(strBegin, strRange);
    return;
};

std::unordered_map<std::string,std::string> misc::options_map(std::string line){
    // returns a map with options as keys and the option values as parameters
    std::unordered_map<std::string, std::string> options;
    // split input line on comma, keep as capital case
    std::vector<std::string> strings = misc::split_on(line,',',false);
    // loop over comma-split string and start creating 
    // key-value pairs
    for (int i = 0; i < strings.size(); i++)
    {
        // try to split on equal sign
        std::vector<std::string> key_and_value = misc::split_on(strings.at(i), '=', false);
        std::string key = key_and_value.at(0);
        // trim leading and trailing whitespace of string
        misc::trim(key);
        try
        {
            std::string value = key_and_value.at(1);
            options[key] = value;
            // std::cout << "key <-> value:" << key << "<->" << value << "\n";
        }
        // TODO: Maybe print something about keyword has no options?
        // But it's not always an issue..
        catch(const std::exception& e)
        {
            // std::cout << "no value found for key " << key << "\n";
        }
        
    }
    return options;
};

void misc::append_newline_to_textfile(std::string filename)
{
    std::ofstream input_file(filename, std::ios::app);
    
}

bool misc::is_keyword(std::string line){
    // If the line starts with an asterix
    // and the second character is an alpha
    // we don't have a keyword!
    // NOTE: compare return 0 if equal, isalpha returns non-zero if true
    if (line.compare(0, 1, "*") == 0 && isalpha(line.at(1))!= 0){
        return true;
    }
    else{    
        return false;
    }
};

bool misc::is_comment(std::string line){
    // NOTE: compare return 0 if equal
    if (line.compare(0, 2, "**") == 0){
        return true;
    }
    else{
    return false;
    };   

};

std::vector<std::string> misc::split_on(std::string in,char split_char,bool make_lower_case)
{
    std::stringstream ss(in);
    std::vector<std::string> items;
    while (ss.good())
    {
        std::string item;
        getline(ss, item, split_char);
        // std::cout << item << "\n";
        // make lower case!
        if (make_lower_case == true){
            item = misc::to_lower(item);
        }; 
        items.push_back(item);
    };
    return items;
};



std::string misc::to_lower(std::string text)
{
    // return a string in all lower case
    std::for_each(text.begin(), text.end(), [](char &c) {
        c = std::tolower(c);
    });
    return text;
};