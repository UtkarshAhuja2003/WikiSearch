#ifndef Classifiers_H
#define Classifiers_H

#include <string>
#include <unordered_set>

/**
 * @class Classifiers
 * @brief A class that provides functionality for classifying words during tokenisation.
 */
class Classifiers
{
    private:
        // Contains stemmarised stop words to remove common words
        std::unordered_set<std::string> stopWords = {
        "yourself", "your", "you'll", "you", "word", "won't", "wont", "without", "within", "with",
        "wish", "will", "will", "why", "whose", "whomev", "who'll", "whole", "whoever", "whim", "your",
        "while", "whether", "whereupon", "where'", "wherein", "wherea", "whereaft", "when'", "whenev",
        "what'", "what'll", "what", "we'v", "we'll", "welcom", "we'd", "we", "wai", "wa", "want", "via",
        "veri", "which", "variou", "usual", "us", "us", "usefulli", "us", "up", "up", "until", "unto",
        "unlik", "unlik", "unless", "wonder", "unfortun", "two", "twice", "just", "should", "appar",
        "anyon", "befor", "is", "import", "instead", "seriou", "indic", "avail", "serious", "sorri",
        "inde", "isn't", "you'r", "needn't", "their", "howev", "hopefulli", "thanx", "would", "her",
        "hereupon", "concern", "noth", "here", "onli", "henc", "want", "follow", "when", "that", "help",
        "often", "hasn't", "hardli", "go", "given", "further", "front", "signific", "later", "side",
        "give", "exactli", "fill", "accordingli", "except", "exampl", "who", "actual", "get", "someon",
        "have", "he", "had", "same", "everyon", "somebodi", "weren't", "greet", "enough", "gave", "empti",
        "do", "forth", "might", "show", "promptli", "end", "elsewher", "mere", "i'll", "downward", "describ",
        "down", "don't", "again", "nearli", "follow", "differ", "follow", "the", "chang", "he'd", "did",
        "somehow", "doe", "can't", "ow", "alon", "let'", "there'l", "aris", "much", "beyond", "immedi",
        "seem", "caus", "ar", "between", "she", "here", "otherwis", "result", "ani", "moreov", "becam",
        "our", "describ", "aren't", "amongst", "possibl", "herself", "out", "allow", "em", "apart", "i'm",
        "make", "anywher", "thu", "http", "goe", "few", "anymor", "act", "it", "awai", "get", "against",
        "c'mon", "nobodi", "everi", "alreadi", "neither", "and", "entir", "right", "anyhow", "but", "becom",
        "give", "cant", "affect", "end", "ain't", "inward", "her", "never", "near", "affect", "mainli",
        "asid", "consid", "call", "in", "mostli", "itself", "must", "either", "ref", "sai", "self", "us",
        "almost", "anoth", "he", "presum", "begin", "ad", "everywher", "mr", "previous", "furthermor", "becom",
        "abl", "across", "certain", "begin", "sure", "consid", "second", "due", "upon", "approxim", "i'v",
        "meanwhil", "abov", "also", "into", "haven't", "mustn't", "place", "thoroughli", "get", "wherev",
        "etc", "begin", "am", "soon", "should'v", "go", "somewher", "after", "how", "along", "potenti",
        "definit", "sai", "amount", "from", "himself", "former", "she'll", "for", "besid", "went", "announc",
        "com", "respect", "accord", "briefli", "indic", "there", "ask", "begin", "forti", "els", "both",
        "around", "myself", "wasn't", "associ", "find", "shall", "at", "whatev", "no", "it'", "accord",
        "although", "current", "well", "give", "seem", "awfulli", "believ", "back", "be", "he'", "they'r",
        "therein", "becaus", "tri", "becom", "present", "won", "top", "certainli", "been", "everyth", "do",
        "indic", "about", "it'll", "most", "noon", "besid", "best", "hereaft", "us", "that'v", "result",
        "though", "better", "happen", "detail", "him", "that'll", "here'", "there'v", "pleas", "brief",
        "own", "effect", "especi", "take", "as", "came", "have", "can", "move", "cannot", "behind", "caus",
        "correspond", "all", "mill", "beforehand", "on", "viz", "anywai", "ref", "herein", "come", "hello",
        "anywai", "that'", "alwai", "sometim", "where", "we'r", "tell", "consequ", "name", "contain", "known",
        "sub", "contain", "of", "inc", "appear", "strongli", "still", "got", "could", "readili", "me", "tri",
        "couldn't", "sixti", "cours", "hi", "each", "even", "an", "despit", "thereupon", "try", "keep",
        "thank", "yet", "outsid", "thei", "kept", "less", "specifi", "anybodi", "know", "proud", "doesn't",
        "know", "last", "late", "latter", "keep", "next", "wouldn't", "ran", "latterli", "let", "like",
        "like", "import", "saw", "toward", "immedi", "like", "line", "yourselv", "clearli", "look", "done",
        "thereaft", "gone", "look", "slightli", "look", "made", "make", "mani", "were", "mai", "mayb", "she'",
        "mean", "seem", "mean", "specifi", "who'", "meantim", "afterward", "mightn't", "million", "gotten",
        "onc", "selv", "mine", "it", "result", "more", "anyth", "stop", "mr", "contain", "nevertheless",
        "allow", "howbeit", "possibl", "suffici", "ms", "my", "formerli", "name", "us", "significantli",
        "thank", "necessarili", "wherebi", "necessari", "need", "new", "none", "nonetheless", "www", "they'd",
        "nor", "sent", "found", "normal", "not", "too", "predominantli", "ever", "take", "appreci", "nowher",
        "miss", "obtain", "obtain", "whom", "obvious", "off", "ok", "okai", "omit", "you'd", "on", "tend",
        "bottom", "on", "onto", "come", "reason", "other", "other", "our", "least", "them", "ourselv", "over",
        "recent", "overal", "those", "part", "particularli", "past", "they'v", "dure", "per", "that", "how'",
        "now", "perhap", "primarili", "themselv", "probabl", "under", "provid", "everybodi", "put", "ought",
        "quickli", "rel", "quit", "rather", "herebi", "realli", "recent", "regardless", "regard", "regard",
        "relat", "there", "said", "sai", "see", "ask", "see", "seem", "he'll", "seen", "sensibl", "taken",
        "couldnt", "sever", "let", "shan't", "thi", "thereof", "below", "she'd", "shouldn't", "full", "show",
        "wide", "their", "show", "shown", "similarli", "togeth", "sinc", "sincer", "hi", "so", "ye", "some",
        "hadn't", "someth", "sometim", "than", "somewhat", "specif", "specifi", "substanti", "note",
        "successfulli", "particular", "date", "such", "thin", "then", "therebi", "didn't", "affect", "therefor",
        "there'", "thereto", "these", "you'v", "theyd", "they'll", "theyr", "similar", "think", "among",
        "thorough", "through", "throughout", "tip", "appropri", "to", "ha", "took", "suggest", "need",
        "toward", "or", "be", "truli", "try"
    };

    public:
        /**
         * Checks if a given word is a stop word.
         *
         * @param word The word to check.
         * @return True if the word is a stop word, false otherwise.
         */
        bool isStopWord(std::string const &word)
        {
            return (stopWords.find(word) == stopWords.end()) ? false : true;
        }
};

#endif
