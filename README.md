
# WikiSearch

A fast and efficient search engine built with C++ using Wikipedia Dump data. Optimized for quick and accurate information retrieval.

Wikipedia Dump (90 GB) - : http://dumps.wikimedia.org/enwiki/latest/enwiki-latest-pages-articles.xml.bz2

## Statistics

- For 90 GB of data Wiki XML Dump :
   + Size of index ( primary+secondary ) : 9.12 GB
   + Size of Metadata : 863 MB
   + Time to index : 3hr 30min (average)
   + Time to search : 0.34 sec (average on 100 searches)
## Features

 - User search system for faster information retrieval
 - Web-based interface
 - Direct links to actual Wikipedia pages
 - Stemming for improved search accuracy

## Screenshots
![image](https://github.com/UtkarshAhuja2003/WikiSearch/assets/70762626/b4cd5f00-9cb4-4512-b156-dc8942e44a8e)

![image](https://github.com/UtkarshAhuja2003/WikiSearch/assets/70762626/cfe4b65f-4e8f-418c-bfcf-65a120a9b1ea)


## Installation

To install and run this project, follow these steps:

1. **Clone the repository:**

   ```sh
   git clone https://github.com/UtkarshAhuja2003/WikiSearch.git
   cd WikiSearch
   ```
2. **Create a build directory:**

    ```sh
   mkdir build
   cd build
   ```
3. **Generate the build files with CMake:**

    ```sh
    cmake ..
   ```
4. **Build the project:**

    ```sh
    make
   ```
5. **Run the application:**

    ```sh
    ./WikiSearch
   ```


## File Structure

```bash
├──.github # Github actions workflow
├──build # Build files for the project
├──client # Web frontend
├──dependencies
├──res # Posting List, Metadata, WikiDump
└──src # Source code
    ├── index # Parse Wikipedia Data
    ├── search
    └──utils # File Management, Stemming and Classifiers
```
