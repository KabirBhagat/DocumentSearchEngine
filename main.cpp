#include <bits/stdc++.h>
#include <conio.h>
using namespace std;

class TrieNode
{
public:
    TrieNode* children[26];
    bool isEnd;

    TrieNode()
    {
        for (int i = 0; i < 26; i++)
        {
            children[i] = nullptr;
        }
        isEnd = false;
    }
};

class Trie
{
public:
    TrieNode* root;

    Trie()
    {
        root = new TrieNode();
    }

    void insert(string word)
    {
        TrieNode* curr = root;

        for (char ch : word)
        {
            int idx = ch - 'a';

            if (idx < 0 || idx >= 26)
                return;

            if (curr->children[idx] == nullptr)
            {
                curr->children[idx] = new TrieNode();
            }

            curr = curr->children[idx];
        }

        curr->isEnd = true;
    }

    bool search(string word)
    {
        TrieNode* curr = root;

        for (char ch : word)
        {
            int idx = ch - 'a';

            if (idx < 0 || idx >= 26)
                return false;

            if (curr->children[idx] == nullptr)
            {
                return false;
            }

            curr = curr->children[idx];
        }

        return curr->isEnd;
    }

    bool startsWith(string prefix)
    {
        TrieNode* curr = root;

        for (char ch : prefix)
        {
            int idx = ch - 'a';

            if (idx < 0 || idx >= 26)
                return false;

            if (curr->children[idx] == nullptr)
            {
                return false;
            }

            curr = curr->children[idx];
        }

        return true;
    }

    void getWords(TrieNode* curr, string word,
                  vector<string>& suggestions)
    {
        if (curr->isEnd)
        {
            suggestions.push_back(word);
        }

        for (int i = 0; i < 26; i++)
        {
            if (curr->children[i] != nullptr)
            {
                getWords(curr->children[i],
                         word + char('a' + i),
                         suggestions);
            }
        }
    }

    vector<string> autocomplete(string prefix)
    {
        TrieNode* curr = root;

        for (char ch : prefix)
        {
            int idx = ch - 'a';

            if (idx < 0 || idx >= 26)
                return {};

            if (curr->children[idx] == nullptr)
            {
                return {};
            }

            curr = curr->children[idx];
        }

        vector<string> suggestions;
        getWords(curr, prefix, suggestions);

        return suggestions;
    }
};

int main()
{
    int documentID = 0;
    unordered_map<string, vector<int>> mp;
    vector<string> document;
    unordered_map<string, unordered_map<int, int>> mp2;
    vector<int> totalWords;
    Trie trie;

    // Index documents
    for (auto entry : filesystem::directory_iterator("documents"))
    {
        ifstream file(entry.path());
        if (!file)
            continue;

        string line;
        document.push_back(entry.path().filename().string());
        totalWords.push_back(0);

        while (getline(file, line))
        {
            stringstream ss(line);
            string word;

            while (ss >> word)
            {
                string cword = "";

                for (char ch : word)
                {
                    ch = tolower(ch);

                    if (isalnum(ch))
                    {
                        cword += ch;
                    }
                }

                if (cword.empty())
                    continue;

                totalWords[documentID]++;

                auto &v = mp[cword];
                mp2[cword][documentID]++;

                if (!v.empty() && v.back() == documentID)
                    continue;

                mp[cword].push_back(documentID);

                if (mp2[cword][documentID] == 1)
                {
                    bool valid = true;

                    for (char ch : cword)
                    {
                        if (ch < 'a' || ch > 'z')
                        {
                            valid = false;
                            break;
                        }
                    }

                    if (valid)
                        trie.insert(cword);
                }
            }
        }

        documentID++;
    }

    string ww = "", userWord = "";

    // Initial search screen
    cout << "Document Search\n\n";
    cout << "Search: ";

    while (true)
    {
        char ch = _getch();

        if (ch == 27)
        {
            system("cls");
            cout << "Document Search\n\n";
            cout << "Exiting search...\n";
            return 0;
        }
        else if (ch == '\r')
        {
            break;
        }
        else if (ch == '\b')
        {
            if (!ww.empty())
            {
                ww.pop_back();
            }
        }
        else if (isalpha((unsigned char)ch))
        {
            ch = tolower((unsigned char)ch);
            ww += ch;
        }
        else
        {
            continue;
        }

        // Refresh search screen
        system("cls");

        cout << "Document Search\n\n";
        cout << "Search: " << ww << "|\n\n";

        cout << "Suggestions:\n";

        if (ww.empty())
        {
            cout << "Start typing to see suggestions.\n";
        }
        else
        {
            vector<string> suggestions = trie.autocomplete(ww);

            if (suggestions.empty())
            {
                cout << "No suggestions found.\n";
            }
            else
            {
                int count = 0;

                for (auto word : suggestions)
                {
                    cout << word << "\n";
                    count++;

                    if (count == 5)
                        break;
                }

                if (suggestions.size() > 5)
                {
                    cout << "... and "
                         << suggestions.size() - 5
                         << " more\n";
                }
            }
        }

        cout << "\nPress ENTER to search | ESC to exit\n";
    }

    system("cls");

    cout << "Document Search\n\n";

    if (ww.empty())
    {
        cout << "Please enter a search word.\n";
        return 0;
    }

    userWord = ww;
    auto it = mp2.find(userWord);

    if (it == mp2.end())
    {
        cout << "No documents found for: "
             << userWord << "\n";
        return 0;
    }

    auto &mp3 = it->second;
    int N = document.size();
    int DF = mp3.size();
    double idf = log((double)N / DF);
    priority_queue<pair<double, int>> pq;

    // Calculate TF-IDF and rank documents
    for (auto i : mp3)
    {
        double tf = (double)i.second / totalWords[i.first];
        double tfidf = tf * idf;
        pq.push({tfidf, i.first});
    }

    // Display search results
    cout << "Search results for: " << ww << "\n\n";
    cout << "Found " << DF << " documents.\n\n";

    int rank = 1;

    while (!pq.empty())
    {
        auto i = pq.top();
        pq.pop();

        cout << rank << ". " << document[i.second] << "\n";
        cout << "   Occurrences: " << mp3[i.second] << "\n\n";

        rank++;
    }

    cout << "Search complete.\n";

    return 0;
}