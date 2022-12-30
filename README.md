# Five Words

I saw the problem on [youtube](https://www.youtube.com/watch?v=_-AfhLQfb6w). Its generic form would be given an alphabet and wordlength find the minimal amount of words which cover entirely or the largest possible subset of the given alphabet. In particular, the task is to find 5 English words with 25 distinct characters.

## Approach

I use a cycle graph to find valid solutions for a given alphabetically sorted wordlist in the following manner:

1. All words are repsentented as uints encoding the letters in each word as bits.
2. Adjancency lists are being built as a mapping between each word and its list of neighbors - the words with different characters, only the words with higher alphabeticall order are stored.
3. Cliques of the prespecified size are built by iterating over the wordlist and considering the each next word from the adjancency list of the previous (cyclic graph), while keeping track of all letters from the words added to the clique.
4. All cliques are stored in a solutions container.

A similar solution was implemented [here](https://gitlab.com/bpaassen/five_clique).

## Build, run and time
The code utilizes the parallel execution algorithms implemented in the C++17 standard:

`λ git main* → g++ cycle_graph.cpp -o cycle_graph.o -ltbb -std=c++17 -O3 && time ./cycle_graph.o`
