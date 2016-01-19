clear all;

% open and read file
file = fopen('KitSprout.txt','r');
data = fread(file);

% get channel size
channelSize = size(data, 1) - 16;

fileDecode = 0;
% check file header
if (data(1)  == 75) & ...   % 'K'
   (data(2)  == 45) & ...   % '-'
   (data(3)  == 76) & ...   % 'L'
   (data(10) == 83) & ...   % 'S'
   (data(15) == 69) & ...   % 'E'
   (data(16) == 68)         % 'D'

    dataLens   = data(4) * 2^40 + data(5) * 2^32 + data(6) * 2^24 +  data(7) * 2^16 + data(8) * 2^8 + data(9);
    sampleRate = data(11) * 2^24 +  data(12) * 2^16 + data(13) * 2^8 + data(14);

    % check channel size
    sChannelLens = dataLens + 8;
    if mod(channelSize, sChannelLens) == 0
        fileDecode = 1;
        channel = channelSize / sChannelLens;
        for i = 0 : (channel - 1)
            % check channel header
            if (data(17 + i * sChannelLens) == 13) & ...   % '\r'
               (data(18 + i * sChannelLens) == 10) & ...   % '\n'
               (data(19 + i * sChannelLens) == 67) & ...   % 'C'
               (data(20 + i * sChannelLens) == 72) & ...   % 'H'
               (data(23 + i * sChannelLens) == 13) & ...   % '\r'
               (data(24 + i * sChannelLens) == 10)         % '\n'

                % get current channel data
                channel   = (data(21 + i * sChannelLens) - 48) * 10 + (data(22 + i * sChannelLens) - 48);
                fileData(:, i + 1) = data(24 + i * sChannelLens + 1 : 24 + i * sChannelLens + dataLens);
            else
                sprintf('ERROR - channel header\r\n');
            end
        end
    else
        sprintf('ERROR - channel size\r\n');
    end
else
    sprintf('ERROR - file header\r\n');
end

fclose(file);

if fileDecode == 1
    dataLens = dataLens / 2;
    for i = 1 : channel
        for j = 1 : dataLens
            signal(j, i) = unsigned2signed(fileData(j * 2, i) * 2^8 + fileData(j * 2 - 1, i));
        end
    end
    hold on;
    grid on;
    plot([1 : dataLens], signal(:, 1 : channel));
else
    sprintf('ERROR - file decode failed\r\n');
end
