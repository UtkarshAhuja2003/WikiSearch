#include "Search.h"

std::vector<std::string> Search::getPostingListForSingleTerm(std::string word)
{
    std::vector<std::string> postingList;
    int offset = invertedIndex[word];
    int startChar = word[0] - 'a';
    postingListsBuffer[startChar]->pubseekpos(offset);
    std::string posting;
    char c;
    while((c = postingListsBuffer[startChar]->sbumpc()) != '\n')
    {
        posting.push_back(c);
    }

    std::string index;
    for(char c : posting)
    {
        if(c == ';')
        {
            postingList.emplace_back(index);
            index.clear();
        }
        else{
            index.push_back(c);
        }
    }

    return postingList;
}

// std::vector<std::string> Search::getPostingList(std::string query)
// {
//     std::vector<std::string> postingList;
//     char word[1000] = "";

//     for(char c : query)
//     {
//         if (isalpha(c))
//         {
//             c = tolower(c);
//             strncat(word, &c, 1);
//         }
//         else
//         {
//             this->stemWord(word);
//             std::string data(word);
//             if(data.size() > 1 && !this->classifiers.isStopWord(data))
//             {
//                 std::vector<std::string> t = getPostingListForSingleTerm(data);
//                 postingList.insert(postingList.end(), t.begin(), t.end());
//             }
//             word[0] = '\0';
//         }
//     }
//     return postingList;
// }

void Search::search()
{
    std::string searchQuery;

    FileIO file("../../res");
    file.initialiseDictFiles(std::ios::in);
    file.initialisePostingLists(std::ios::in);
    dictBuffer = file.getDictBuffer();
    postingListsBuffer = file.getPostingListBuffer();

    this->initializeStemmer();
    loadInvertedIndex();
    std::vector<std::string> res = getPostingListForSingleTerm("famili");
    std::cout << "Result: ";
    for(const std::string& item : res)
    {
        std::cout << item << " ";
    }

    // while(searchQuery != "q" || searchQuery != "exit")
    // {
    //     std::cout << "Enter the query to search: ";
    //     std::cin >> searchQuery;
    //     if (searchQuery == "q" || searchQuery == "exit")
    //         break;
        
    //     std::cout << "Search results for query: " << searchQuery << "\n";

    //     std::vector<std::string> results = getPostingList(searchQuery);
    //     for(std::string index : results)
    //     {
    //         std::cout << index << " ";
    //     }
        
    //     std::cout << "\nEnter q or exit to quit search\n";
    // }

    this->freeStemmer();
}

void Search::loadInvertedIndex()
{
    for(int i = 0; i < 14; i++)
    {
        int lineNumber = 0;
        while(dictBuffer[i]->sgetc() != EOF)
        {
            std::string line;
            char c;
            while((c = dictBuffer[i]->sbumpc()) != '\n')
            {
                line.push_back(c);
            }
            std::string word = line.substr(0, line.find(":"));
            if(word != "")
            {
                int offset = std::stoi(line.substr(line.find(":") + 1)) + lineNumber;
                invertedIndex[word] = offset;
            }
            lineNumber++;
        }
    }
}

void Search::initializeStemmer()
{
    this->stemmer = create_stemmer();
}

void Search::stemWord(char word[])
{
    word[stem(this->stemmer, word, strlen(word) - 1) + 1] = '\0';
}

void Search::freeStemmer()
{
    free_stemmer(this->stemmer);
}
