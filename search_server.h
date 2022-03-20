#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <future>
using namespace std;

class InvertedIndex {
public:
    InvertedIndex() {
        docs.reserve(50'000);
    }
    void Add(string document);

    const vector<pair<size_t, size_t>>* Lookup(const string& word) const;
    vector<pair<size_t, size_t>> LookupV(const string& word) const;

    const string& GetDocument(size_t id) const {
        return docs[id];
    }
    const size_t getNumbDoc() const {
        return docs.size();
    }

private:
    map<string_view, vector<pair<size_t, size_t>>> index;
    vector<string> docs;
};

class Synchronized {
    InvertedIndex index;
    mutex m_mutex;
public:
    explicit Synchronized(InvertedIndex initial = InvertedIndex())
        : index(move(initial)), m_mutex() {
    }

    struct Access {
        lock_guard<mutex> m_g;
        InvertedIndex& ref_to_value;
    };

    Access GetAccess() {
        return {lock_guard(m_mutex), index};
    }
};

class SearchServer {
public:
    SearchServer() = default;
    explicit SearchServer(istream& document_input);
    void UpdateDocumentBase(istream& document_input);
    void Update(istream& document_input);
    void AddQueriesStream(istream& query_input, ostream& search_results_output);
    void AddQueriesSingleTread(istream& query_input, ostream& search_results_output);

private:
    Synchronized index;
    vector<future<void>> threadeFutures;
    //vector<mutex> threadeMutex;
    //mutex mu;
};
