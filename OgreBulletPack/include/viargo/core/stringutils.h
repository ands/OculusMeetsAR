
#ifndef VIARGO_STRINGUTILS_H
#define VIARGO_STRINGUTILS_H

#include <vector>
#include <string>

#include "core/vrgmath.h"

namespace viargo{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class StringUtils{

public:
    ///converts the string to integer
    static int stringToInt(std::string in);

    ///converts an integer to string
    static std::string intToString(int in);

    ///converts the string to float
    static float stringToFloat(std::string in);

    /// converts the input string to lower case
    /// <example> toLower("QwErTy") => "qwerty"
    static std::string toLower(std::string in);

    /// converts the input string to upper case
    /// <example> toUpper("QwErTy") => "QWERTY"
    static std::string toUpper(std::string in);

    /// leaves only the characters from filterChar in the result string
    /// <example> filter("QwErTy", "QWERTY") => "QET"
    static std::string filter(std::string in, std::string filterChar);
    
    /// removes the characters from dropChar from the input string
    /// <example> drop("QwErTy", "QWERTY") => "wry"
    static std::string drop(std::string in, std::string dropChars);

    /// returns a substring from in[0] to first occurance of one of the 
    /// characters in separator-string
    /// <example> splitHead("QwErTy", "rT") => "QwEr"
    static std::string splitHead(std::string in, std::string separator);

    /// returns a substring from first occurance of one of the 
    /// characters in separator-string to the end of the string
    /// <example> dropHead("QwErTy", "rT") => "Ty"
    static std::string dropHead (std::string in, std::string separator);
    
    /// removes dropChars from the input string until a character not in 
    /// dropChars is found
    /// <example> dropFirst("QwErTz", "QqWwEe") => "rTz"
    static std::string dropFirst(std::string in, std::string dropChars);

private:
    StringUtils(){} // disable creation
    StringUtils(StringUtils& ){} // disable copy 

    inline static int lowerCase(int c){
        return tolower(c);
    }

    inline static int upperCase(int c){
        return toupper(c);
    }
};


} // namespace viargo

#endif // VIARGO_STRINGUTILS_H

