import csv
import numpy as np
import torch
from torch.utils.data import dataset

from tools import *

def get_set_encoding(source_set, onehot=True):
    def idx_to_onehot(idx, num_elements):
        onehot = np.zeros(num_elements, dtype=np.float32)
        onehot[idx] = 1.
        return onehot
    
    num_elements = len(source_set)
    source_list = list(source_set)
    source_list.sort()
    thing2idx = {s: i for i, s in enumerate(source_list)}
    idx2thing = [s for i, s in enumerate(source_list)]
    if onehot:
        thing2vec = {s: idx_to_onehot(i, num_elements) for i, s in enumerate(source_list)}
        return thing2vec, idx2thing
    return thing2idx, idx2thing

def load_data(file_name, num_materialized_samples):
    def chunks(l, n):
        for i in range(0, len(l), n):
            yield l[i:i + n]
    
    joins = []
    predicates = []
    tables = []
    samples = []
    label = []

    with open(file_name + ".csv", 'rU') as f:
        data_raw = list(list(rec) for rec in csv.reader(f, delimiter='#'))
        for row in data_raw:
            tables.append(row[0].split(','))
            joins.append(row[1].split(','))
            predicates.append(row[2].split(','))
            if int(row[3]) < 1:
                exit(1)
            label.append(row[3])

    num_bytes_per_bitmap = int((num_materialized_samples + 7) >> 3)
    with open(file_name + ".bitmaps", 'rb') as f:
        for i in range(len(tables)):
            four_bytes = f.read(4)
            if not four_bytes:
                exit(1)
            num_bitmaps_curr_query = int.from_bytes(four_bytes, byteorder='little')
            bitmaps = np.empty((num_bitmaps_curr_query, num_bytes_per_bitmap * 8), dtype=np.uint8)
            for j in range(num_bitmaps_curr_query):
                # Read bitmap
                bitmap_bytes = f.read(num_bytes_per_bitmap)
                if not bitmap_bytes:
                    exit(1)
                bitmaps[j] = np.unpackbits(np.frombuffer(bitmap_bytes, dtype=np.uint8))
            tmp = np.zeros((num_bitmaps_curr_query, num_bytes_per_bitmap * 8))
            samples.append(tmp)

    predicates = [list(chunks(d, 3)) for d in predicates]

    return joins, predicates, tables, samples, label


def load_and_encode_train_data(num_queries, num_materialized_samples):
    def get_all_column_names(predicates):
        column_names = set()
        for query in predicates:
            for predicate in query:
                if len(predicate) == 3:
                    column_name = predicate[0]
                    column_names.add(column_name)
        return column_names

    def get_all_table_names(tables):
        table_names = set()
        for query in tables:
            for table in query:
                table_names.add(table)
        return table_names
    
    def get_all_operators(predicates):
        operators = set()
        for query in predicates:
            for predicate in query:
                if len(predicate) == 3:
                    operator = predicate[1]
                    operators.add(operator)
        return operators
    
    def get_all_joins(joins):
        join_set = set()
        for query in joins:
            for join in query:
                join_set.add(join)
        return join_set

    file_name_queries = "data/train"
    file_name_column_min_max_vals = "data/column_min_max_vals.csv"

    joins, predicates, tables, samples, label = load_data(file_name_queries, num_materialized_samples)

    column_names = get_all_column_names(predicates)
    column2vec, idx2column = get_set_encoding(column_names)

    table_names = get_all_table_names(tables)
    table2vec, idx2table = get_set_encoding(table_names)

    operators = get_all_operators(predicates)
    op2vec, idx2op = get_set_encoding(operators)

    join_set = get_all_joins(joins)
    join2vec, idx2join = get_set_encoding(join_set)

    with open(file_name_column_min_max_vals, 'rU') as f:
        data_raw = list(list(rec) for rec in csv.reader(f, delimiter=','))
        column_min_max_vals = {}
        for i, row in enumerate(data_raw):
            if i == 0:
                continue
            column_min_max_vals[row[0]] = [float(row[1]), float(row[2])]

    samples_enc = encode_samples(tables, samples, table2vec)
    predicates_enc, joins_enc = encode_data(predicates, joins, column_min_max_vals, column2vec, op2vec, join2vec)
    label_norm, min_val, max_val = norm_result(label)

    num_train = int(num_queries * 0.9)
    num_test = num_queries - num_train

    samples_train = samples_enc[:num_train]
    predicates_train = predicates_enc[:num_train]
    joins_train = joins_enc[:num_train]
    labels_train = label_norm[:num_train]

    samples_test = samples_enc[num_train:num_train + num_test]
    predicates_test = predicates_enc[num_train:num_train + num_test]
    joins_test = joins_enc[num_train:num_train + num_test]
    labels_test = label_norm[num_train:num_train + num_test]

    max_num_joins = max(max([len(j) for j in joins_train]), max([len(j) for j in joins_test]))
    max_num_predicates = max(max([len(p) for p in predicates_train]), max([len(p) for p in predicates_test]))

    dicts = [table2vec, column2vec, op2vec, join2vec]
    train_data = [samples_train, predicates_train, joins_train]
    test_data = [samples_test, predicates_test, joins_test]
    return dicts, column_min_max_vals, min_val, max_val, labels_train, labels_test, max_num_joins, max_num_predicates, train_data, test_data


