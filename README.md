# WikiSearch

## Stats

### Configuration 1

- **Indexing Method**: In memory indexing with set and map
- **Word Filtering**: Only a-z and A-Z in words
- **Lowercasing**: No
- **Stopwords Removal**: Yes

#### Stats

- **Wiki Dump**: 1 GB
- **Time Taken**: 642 seconds
- **CPU Usage**: 25%
- **Memory Usage**: 2.5 GB

### Configuration 2

- **Indexing Method**: In memory indexing with set and map
- **Word Filtering**: Only a-z and A-Z in words
- **Lowercasing**: No
- **Stopwords Removal**: No

#### Stats

- **Wiki Dump**: 1 GB
- **Time Taken**: 786 seconds
- **CPU Usage**: 25%
- **Memory Usage**: 2.8 GB

### Configuration 3

- **Indexing Method**: In memory indexing with set and map
- **Word Filtering**: Only a-z and A-Z in words
- **Lowercasing**: Yes
- **Stopwords Removal**: Yes

#### Stats

- **Wiki Dump**: 1 GB
- **Time Taken**: 555 seconds
- **CPU Usage**: 25%
- **Memory Usage**: 2.2 GB

### Configuration 4

- **Indexing Method**: In memory indexing with set and map
- **Word Filtering**: Only a-z and A-Z in words
- **Lowercasing**: Yes
- **Stopwords Removal**: Yes
- **Stemmed Words**: Yes

#### Stats

- **Wiki Dump**: 1 GB
- **Time Taken**: 500 seconds
- **CPU Usage**: 25%
- **Memory Usage**: 2 GB

### Configuration 5

- **Indexing Method**: File Buffer, all indexes in single file
- **Word Filtering**: Only a-z and A-Z in words
- **Lowercasing**: Yes
- **Stopwords Removal**: Yes
- **Stemmed Words**: Yes

#### Stats

- **Wiki Dump**: 1 GB
- **Time Taken**: 119 seconds
- **CPU Usage**: 25%
- **Index File Size**: 1 GB

### Configuration 6

- **Indexing Method**: File Buffer, 1000 pages per index file
- **Word Filtering**: Only a-z and A-Z in words
- **Lowercasing**: Yes
- **Stopwords Removal**: Yes
- **Stemmed Words**: Yes

#### Stats

- **Wiki Dump**: 1 GB
- **Time Taken**: 414 seconds
- **CPU Usage**: 25%
- **Index File Size**: 1 GB

### Configuration 7

- **Indexing Method**: SPMI using map and set with 1000 pages per file 
- **Word Filtering**: Only a-z and A-Z in words
- **Lowercasing**: Yes
- **Stopwords Removal**: Yes
- **Stemmed Words**: Yes

#### Stats

- **Wiki Dump**: 1 GB
- **Time Taken**: 465 seconds
- **CPU Usage**: 25%
- **Index File Size**: 162 MB