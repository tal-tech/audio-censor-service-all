import re
import json
import pandas as pd


def neg_model(text, patterns):
    if len(patterns) == 0:
        return 0
    hit = False
    for pattern in patterns:
        if not re.search(pattern, text) is None:
            hit = True
    if hit:
        return 1
    else:
        return 0


def pos_model(text, patterns):
    if len(patterns) == 0:
        return 0
    hit = False
    for pattern in patterns:
        if not re.search(pattern, text) is None:
            hit = True
    if hit:
        return 1
    else:
        return 0


def model(text, config):
    patterns = config['patterns']
    neg_patterns = config['neg_patterns']
    actions = config['actions']

    neg_label = neg_model(text, neg_patterns)
    pos_label = pos_model(text, patterns)
    if pos_label == 1 and neg_label == 1:
        return actions["{}_{}".format(pos_label, neg_label)]
    elif pos_label == 1 and neg_label == 0:
        return actions["{}_{}".format(pos_label, neg_label)]
    elif pos_label == 0 and neg_label == 1:
        return actions["{}_{}".format(pos_label, neg_label)]
    elif pos_label == 0 and neg_label == 0:
        return actions["{}_{}".format(pos_label, neg_label)]


def model_final(text, word, model_config):
    remove_words = []
    if word in remove_words:
        return 0
    if word in model_config:
        label = model(text, model_config[word])
    elif re.search("(没有|不|没|不能).{0,5}%s" % word, text):
        label = 0
    else:
        label = 1
    return label

class KFSentenceDetectorYuqing():
    def __init__(self, words, model_config_path):
        self.words = words
        with open(model_config_path,'r') as f:
            self.model_config = json.load(f)
        self.re_word = re.compile('|'.join(self.words))

    def convert_list_to_json(self, item):
        uniqueKeys = set(item)
        result = []
        for i in uniqueKeys:
            result.append({'keyword': i, 'word_count': item.count(i)})
        return result

    def predict_one_word(self, text, word):
        label = model_final(text, word, self.model_config)
        return label

    def predict_text(self, text):
        keywords = self.re_word.findall(text)
        #print(keywords)
        res = {}
        for word in keywords:
            if self.predict_one_word(text, word):
                res[word] = res.get(word, 0) +1
            else:
                pass
        return res

    def re_detector(self, df):
        keywords = df.text.apply(lambda x: self.re_word.findall(str(x)))
        keywordId = keywords.apply(lambda x: False if len(x) == 0 else True)
        df['keywords'] = keywords
        df_re = df[keywordId].copy()
        return df_re

    def predict_row(self, row):
        text = row['text']
        word = row['keywords'][0]
        label = model_final(text, word, self.model_config)
        return label

    def predict_text_list_format(self, text_list):
        df = pd.DataFrame(text_list)
        df['sentence_id'] = range(1, df.shape[0] + 1)
        df_re = self.re_detector(df)
        if df_re.shape[0] == 0:
            return []
        df_re['label'] = df_re.apply(self.predict_row, axis=1)
        df_re = df_re[df_re['label'] == 1]

        if df_re.shape[0] == 0:
            data = []
        else:
            data = df_re.apply(lambda x: {'begin_time': x['begin_time'], 'end_time': x['end_time'],
                                          'sentence': x['text'],
                                          'keyword_list': self.convert_list_to_json(x['keywords'])}, axis=1).tolist()
        return data

