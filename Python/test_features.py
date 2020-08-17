
import os
import pandas as pd
import catchaMouse16

#-------------------------------------------------------------------------------
def giveMeFeatureVector(tsData):
    '''
    Returns a catchaMouse-16 feature vector from input time-series data
    '''
    features = dir(catchaMouse16)
    features = [item for item in features if not '__' in item]

    featureVector = []
    for testFun in features:
        featureFun = getattr(catchaMouse16,testFun)
        featureVector.append(featureFun(tsData))

    return featureVector

#-------------------------------------------------------------------------------

x = pd.read_csv('test_ts.csv',header=None);
tsData = x[0].values.tolist()
fV = giveMeFeatureVector(tsData)
