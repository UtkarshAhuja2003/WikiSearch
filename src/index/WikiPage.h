#ifndef WikiPage_H
#define WikiPage_H

class WikiPage
{
    private:
        std::string pageId = "";
        std::string pageTitle = "";
        std::string pageText = "";

    public:
        void clear();

        std::string getPageId() const;
        void setPageId(const std::string& id);

        std::string getPageTitle() const;
        void setPageTitle(const std::string& title);

        std::string& getPageText();
        void setPageText(const std::string& text);
};

inline void WikiPage::clear()
{
    pageId = "";
    pageTitle = "";
    pageText = "";
}

inline std::string WikiPage::getPageId() const {
    return pageId;
}

inline void WikiPage::setPageId(const std::string& id) {
    this->pageId = id;
}

inline std::string WikiPage::getPageTitle() const {
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


#endif
