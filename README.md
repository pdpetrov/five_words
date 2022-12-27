# Five Words

I saw the problem on [youtube](https://www.youtube.com/watch?v=_-AfhLQfb6w). Its generic form would be given an alphabet and words length find the minimal amount of words which cover entirely or the largest possible subset of the alphabet. In particular, the task is to find five English words with 25 distinct characters. 

I use a cycle graph to solve the problem. First I build an adjancency list as a mapping between each word and its list of neighbors - words with different characters. Next, I start with a specific word and choose the next word from the adjancency list. I choose the third word from the neighbor list of the second, while keeping track of all used characters in the first two words and only select a word, if it consists only of non-overlapping characters, etc.

A similar solution was implemented [here](https://gitlab.com/bpaassen/five_clique).
