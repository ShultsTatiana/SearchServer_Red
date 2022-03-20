#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>

vector<string> SplitIntoWords(const string& line) {
    istringstream words_input(line);
    return { istream_iterator<string>(words_input), istream_iterator<string>() };
}

SearchServer::SearchServer(istream& document_input) {
    UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    InvertedIndex new_index;

    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }

    index = move(new_index);
}

void SearchServer::AddQueriesStream( istream& query_input, 
                                     ostream& search_results_output ) {
    for (string current_query; getline(query_input, current_query); ) {
        const auto words = SplitIntoWords(current_query);
		
		//один раз создаём вектор для хранения рейтинга документов
		size_t Ndoc(index.getNumbDoc());
		vector<pair<size_t, size_t>> search_results(Ndoc);

        for (const auto& word : words) {
            const auto mapPtr = index.Lookup(word);
            if (mapPtr != nullptr) {
                for (const auto& [docid, count] : *mapPtr) {
					search_results[docid].first = docid;
                    search_results[docid].second += count;
                }
            }
        }
        partial_sort(begin(search_results), 
            (begin(search_results) + (5 < Ndoc ? 5: Ndoc)),
            end(search_results),
            [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
                int64_t lhs_docid = lhs.first;
                int64_t rhs_docid = rhs.first;
                return make_pair(lhs.second, -lhs_docid) > make_pair(rhs.second, -rhs_docid);
            }
        );

        search_results_output << current_query << ':';
        for (auto [docid, hitcount] : Head(search_results, 5)) {
            if (hitcount != 0) {
                search_results_output << " {"
                    << "docid: " << docid << ", "
                    << "hitcount: " << hitcount << '}';
            }
        }
        search_results_output << '\n';
    }
}

void LeftStrip(string_view& sv) {
    while (!sv.empty() && isspace(sv[0])) {
        sv.remove_prefix(1);
    }
}

string_view ReadToken(string_view& sv) {
    LeftStrip(sv);

    auto pos = sv.find(' ');
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());

    //LeftStrip(sv);
    return result;
}


map<string_view, size_t> SplitIntoWordsView(string_view line) {
    map<string_view, size_t> temp;
    while (!line.empty()) {
        string_view strView = ReadToken(line);
        if (!strView.empty()) { temp[strView]++; }
    }
    return temp;
}

void InvertedIndex::Add(string document) {
    docs.push_back(move(document));

    const size_t docid = docs.size() - 1;
    for (auto [word, count] : SplitIntoWordsView(docs[docid])) {
        index[word].push_back({ docid, count });
    }
    //for (auto word : SplitIntoWords(docs[docid])) {
    //    index[word][docid]++;
    //}
}

const vector<pair<size_t, size_t>>* InvertedIndex::Lookup(const string& word) const {
    if (auto it = index.find(word); it != index.end()) {
        const vector<pair<size_t, size_t>>* ptr = &(it->second);
        return ptr;
    }
    else {
        return nullptr;
    }
}
