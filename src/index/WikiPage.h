#ifndef WikiPage_H
#define WikiPage_H

#include <string>

class WikiPage
{
    private:
        std::string pageId = "";
        std::string pageTitle = "";
        std::string pageText = "";

    public:
        WikiPage(const std::string& id);
        ~WikiPage();

        std::string getPageId() const;
        void setPageId(const std::string& id);

        std::string getPageTitle() const;
        void setPageTitle(const std::string& title);

        std::string getPageText() const;
        void setPageText(const std::string& text);
};

WikiPage::WikiPage(const std::string& id)
{
    this->pageId = id;
}

WikiPage::~WikiPage()
{
    this->pageId = "";
    this->pageText = "";
    this->pageTitle = "";
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

inline std::string WikiPage::getPageText() const {
    return pageText;
}

inline void WikiPage::setPageText(const std::string& text) {
    this->pageText = text;
}


#endif
