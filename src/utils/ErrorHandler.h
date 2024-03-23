#define ASSERT(x) if(!(x)) throw std::runtime_error("Error in parsing XML");
#define XMLCall(p, x) x;\
    ASSERT(XMLLogCall(p, #x, __FILE__, __LINE__))

/**
 * @brief Logs XML parsing errors and returns whether an error occurred.
 * 
 * @param parser XML parser object.
 * @param functionName name of the function where the error occurred.
 * @param fileName name of the file where the error occurred.
 * @param lineNumber line number where the error occurred.
 * @return true if no error occurred, false otherwise.
 */
static bool XMLLogCall(XML_Parser parser, const char* functionName, const char* fileName, int lineNumber) {
    const XML_Error errorCode =  XML_GetErrorCode(parser);
    if(errorCode == XML_ERROR_NONE) return true;

    const XML_LChar* errorString = XML_ErrorString(errorCode);
    std::cerr << "[XML Error] (" << errorCode << ": " << errorString
              << ") in function " << functionName << " at " << fileName << ":" << lineNumber
              << " (Line: " << lineNumber << ")" << std::endl;
    return false;
}