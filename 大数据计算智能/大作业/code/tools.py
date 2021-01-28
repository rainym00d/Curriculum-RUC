import numpy as np
import torch


def norm_result(labels, min_val=None, max_val=None):
    labels = np.array([np.log(float(l)) for l in labels])
    if min_val is None:
        min_val = labels.min()
    if max_val is None:
        max_val = labels.max()
    labels_norm = (labels - min_val) / (max_val - min_val)
    labels_norm = np.minimum(labels_norm, 1)
    labels_norm = np.maximum(labels_norm, 0)
    return labels_norm, min_val, max_val


def unnorm_result(labels_norm, min_val, max_val):
    labels_norm = np.array(labels_norm, dtype=np.float32)
    labels = (labels_norm * (max_val - min_val)) + min_val
    return np.array(np.round(np.exp(labels)), dtype=np.int64)


def encode_samples(tables, samples, table2vec):
    samples_enc = []
    for i, query in enumerate(tables):
        samples_enc.append(list())
        for j, table in enumerate(query):
            sample_vec = []
            sample_vec.append(table2vec[table])
            sample_vec.append(samples[i][j])
            sample_vec = np.hstack(sample_vec)
            samples_enc[i].append(sample_vec)
    return samples_enc


def encode_data(predicates, joins, column_min_max_vals, column2vec, op2vec, join2vec):
    def norm_data(val, column_name, column_min_max_vals):
        min_val = column_min_max_vals[column_name][0]
        max_val = column_min_max_vals[column_name][1]
        val = float(val)
        val_norm = 0.0
        if max_val > min_val:
            val_norm = (val - min_val) / (max_val - min_val)
        return np.array(val_norm, dtype=np.float32)

    predicates_enc = []
    joins_enc = []
    for i, query in enumerate(predicates):
        predicates_enc.append(list())
        joins_enc.append(list())
        for predicate in query:
            if len(predicate) == 3:
                column = predicate[0]
                operator = predicate[1]
                val = predicate[2]
                norm_val = norm_data(val, column, column_min_max_vals)

                pred_vec = []
                pred_vec.append(column2vec[column])
                pred_vec.append(op2vec[operator])
                pred_vec.append(norm_val)
                pred_vec = np.hstack(pred_vec)
            else:
                pred_vec = np.zeros((len(column2vec) + len(op2vec) + 1))

            predicates_enc[i].append(pred_vec)

        for predicate in joins[i]:
            join_vec = join2vec[predicate]
            joins_enc[i].append(join_vec)
    return predicates_enc, joins_enc


def loss_by_qerror(preds, targets, min_val, max_val):
    def unnorm_tmp(vals, min_val, max_val):
        vals = (vals * (max_val - min_val)) + min_val
        return torch.exp(vals)
    qerror = []
    preds = unnorm_tmp(preds, min_val, max_val)
    targets = unnorm_tmp(targets, min_val, max_val)

    for i in range(len(targets)):
        if (preds[i] > targets[i]).cpu().data.numpy()[0]:
            qerror.append(preds[i] / targets[i])
        else:
            qerror.append(targets[i] / preds[i])
    return torch.mean(torch.cat(qerror))