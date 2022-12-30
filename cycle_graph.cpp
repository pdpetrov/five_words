#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <execution>
#include <algorithm>

#define DEBUG false
#define ALPHABET 26
#define WORDLEN 5
#define NWORDS ALPHABET/WORDLEN


bool all_distinct_letters(std::string &w){

    /**
     * Tests if all the letters in a word are distinct
     * 
     * @param w string to be parsed
     */

    bool seen[ALPHABET]{};
    for (int i = 0 ; i != w.length() ; i++) {
        int index = int(w[i]) - int('a');
        if (seen[index]) return false;
        seen[index] = true;
    }
    return true;
}

unsigned int word_as_uint(const std::string &w){

    /**
     * Covert a word to a positive integer
     * the integer is ALPHABET wide bitset representation based on
     * letters used in the word.
     * The representation is alphabetical meaning the letter
     * 'a' is encoded as 000...1
     * 'z' is encoded as 100...0
     *
     * @param w the word
     */
    
    unsigned int num {0};
    for(char l : w){
        num |= (1 << (int(l) - int('a')));
    }
    return num;
}

void gen_uint(const std::vector<std::string> &words, std::vector<unsigned int>& uints){

    /**
     * Wrapper around the word coverter to a positive integer
     *
     * @param words vector with all words, alphabetically sorted
     * @param uints vector where unsigned int representations are stored
     */


    for (unsigned int i=0; i < words.size(); ++i){
        uints[i] = word_as_uint(words[i]);
    }

    return;
}

void gen_adj_map(
    const std::vector<unsigned int> &wbits,
    std::map<unsigned int, std::vector<unsigned int>>& adj_map
    ){

    /**
     * Generate an adjacency list as a mapping between the
     * word repsented as an integer - see word_as_uint
     * and a vector with other words without overlapping letters
     * (neighbors) also represented as integers
     * 
     * @param wbits vector of integer representations of all words
     * @param adj_map map containing adjacency list
     */
    
    for(unsigned int idx1=0; idx1<wbits.size(); ++idx1){
        adj_map[idx1] = {}; //initilize the map with empty list
        for(unsigned int idx2=0; idx2<wbits.size(); ++idx2){
            if(((wbits[idx1] & wbits[idx2]) == 0) & (idx2 > idx1)){
                adj_map[idx1].push_back(idx2);
            }
        }
    }
    return;
};

void findcliques(
    const unsigned int& idx,
    const std::vector<std::string>& words,
    const std::vector<unsigned int>& wuints,
    const std::map<unsigned int, std::vector<unsigned int>>& adj_map,
    const unsigned int& seenletters,
    std::vector<unsigned int>& clique,
    unsigned int foundwords,
    std::ofstream& outfile
    ){

    /**
     * Recursive function to build cliques of size NWORDS
     * and store them in the results container
     * 
     * @param idx index of the current word considered for the clique
     * @param words vector of all words
     * @param wuints vector of integer representations of all words
     * @param adj_map the adjancy lists stored as a map
     * @param seenletters integer representation of all seen letters in the words in the clique
     * @param clique container for the indeces of the words in the clique
     * @param foundwords counter of the words currently added to the clique
     * @param outfile file to output all discovered cliques
     */


    if(foundwords==NWORDS){
        for(unsigned int i=0; i<NWORDS; ++i){
            outfile << words[clique[i]] << " ";
        }
        outfile << "\n";
        return;
    }

    for(const unsigned int& idx : adj_map.at(idx)){
        if((seenletters & wuints.at(idx)) == 0){
            clique[foundwords] = idx;
            findcliques(
                idx,
                words,
                wuints,
                adj_map,
                (seenletters | wuints.at(idx)),
                clique,
                foundwords+1,
                outfile
            );
        }
    }
}

void read_wordlist(std::string fname, std::vector<std::string>& words){
    
    /**
     * Reads the initial file with all allowed words in alphabeical order
     * We keep only WORDLEN letter words with distinct letters
     *
     * @param fname filename
     * @param words vector where all words are stored
     */
    
    std::ifstream file(fname);
    
   	while (!file.eof()) {
		std::string word;
		file >> word;

		if (word.length() != WORDLEN) continue;
        if (!all_distinct_letters(word)) continue;

		words.push_back(word);
	}

    file.close();

    return;
}

void write_wordlist(std::string fname, const std::vector<std::string> &words){

    /**
     * Writes words to file. One word per line.
     *
     * @param fname filename
     * @param words vector of words to write
     */

    std::ofstream file(fname);
    
    for(std::string w : words){
        file << w << "\n";
    }

    file.close();

}


int main(){
    
    struct stat buffer;   
    std::string complete_file = "words_alpha.txt";
    std::string words_file = "words_beta.txt";

    std::vector<std::string> words;

    if (!(stat (words_file.c_str(), &buffer) == 0)) {
        read_wordlist(complete_file, words);
        write_wordlist(words_file, words);
    }

    read_wordlist(words_file, words);
    std::vector<unsigned int> uints(words.size());
    gen_uint(words, uints);
    std::vector<unsigned int> idxwords(words.size());
    std::iota(idxwords.begin(), idxwords.end(), 0);

    std::map<unsigned int, std::vector<unsigned int>> adj_map;
    gen_adj_map(uints, adj_map);
    
    std::ofstream outfile("words_out.txt");

    std::for_each(
        std::execution::par,
        idxwords.begin(),
        idxwords.end(),
        [&words, &uints, &adj_map, &outfile](unsigned int idx) {
            std::vector<unsigned int> clique (NWORDS);
            clique[0] = idx;
            findcliques(idx, words, uints, adj_map, uints.at(idx), clique, 1, outfile);
        } 
    );

    outfile.close();

}
