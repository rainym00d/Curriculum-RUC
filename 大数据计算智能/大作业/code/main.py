import os
from tools import *
from preprocessing import get_train_datasets, load_data, make_dataset
from model import *
from config import Config
import torch
from torch.autograd import Variable
from torch.utils.data import DataLoader




def pred(model, data_loader, cuda):
    preds = []

    model.eval()
    for batch_idx, data_batch in enumerate(data_loader):

        samples, predicates, joins, targets, sample_masks, predicate_masks, join_masks = data_batch

        if cuda:
            samples, predicates, joins, targets = samples.cuda(), predicates.cuda(), joins.cuda(), targets.cuda()
            sample_masks, predicate_masks, join_masks = sample_masks.cuda(), predicate_masks.cuda(), join_masks.cuda()
        samples, predicates, joins, targets = Variable(samples), Variable(predicates), Variable(joins), Variable(
            targets)
        sample_masks, predicate_masks, join_masks = Variable(sample_masks), Variable(predicate_masks), Variable(
            join_masks)

        outputs = model(samples, predicates, joins, sample_masks, predicate_masks, join_masks)

        for i in range(outputs.data.shape[0]):
            preds.append(outputs.data[i])

    return preds


def run(workload_name, num_queries, num_epochs, batch_size, hid_units, cuda):
    num_materialized_samples = 1000
    dicts, column_min_max_vals, min_val, max_val, labels_train, labels_test, max_num_joins, max_num_predicates, train_data, test_data = get_train_datasets(
        num_queries, num_materialized_samples)
    table2vec, column2vec, op2vec, join2vec = dicts

    sample_feats = len(table2vec) + num_materialized_samples
    predicate_feats = len(column2vec) + len(op2vec) + 1
    join_feats = len(join2vec)

    model = MyModel(sample_feats, predicate_feats, join_feats, hid_units)

    optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

    if cuda:
        model.cuda()

    train_data_loader = DataLoader(train_data, batch_size=batch_size)
    test_data_loader = DataLoader(test_data, batch_size=batch_size)

    model.train()
    for epoch in range(num_epochs):
        loss_total = 0.

        for batch_idx, data_batch in enumerate(train_data_loader):

            samples, predicates, joins, targets, sample_masks, predicate_masks, join_masks = data_batch

            if cuda:
                samples, predicates, joins, targets = samples.cuda(), predicates.cuda(), joins.cuda(), targets.cuda()
                sample_masks, predicate_masks, join_masks = sample_masks.cuda(), predicate_masks.cuda(), join_masks.cuda()
            samples, predicates, joins, targets = Variable(samples), Variable(predicates), Variable(joins), Variable(
                targets)
            sample_masks, predicate_masks, join_masks = Variable(sample_masks), Variable(predicate_masks), Variable(
                join_masks)

            optimizer.zero_grad()
            outputs = model(samples, predicates, joins, sample_masks, predicate_masks, join_masks)
            loss = loss_by_qerror(outputs, targets.float(), min_val, max_val)
            loss_total += loss.item()
            loss.backward()
            optimizer.step()

        print("Epoch {}, loss: {}".format(epoch, loss_total / len(train_data_loader)))

    preds_train = pred(model, train_data_loader, cuda)

    preds_test = pred(model, test_data_loader, cuda)

    preds_train_unnorm = unnorm_result(preds_train, min_val, max_val)
    labels_train_unnorm = unnorm_result(labels_train, min_val, max_val)

    preds_test_unnorm = unnorm_result(preds_test, min_val, max_val)
    labels_test_unnorm = unnorm_result(labels_test, min_val, max_val)

    file_name = "workloads/" + workload_name
    joins, predicates, tables, samples, label = load_data(file_name, num_materialized_samples)

    samples_test = encode_samples(tables, samples, table2vec)
    predicates_test, joins_test = encode_data(predicates, joins, column_min_max_vals, column2vec, op2vec, join2vec)
    labels_test, _, _ = norm_result(label, min_val, max_val)

    max_num_predicates = max([len(p) for p in predicates_test])
    max_num_joins = max([len(j) for j in joins_test])

    test_data = make_dataset(samples_test, predicates_test, joins_test, labels_test, max_num_joins, max_num_predicates)
    test_data_loader = DataLoader(test_data, batch_size=batch_size)

    preds_test = pred(model, test_data_loader, cuda)

    preds_test_unnorm = unnorm_result(preds_test, min_val, max_val)

    file_name = "results/hidden_units_" + str(hid_units)+"_epochs_"+str(num_epochs) + ".csv"
    os.makedirs(os.path.dirname(file_name), exist_ok=True)
    with open(file_name, "w") as f:
        for i in range(len(preds_test_unnorm)):
            f.write(str(preds_test_unnorm[i]) + "," + label[i] + "\n")


if __name__ == "__main__":
    config = Config()
    run(config.testset, config.queries, config.epochs, config.batch, config.hid, config.cuda)