def make_dataset(samples, predicates, joins, labels, max_num_joins, max_num_predicates):
    sample_masks = []
    sample_tensors = []
    for sample in samples:
        sample_tensor = np.vstack(sample)
        num_pad = max_num_joins + 1 - sample_tensor.shape[0]
        sample_mask = np.ones_like(sample_tensor).mean(1, keepdims=True)
        sample_tensor = np.pad(sample_tensor, ((0, num_pad), (0, 0)), 'constant')
        sample_mask = np.pad(sample_mask, ((0, num_pad), (0, 0)), 'constant')
        sample_tensors.append(np.expand_dims(sample_tensor, 0))
        sample_masks.append(np.expand_dims(sample_mask, 0))
    sample_tensors = np.vstack(sample_tensors)
    sample_tensors = torch.FloatTensor(sample_tensors)
    sample_masks = np.vstack(sample_masks)
    sample_masks = torch.FloatTensor(sample_masks)

    predicate_masks = []
    predicate_tensors = []
    for predicate in predicates:
        predicate_tensor = np.vstack(predicate)
        num_pad = max_num_predicates - predicate_tensor.shape[0]
        predicate_mask = np.ones_like(predicate_tensor).mean(1, keepdims=True)
        predicate_tensor = np.pad(predicate_tensor, ((0, num_pad), (0, 0)), 'constant')
        predicate_mask = np.pad(predicate_mask, ((0, num_pad), (0, 0)), 'constant')
        predicate_tensors.append(np.expand_dims(predicate_tensor, 0))
        predicate_masks.append(np.expand_dims(predicate_mask, 0))
    predicate_tensors = np.vstack(predicate_tensors)
    predicate_tensors = torch.FloatTensor(predicate_tensors)
    predicate_masks = np.vstack(predicate_masks)
    predicate_masks = torch.FloatTensor(predicate_masks)

    join_masks = []
    join_tensors = []
    for join in joins:
        join_tensor = np.vstack(join)
        num_pad = max_num_joins - join_tensor.shape[0]
        join_mask = np.ones_like(join_tensor).mean(1, keepdims=True)
        join_tensor = np.pad(join_tensor, ((0, num_pad), (0, 0)), 'constant')
        join_mask = np.pad(join_mask, ((0, num_pad), (0, 0)), 'constant')
        join_tensors.append(np.expand_dims(join_tensor, 0))
        join_masks.append(np.expand_dims(join_mask, 0))
    join_tensors = np.vstack(join_tensors)
    join_tensors = torch.FloatTensor(join_tensors)
    join_masks = np.vstack(join_masks)
    join_masks = torch.FloatTensor(join_masks)

    target_tensor = torch.FloatTensor(labels)

    return dataset.TensorDataset(sample_tensors, predicate_tensors, join_tensors, target_tensor, sample_masks,
                                 predicate_masks, join_masks)


def get_train_datasets(num_queries, num_materialized_samples):
    dicts, column_min_max_vals, min_val, max_val, labels_train, labels_test, max_num_joins, max_num_predicates, train_data, test_data = load_and_encode_train_data(
        num_queries, num_materialized_samples)
    train_dataset = make_dataset(*train_data, labels=labels_train, max_num_joins=max_num_joins,
                                 max_num_predicates=max_num_predicates)
    test_dataset = make_dataset(*test_data, labels=labels_test, max_num_joins=max_num_joins,
                                max_num_predicates=max_num_predicates)
    return dicts, column_min_max_vals, min_val, max_val, labels_train, labels_test, max_num_joins, max_num_predicates, train_dataset, test_dataset
