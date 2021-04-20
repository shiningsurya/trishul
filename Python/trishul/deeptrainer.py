"""In charge of training, evaluating

Has all the functions

"""
from tqdm import tqdm
from .deeputils import RunningMean, TopK, Recall_score, Precision_score
from torch.autograd import Variable
import torch.nn as tn


def Trainer (model, optimizer, loss_fn, dataloader, metrics):
    """Train for one epoch
    Args:
        model: (nn.Module) the neural network
        optimizer:   optimizer

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # go to training
    model.train ()
    # main training epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Trainer")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        train_b    = Variable (sample_b['bt'].cuda(non_blocking=True))
        target_b   = Variable (sample_b['target'].cuda(non_blocking=True))
        # forward pass
        output_b   = model (train_b)
        # loss
        loss       = loss_fn (output_b, target_b)
        # clear previous grads
        # TODO understand why they designed it this way
        optimizer.zero_grad ()
        # backprop pass
        loss.backward ()
        # update model
        optimizer.step ()
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, target_b) )
    # return computed metrics
    return retmetrics

def AETrainer (model, optimizer, loss_fn, dataloader, metrics):
    """Trains and autoencoder for one epoch

    Different with Trainer in target is same as train.
    Args:
        model: (nn.Module) the neural network
        optimizer:   optimizer

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # go to training
    model.train ()
    # main training epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Trainer")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        train_b    = Variable (sample_b['payload'].cuda(non_blocking=True))
        # forward pass
        output_b, code_b = model (train_b)
        # loss
        loss       = loss_fn (output_b, train_b)
        # clear previous grads
        # TODO understand why they designed it this way
        optimizer.zero_grad ()
        # backprop pass
        loss.backward ()
        # update model
        optimizer.step ()
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, target_b) )
    # return computed metrics
    return retmetrics

def Evaluator (model, loss_fn, dataloader, metrics):
    """Evaluates for one epoch
    Args:
        model: (nn.Module) the neural network
        loss_fn 

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # much easier than train bc 
    # we don't have optimization step
    model.eval ()
    # main evaluation epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Evaluator")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        eval_b    = Variable (sample_b['bt'].cuda(non_blocking=True))
        target_b   = Variable (sample_b['target'].cuda(non_blocking=True))
        # forward pass them
        output_b  = model (eval_b)
        # loss
        loss      = loss_fn (output_b, target_b)
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, target_b) )
    # return metrics
    return retmetrics

def AECLFTrainerNoCuda (model, clf, optimizer, loss_fn, dataloader, metrics):
    """Trains an clf which takes input from an autoencoder for one epoch

    Different with Trainer in target is same as train.
    Args:
        model: (nn.Module) the neural network
        optimizer:   optimizer

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # go to training
    # only the classifier goes to training
    clf.train ()
    # main training epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Trainer")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        train_b    = Variable (sample_b['payload'])
        target_b   = Variable (sample_b['target'])
        # forward pass
        output_b, code_b = model (train_b)
        predict_b   = clf(code_b)
        # loss
        loss       = loss_fn (predict_b, target_b)
        # clear previous grads
        # TODO understand why they designed it this way
        optimizer.zero_grad ()
        # backprop pass
        loss.backward ()
        # update model
        optimizer.step ()
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (predict_b, target_b) )
    # return computed metrics
    return retmetrics

def AETrainerNoCuda (model, optimizer, loss_fn, dataloader, metrics):
    """Trains and autoencoder for one epoch

    Different with Trainer in target is same as train.
    Args:
        model: (nn.Module) the neural network
        optimizer:   optimizer

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # go to training
    model.train ()
    # main training epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Trainer")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        train_b    = Variable (sample_b['payload'])
        # forward pass
        output_b, code_b = model (train_b)
        # loss
        loss       = loss_fn (output_b, train_b)
        # clear previous grads
        # TODO understand why they designed it this way
        optimizer.zero_grad ()
        # backprop pass
        loss.backward ()
        # update model
        optimizer.step ()
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, train_b) )
    # return computed metrics
    return retmetrics

def AETrainer (model, optimizer, loss_fn, dataloader, metrics):
    """Trains and autoencoder for one epoch

    Different with Trainer in target is same as train.
    Args:
        model: (nn.Module) the neural network
        optimizer:   optimizer

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # go to training
    model.train ()
    # main training epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Trainer")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        train_b    = Variable (sample_b['payload'].cuda(non_blocking=True))
        # forward pass
        output_b, code_b = model (train_b)
        # loss
        loss       = loss_fn (output_b, train_b)
        # clear previous grads
        # TODO understand why they designed it this way
        optimizer.zero_grad ()
        # backprop pass
        loss.backward ()
        # update model
        optimizer.step ()
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, train_b) )
    # return computed metrics
    return retmetrics

