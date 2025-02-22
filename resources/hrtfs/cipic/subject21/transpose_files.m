% Get a list of all WAV files in the current directory
files = dir('*.wav');

% Specify the new directory where the transposed files will be saved
newDirectory = 'transposed_files';

% Check if the directory exists, if not, create it
if ~exist(newDirectory, 'dir')
    mkdir(newDirectory);
end

% Loop through each file
for idx = 1:length(files)
    % Full path of the current file
    filePath = files(idx).name;
    
    % Read the WAV file
    [audioData, sampleRate] = audioread(filePath);
    
    % Transpose the audio data
    transposedData = audioData.';

    % Create the full path for the new file
    newFilePath = fullfile(newDirectory, filePath);
    
    % Write the transposed data to the new directory
    audiowrite(newFilePath, transposedData, sampleRate);
end

disp('All files have been transposed and saved to the new directory.');
