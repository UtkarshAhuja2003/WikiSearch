#ifndef WikiPage_H
#define WikiPage_H

/**
 * @class WikiPage
 * 
 * @brief Represents a WikiPage with ID, Title and text
 */
class WikiPage
{
    private:
        std::string pageId = ""; // The unique identifier of the wiki page.
        std::string pageTitle = ""; // The title of the wiki page.
        std::string pageText = ""; // The content of the wiki page.
        bool idParsed = false; // If the DocID is parsed in wikiPage

    public:
        /**
         * Clears the content of the wiki page.
         */
        void clear();

        /**
         * Retrieves the page ID of the wiki page.
         * 
         * @return The page ID.
         */
        std::string& getPageId();

        /**
         * Sets the page ID of the wiki page.
         * 
         * @param id The page ID to set.
         */
        void setPageId(const std::string& id);

        /**
         * Retrieves the page title of the wiki page.
         * 
         * @return The page title.
         */
        std::string& getPageTitle();

        /**
         * Sets the page title of the wiki page.
         * 
         * @param title The page title to set.
         */
        void setPageTitle(const std::string& title);

        /**
         * Retrieves the page text of the wiki page.
         * 
         * @return A reference to the page text.
         */
        std::string& getPageText();

        /**
         * Sets the page text of the wiki page.
         * 
         * @param text The page text to set.
         */
        void setPageText(const std::string& text);

        /**
         * Retrieves boolean, if parsed ID->true.
         * 
         * @return A reference to the parsedID.
         */
        bool& isIDParsed();

        /**
         * Sets the parsed ID boolean.
         * 
         * @param idParsed Boolean, if parsed ID->true
         */
        void setIDParsed(bool idParsed);
};

inline void WikiPage::clear()
{
    pageId = "";
    pageTitle = "";
    pageText = "";
    idParsed = false;
}

inline std::string& WikiPage::getPageId() {
    return pageId;
}

inline void WikiPage::setPageId(const std::string& id) {
    this->pageId = id;
}

inline std::string& WikiPage::getPageTitle() {
    return pageTitle;
}

inline void WikiPage::setPageTitle(const std::string& title) {
    this->pageTitle = title;
}

inline std::string& WikiPage::getPageText() {
    return pageText;
}

inline void WikiPage::setPageText(const std::string& text) {
    this->pageText = text;
}

inline bool& WikiPage::isIDParsed() {
    return idParsed;
}

inline void WikiPage::setIDParsed(bool idParsed) {
    this->idParsed = idParsed;
}

#endif
