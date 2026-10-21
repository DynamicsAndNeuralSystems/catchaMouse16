function mexAll(gslPrefix)
% mexAll  Compile the catchaMouse16 MEX functions (needs GSL).
%
% mexAll           finds GSL with gsl-config, or else in /opt/homebrew,
%                  /usr/local or /usr
% mexAll(prefix)   uses the GSL installed under prefix (prefix/include and
%                  prefix/lib), e.g., mexAll('/opt/homebrew') or
%                  mexAll(getenv('CONDA_PREFIX'))
%
% The MEX files are written to the folder that contains this file.

% folder of this file, and where the C implementation lives
matlabDir = fileparts(mfilename('fullpath'));
basePath = fullfile(matlabDir, '..', 'C', 'src');

if nargin < 1
    gslPrefix = '';
end
[gslIncludeDirs, gslLibDirs] = findGSL(gslPrefix);
fprintf('Using GSL headers in %s\n', strjoin(gslIncludeDirs, ', '));

ipath = [{['-I', basePath], ['-I', matlabDir]}, strcat('-I', gslIncludeDirs)];
lpath = strcat('-L', gslLibDirs);
% find GSL's shared libraries at run time too (e.g., conda's @rpath libraries)
if isunix && ~isempty(gslLibDirs)
    lpath{end+1} = ['LDFLAGS=$LDFLAGS', sprintf(' -Wl,-rpath,%s', gslLibDirs{:})];
end

% list all C files to include in the mex-call (not the command-line program)
CFiles = dir(fullfile(basePath, '*.c'));
CFileNames = setdiff({CFiles.name}, {'main.c'});

% add path to the c file names
includeFiles = cellfun(@(x) fullfile(basePath, x), CFileNames, 'UniformOutput', false);

% get function names
featureNames = strsplit(strtrim(fileread(fullfile(matlabDir, '..', 'featureList.txt'))));

% mex all feature functions separately
for i = 1:length(featureNames)

    featureName = featureNames{i};

    fprintf('Compiling %s...\n', featureName);
    mex(ipath{:}, fullfile(matlabDir, ['catchaMouse16_', featureName, '.c']), ...
        fullfile(matlabDir, 'M_wrapper.c'), includeFiles{:}, lpath{:}, ...
        '-lgsl', '-lgslcblas', '-lm', '-outdir', matlabDir)
    fprintf('\n');
end

end

%-------------------------------------------------------------------------------
function [includeDirs, libDirs] = findGSL(gslPrefix)
% Find GSL's include and library directories.

hasHeaders = @(includeDir) exist(fullfile(includeDir, 'gsl', 'gsl_vector.h'), 'file') == 2;

% 1. a prefix given by the user
if ~isempty(gslPrefix)
    includeDirs = {fullfile(gslPrefix, 'include')};
    libDirs = {fullfile(gslPrefix, 'lib')};
    if ~hasHeaders(includeDirs{1})
        error('catchaMouse16:noGSL', 'GSL headers not found in %s', includeDirs{1});
    end
    return
end

% 2. gsl-config (MATLAB's PATH may not include Homebrew or conda, so also
% look in their usual bin folders)
gslConfigs = {'gsl-config', '/opt/homebrew/bin/gsl-config', '/usr/local/bin/gsl-config'};
condaPrefix = getenv('CONDA_PREFIX');
if ~isempty(condaPrefix)
    gslConfigs = [{fullfile(condaPrefix, 'bin', 'gsl-config')}, gslConfigs];
end
for k = 1:length(gslConfigs)
    [status, cflags] = system([gslConfigs{k}, ' --cflags']);
    if status ~= 0
        continue
    end
    [status, libs] = system([gslConfigs{k}, ' --libs']);
    if status ~= 0
        continue
    end
    includeDirs = flagValues(cflags, '-I');
    libDirs = flagValues(libs, '-L');
    if any(cellfun(hasHeaders, includeDirs))
        return
    end
end

% 3. the usual install locations
prefixes = {'/opt/homebrew', '/usr/local', '/usr'};
for k = 1:length(prefixes)
    if hasHeaders(fullfile(prefixes{k}, 'include'))
        includeDirs = {fullfile(prefixes{k}, 'include')};
        if strcmp(prefixes{k}, '/usr')
            libDirs = {}; % the linker searches the system folders already
        else
            libDirs = {fullfile(prefixes{k}, 'lib')};
        end
        return
    end
end

error('catchaMouse16:noGSL', ['GSL not found. Install GSL (e.g., brew install gsl, ', ...
      'or apt install libgsl-dev) or call mexAll(prefix) with its install prefix.']);
end

%-------------------------------------------------------------------------------
function values = flagValues(flags, flag)
% Values of the compiler flags that start with flag, e.g., '-I' or '-L'.
tokens = strsplit(strtrim(flags));
tokens = tokens(startsWith(tokens, flag));
values = cellfun(@(t) t(length(flag)+1:end), tokens, 'UniformOutput', false);
end
