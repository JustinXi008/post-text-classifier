# Post Text Classifier

A C++ command-line classifier that learns labels from tagged posts and predicts
labels for new text. It reports each prediction, its log score, and overall
accuracy on a labeled evaluation dataset.

## Build

Requires a C++17 compiler and Make.

```sh
make
```

## Create your own training data

Datasets are not included. Create a UTF-8 CSV file named `train.csv` in this
folder with two columns: `tag` (the category) and `content` (the post text).
Keep the header exactly as shown, and put one labeled post on each row:

```csv
tag,content
technology,new software update improves battery life
technology,how to install the latest software
sports,the team won the final match
sports,"great match, the team scored twice"
```

Replace these examples with your own labeled posts. Use consistent category
names and include several varied examples for every category. Put text that
contains commas or line breaks inside double quotes. Avoid empty labels or
empty posts, and include at least one training row.

To evaluate predictions, create a separate `test.csv` using the same `tag,content`
format and posts that are not in your training data. The test labels are the
expected categories used to calculate accuracy.

## Run

After creating your CSV files, train and evaluate:

```sh
./classifier train.csv test.csv
```

To inspect the learned vocabulary, class counts, and model parameters using
only the training file:

```sh
./classifier train.csv
```

## How it works

Each post becomes a set of unique, whitespace-separated words. Repeated words
within a post count once; case and punctuation are preserved.

For each label, the model adds its log prior to the log likelihood of each word.
Word likelihoods use the fraction of posts with that label containing the word.
If a word is absent from a label, the model backs off to its frequency across
all training posts; completely unseen words use `1 / training_post_count`.
The highest score wins, with alphabetical label order breaking ties.

Scores are unnormalized log scores, not calibrated probabilities. Training
requires a nonempty labeled dataset. The model runs in memory and is retrained
for each invocation.

## Clean

```sh
make clean
```
