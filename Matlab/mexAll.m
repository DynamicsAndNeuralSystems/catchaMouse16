% path where C implementation lives
basePath = '../src';
ipath = {['-I', basePath], ['-I','/usr/local/include'], '-I.'};

% list all C files to include in the mex-call
CFiles = dir(basePath); 
CFiles = CFiles(cellfun(@(x) contains(x, '.c'), {CFiles.name})); 
CFileNames = {CFiles.name};

% add path to the c file names
includeFiles = cellfun(@(x) fullfile(basePath, x), CFileNames, 'UniformOutput', false);

% get function names
fid = fopen('../featureList.txt','r');
i = 1;
tline = fgetl(fid);
featureNames{i} = tline;
while ischar(tline)
    i = i+1;
    tline = fgetl(fid);
    featureNames{i} = tline;
end
fclose(fid);

% mex all feature functions separately
for i = 1:length(featureNames)-1
    
    featureName = featureNames{i};
    
    fprintf('Compiling %s...\n', featureName);
    mex(ipath{:}, ['catchaMouse16_', featureName,'.c'], 'M_wrapper.c', includeFiles{:}, ['-L','/usr/local/lib'], '-lgsl', '-lgslcblas', '-lm')
    fprintf('\n');
end
% mex -I../src -I/usr/local/include -L/usr/local/lib -lgsl -lgslcblas -lm catchaMouse16_AC_nl_036.c M_wrapper.c ../src/stats.c ../src/CO_NonlinearAutoCorr.c ../src/helper_functions.c
%d = randn(900,1);
%d = zscore(d);