def AEEvaluatorNoCuda (model, loss_fn, dataloader, metrics):
    """Evaluates for one epoch
    Args:
        model: (nn.Module) the neural network
        loss_fn 

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # much easier than train bc 
    # we don't have optimization step
    model.eval ()
    # main evaluation epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Evaluator")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        eval_b    = Variable (sample_b['payload'])
        # forward pass them
        output_b, code_b  = model (eval_b)
        # loss
        loss      = loss_fn (output_b, eval_b)
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, eval_b) )
    # return metrics
    return retmetrics

def AECLFEvaluatorNoCuda (model, clf, loss_fn, dataloader, metrics):
    """Evaluates for one epoch
    Args:
        model: (nn.Module) the neural network
        loss_fn 

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # much easier than train bc 
    # we don't have optimization step
    clf.eval ()
    sf = tn.Softmax()
    # main evaluation epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Evaluator")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        eval_b    = Variable (sample_b['payload'])
        target_b   = Variable (sample_b['target'])
        # forward pass them
        output_b, code_b  = model (eval_b)
        predict_b   = clf(code_b)
        # loss
        loss       = loss_fn (predict_b, target_b.squeeze(1))
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (sf(predict_b), target_b) )
    # return metrics
    return retmetrics

def AEEvaluator (model, loss_fn, dataloader, metrics):
    """Evaluates for one epoch
    Args:
        model: (nn.Module) the neural network
        loss_fn 

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # much easier than train bc 
    # we don't have optimization step
    model.eval ()
    # main evaluation epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Evaluator")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        eval_b    = Variable (sample_b['payload'].cuda(non_blocking=True))
        # forward pass them
        output_b, code_b  = model (eval_b)
        # loss
        loss      = loss_fn (output_b, eval_b)
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, eval_b) )
    # return metrics
    return retmetrics

def AETrainerNoCudaScaled (model, optimizer, loss_fn, dataloader, metrics):
    """Trains and autoencoder for one epoch

    Different with Trainer in target is same as train.
    Args:
        model: (nn.Module) the neural network
        optimizer:   optimizer

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # go to training
    model.train ()
    # main training epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Trainer")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        train_b    = Variable (sample_b['payload'])
        # forward pass
        output_b, code_b = model (train_b)
        # loss
        loss       = loss_fn (output_b*255, train_b*255)
        # clear previous grads
        # TODO understand why they designed it this way
        optimizer.zero_grad ()
        # backprop pass
        loss.backward ()
        # update model
        optimizer.step ()
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, train_b) )
    # return computed metrics
    return retmetrics

def AEEvaluatorNoCudaScaled (model, loss_fn, dataloader, metrics):
    """Evaluates for one epoch
    Args:
        model: (nn.Module) the neural network
        loss_fn 

    """
    retmetrics = {k:RunningMean() for k in metrics.keys() }
    retmetrics['loss'] = []
    # much easier than train bc 
    # we don't have optimization step
    model.eval ()
    # main evaluation epoch loop
    tdl = tqdm (dataloader, unit="bt", desc="Evaluator")
    for i, sample_b  in enumerate (tdl):
        # torch Variables
        eval_b    = Variable (sample_b['payload'])
        # forward pass them
        output_b, code_b  = model (eval_b)
        # loss
        loss       = loss_fn (output_b*255, eval_b*255)
        # compute the metrics
        retmetrics['loss'].append (loss.item())
        for k, v in metrics.items():
            retmetrics[k] ( v (output_b, eval_b) )
    # return metrics
    return retmetrics
