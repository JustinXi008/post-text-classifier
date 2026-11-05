#include <iostream>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include "csv_reader.hpp"
#include <fstream>
using namespace std;

set<string> unique_words(const string &str)
{
    istringstream source(str);
    set<string> words;
    string word;
    while (source >> word)
    {
        words.insert(word);
    }
    return words;
}

class Classifier
{
private:
    int total_post = 0;

    set<string> vocab;

    map<string, int> posts_containing_word;

    map<string, int> posts_per_label;

    map<string, map<string, int>> posts_with_LC;

    double log_prior(string label)
    {
        return log((double)posts_per_label.at(label) / total_post);
    }

    double log_likelihood(string word, string label)
    {
        auto label_at = posts_with_LC.find(label);
        if (label_at != posts_with_LC.end())
        {
            auto word_at = label_at->second.find(word);
            if (word_at != label_at->second.end())
            {
                return log((double)word_at->second / posts_per_label.at(label));
            }
        }

        auto word_entire = posts_containing_word.find(word);
        if (word_entire != posts_containing_word.end())
        {
            return log((double)word_entire->second / total_post);
        }

        return log(1.0 / total_post);
    }

public:
    void train(csvstream &csvin, bool train_only)
    {
        map<string, string> row;
        if (train_only)
        {
            cout << "training data:" << endl;
        }

        while (csvin >> row)
        {
            const string &label = row["tag"];
            const string &content = row["content"];
            if (train_only)
            {
                cout << "  label = " << label << ", content = " << content << endl;
            }

            total_post++;
            posts_per_label[label]++;

            set<string> words = unique_words(content);

            for (const string &word : words)
            {
                vocab.insert(word);
                posts_containing_word[word]++;
                posts_with_LC[label][word]++;
            }
        }
    }

    int get_total_post()
    {
        return total_post;
    }

    int vocabulary()
    {
        return vocab.size();
    }

    void print_class()
    {
        cout << "classes:" << endl;
        for (auto &i : posts_per_label)
        {
            cout << "  " << i.first << ", " << i.second
                 << " examples, log-prior = " << log_prior(i.first) << endl;
        }
    }

    void print_parameters()
    {
        cout << "classifier parameters:" << endl;
        for (auto &i : posts_with_LC)
        {
            for (auto &j : i.second)
            {
                cout << "  " << i.first << ":" << j.first << ", count = " << j.second
                     << ", log-likelihood = " << log_likelihood(j.first, i.first)
                     << endl;
            }
        }
    }

    double score(const set<string> &words, const string &label)
    {
        double total = log_prior(label);
        set<string>::const_iterator it;

        for (it = words.begin(); it != words.end(); it++)
        {
            string current_word = *it;
            double current_log = log_likelihood(current_word, label);
            total = total + current_log;
        }
        return total;
    }

    string predict(const string &content, double &best_score)
    {
        set<string> words = unique_words(content);

        bool first_label = true;
        string best_label;

        map<string, int>::const_iterator it;

        for (it = posts_per_label.begin(); it != posts_per_label.end(); it++)
        {
            string current_label = it->first;
            double current_score = score(words, current_label);

            if (first_label)
            {
                best_label = current_label;
                best_score = current_score;
                first_label = false;
            }
            else if (current_score > best_score)
            {
                best_label = current_label;
                best_score = current_score;
            }
            else if (current_score == best_score && current_label < best_label)
            {
                best_label = current_label;
            }
        }
        return best_label;
    }
};

void test_classifier(Classifier &classifier, const string &testfile)
{
    csvstream csvin(testfile);
    map<string, string> data;

    int correct = 0;
    int total = 0;

    string correct_label = "";
    string content = "";
    string predicted_label = "";

    double best_score = 0.0;

    cout << "test data:" << endl;

    while (csvin >> data)
    {
        correct_label = data["tag"];
        content = data["content"];

        predicted_label = classifier.predict(content, best_score);

        bool label_match = predicted_label == correct_label;
        if (label_match)
        {
            correct++;
        }
        total++;

        cout << "  correct = " << correct_label
             << ", predicted = " << predicted_label
             << ", log-probability score = " << best_score << endl;
        cout << "  content = " << content << endl;
        cout << endl;
    }

    cout << "performance: " << correct
         << " / " << total << " posts predicted correctly";
    cout << endl;
}

int main(int argc, char *argv[])
{
    cout.precision(3);

    if (argc != 2 && argc != 3)
    {
        cout << "Usage: classifier TRAIN_FILE [TEST_FILE]" << endl;
        return 1;
    }

    string trainfile = argv[1];
    string testfile;

    bool train_only = (argc == 2);

    Classifier classifier;

    try
    {
        csvstream train_csv(trainfile);
        classifier.train(train_csv, train_only);
    }
    catch (const csvstream_exception &e)
    {
        cout << e.what() << endl;
        return 1;
    }

    cout << "trained on " << classifier.get_total_post() << " examples" << endl;

    if (train_only)
    {
        cout << "vocabulary size = " << classifier.vocabulary() << endl;
        cout << endl;

        classifier.print_class();
        cout << endl;

        classifier.print_parameters();
        cout << endl;
        return 0;
    }
    else
    {
        testfile = argv[2];

        try
        {
            test_classifier(classifier, testfile);
        }
        catch (const csvstream_exception &e)
        {
            cout << e.what() << endl;
            return 1;
        }
    }
}
