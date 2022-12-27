#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>


#define debug false


bool all_distinct_letters(std::string &w){

    /**
     * Tests if all the letters in a word are distinct
     * 
     * @param w string to be parsed
     */

    bool seen[26]{};
    for (int i = 0 ; i != w.length() ; i++) {
        int index = int(w[i]) - int('a');
        if (seen[index]) return false;
        seen[index] = true;
    }
    return true;
}

unsigned int word_as_unint(const std::string &w){

    /**
     * Covert a word to a positive integer
     * the integer is 26 wide bitset representation based on
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
        unints[i] = word_as_unint(words[i]);
    }

    return unints;
}

std::map<unsigned int, std::vector<unsigned int>> gen_adj_map(
    const std::vector<unsigned int> &wbits
    ){

    /**
     * Generate an adjacency list as a mapping between the
     * word repsented as an integer - see word_as_unint
     * and a vector with other words without overlapping letters
     * (neighbors) also represented as integers
     * 
     * @param wbits vector of integer representations of all words
     */
    
    std::map<unsigned int, std::vector<unsigned int>> adj_map;

    for(unsigned int idx1=0; idx1<wbits.size(); ++idx1){
        for(unsigned int idx2=0; idx2<wbits.size(); ++idx2){
            if((wbits[idx1] & wbits[idx2]) == 0){
                adj_map[idx1].push_back(idx2);
            }
        }
    }
    return adj_map;
};

std::vector<unsigned int> cut_before(std::vector<unsigned int>& neigh, unsigned int cutoff){

    /**
     * The neighbors vector is split and all values before the cutoff removed
     * 
     * @param neigh vector of integer representations of neighbor words, sorted
     * @param cutoff representation value to use as a cutoff
     * @note The integers in the neighbor vector are sorted
     * @note This is used to allow us to consider only the lower triangular adjacency matrix
     */

    unsigned int i{0};
    for(i; i < neigh.size(); ++i){
        if(neigh[i] > cutoff){
            break;
        }
    }

    std::vector<unsigned int> result(neigh.size()-i);
    for(unsigned int j=i; j < neigh.size(); ++j){
        result[j-i] = neigh[j];
    }

    return result;
}

std::vector<std::string> read_file(std::string fname){
    
    /**
     * Reads the initial file with all allowed words in alphabetical order
     * We keep only 5 letter words with distinct letters
     *
     * @param fname filename
     */
    
    std::ifstream file(fname);
    std::vector<std::string> words;
    
   	while (!file.eof()) {
		std::string word;
		file >> word;

		if (word.length() != 5) continue;
        if (!all_distinct_letters(word)) continue;

		words.push_back(word);
	}

    file.close();

    return words;
}

void write_file(std::string fname, const std::vector<std::string> &words){

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
        std::vector<std::string> words = read_file(complete_file);
        write_file(words_file, words);
    }

    std::vector<std::string> words = read_file(words_file);
    std::vector<unsigned int> wunins = gen_unint(words);

    std::cout << "Building adjacency list" << "\n";
    std::map<unsigned int, std::vector<unsigned int>> adj_map = gen_adj_map(wunins);
    
    std::cout << "Building cliques" << "\n";
    std::vector<std::vector<unsigned int>> outcomes;
    std::vector<unsigned int> lookup1;
    std::vector<unsigned int> lookup2;
    std::vector<unsigned int> lookup3;
    std::vector<unsigned int> lookup4;
    unsigned int tmpbit2 {1};
    unsigned int tmpbit3 {1};
    unsigned int tmpbit4 {1};

    // Nest 5 loop
    // Outtermost loop over all words
    // Each inner loop, iterating over the neighbor list
    // each neighbor list pruned from all words with higher alphabetical order

    for(unsigned int idxw1=0; idxw1<adj_map.size(); ++idxw1){
        lookup1 = cut_before(adj_map[idxw1], idxw1);
        for(unsigned int& idxw2 : lookup1){
            tmpbit2 = (wunins[idxw1] | wunins[idxw2]);
            lookup2 = cut_before(adj_map[idxw2], idxw2);
            for(unsigned int& idxw3 : lookup2){
                if((tmpbit2 & wunins[idxw3]) == 0){                      
                    tmpbit3 = tmpbit2 | wunins[idxw3];
                    lookup3 = cut_before(adj_map[idxw3], idxw3);
                    for(unsigned int& idxw4 : lookup3){
                        if((tmpbit3 & wunins[idxw4]) == 0){
                            tmpbit4 = tmpbit3 | wunins[idxw4];
                            lookup4 = cut_before(adj_map[idxw4], idxw4);
                            for(unsigned int& idxw5 : lookup4){
                                if((tmpbit4 & wunins[idxw5]) == 0){
                                    if(debug){
                                        std::cout << words[idxw1] << " "; 
                                        std::cout << words[idxw2] << " ";
                                        std::cout << words[idxw3] << " ";
                                        std::cout << words[idxw4] << " "; 
                                        std::cout << words[idxw5] << "\n";
                                    }
                                    outcomes.push_back({idxw1,idxw2,idxw3,idxw4,idxw5});
                                } // if all new letters
                            } // for lookup 5th word 
                        } // if all new letters
                    } // for lookup 4th word  
                } // if all new letters
            } // for lookup 3th word
        } // for lookup 2th word

        if(debug){
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

    } // for lookup 1th word

    if(debug){
        std::cout << "Cliques built" << "\n";
        std::cout << outcomes.size()<< "\n";    
    }

    std::ofstream outfile("words_out.txt");
    
    for(unsigned int i=0; i<outcomes.size(); ++i){
        for(int j=0; j<5; ++j){
            outfile << words[outcomes[i][j]] << " ";
        }
        outfile << "\n";
    }

    outfile.close();

}
