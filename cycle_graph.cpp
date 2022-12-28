#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

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

std::vector<unsigned int> gen_unint(const std::vector<std::string> &words){

    /**
     * Wrapper around the word coverter to a positive integer
     *
     * @param words vector with all words, alphabetically sorted
     */

    std::vector<unsigned int> unints(words.size());

    for (unsigned int i=0; i < words.size(); ++i){
        unints[i] = word_as_uint(words[i]);
    }

    return unints;
}

std::map<unsigned int, std::vector<unsigned int>> gen_adj_map(
    const std::vector<unsigned int> &wbits
    ){

    /**
     * Generate an adjacency list as a mapping between the
     * word repsented as an integer - see word_as_uint
     * and a vector with other words without overlapping letters
     * (neighbors) also represented as integers
     * 
     * @param wbits vector of integer representations of all words
     */
    
    std::map<unsigned int, std::vector<unsigned int>> adj_map;

    for(unsigned int idx1=0; idx1<wbits.size(); ++idx1){
        adj_map[idx1] = {}; //initilize the map with empty list
        for(unsigned int idx2=0; idx2<wbits.size(); ++idx2){
            if(((wbits[idx1] & wbits[idx2]) == 0) & (idx2 > idx1)){
                adj_map[idx1].push_back(idx2);
            }
        }
    }
    return adj_map;
};

void findcliques(
    const unsigned int& idx,
    const std::map<unsigned int, std::vector<unsigned int>>& adj_map, 
    const std::vector<unsigned int>& wuints,
    const unsigned int& seenletters,
    std::vector<unsigned int>& clique,
    unsigned int foundwords,
    std::vector<std::vector<unsigned int>>& results
    ){

    /**
     * Recursive function to build cliques of size NWORDS
     * and store them in the results container
     * 
     * @param idx index of the current word considered for the clique
     * @param adj_map the adjancy lists stored as a map
     * @param wuints vector of integer representations of all words
     * @param seenletters integer representation of all seen letters in the words in the clique
     * @param clique container for the indeces of the words in the clique
     * @param foundwords counter of the words currently added to the clique
     * @param results container for all discovered cliques
     */

    if(foundwords==NWORDS){
        results.push_back(clique);
        return;
    }

    for(const unsigned int& idx : adj_map.at(idx)){
        if((seenletters & wuints.at(idx)) == 0){
            clique[foundwords] = idx;
            findcliques(
                idx,
                adj_map,
                wuints,
                (seenletters | wuints.at(idx)),
                clique,
                foundwords+1,
                results
            );
        }
    }
}

std::vector<std::string> read_wordlist(std::string fname){
    
    /**
     * Reads the initial file with all allowed words in ALPHABETical order
     * We keep only WORDLEN letter words with distinct letters
     *
     * @param fname filename
     */
    
    std::ifstream file(fname);
    std::vector<std::string> words;
    
   	while (!file.eof()) {
		std::string word;
		file >> word;

		if (word.length() != WORDLEN) continue;
        if (!all_distinct_letters(word)) continue;

		words.push_back(word);
	}

    file.close();

    return words;
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

    if (!(stat (words_file.c_str(), &buffer) == 0)) {
        std::vector<std::string> words = read_wordlist(complete_file);
        write_wordlist(words_file, words);
    }

    std::vector<std::string> words = read_wordlist(words_file);
    std::vector<unsigned int> wunins = gen_unint(words);

    if(DEBUG){
        std::cout << "Building adjacency list" << "\n";
    }

    std::map<unsigned int, std::vector<unsigned int>> adj_map = gen_adj_map(wunins);
    
    if(DEBUG){
        std::cout << "Building cliques" << "\n";
    }

    std::vector<std::vector<unsigned int>> solutions;
    std::vector<unsigned int> clique (NWORDS);

    for(unsigned int idxw1=0; idxw1<adj_map.size(); ++idxw1){
        clique[0] = idxw1;
        findcliques(idxw1, adj_map, wunins, wunins[idxw1], clique, 1, solutions);

        if(DEBUG){
            int bar_width {70};
            std::cout << "[";
            unsigned int pos = float(idxw1) / adj_map.size() * bar_width;

            for(unsigned int i=0; i<bar_width; ++i){
                if(i < pos){
                    std::cout << "=";
                }
                else if( i == pos){
                    std::cout << ">";
                }
                else{
                    std::cout << " ";
                }
            }

            std::cout << "]" << int(float(idxw1) / adj_map.size() * 100.0) << " %\r";
            std::cout.flush();
        }
    }

    if(DEBUG){
        std::cout << "Cliques built" << "\n";
        std::cout << solutions.size()<< "\n";
    }

    std::ofstream outfile("words_out.txt");
    
    for(unsigned int i=0; i<solutions.size(); ++i){
        for(unsigned int j=0; j<NWORDS; ++j){
            outfile << words[solutions[i][j]] << " ";
        }
        outfile << "\n";
    }

    outfile.close();

}
