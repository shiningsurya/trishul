"""In charge of training, evaluating

Has all the functions

"""
from tqdm import tqdm
from .deeputils import RunningMean, TopK, Recall_score, Precision_score
from torch.autograd import Variable


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
