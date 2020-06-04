clear;

fid = fopen('data.bin','r');
data = fread(fid,'uint32');
data = reshape(data,33,length(data)/33)';

samples = data(:,1);
time = samples / 20000;
data1 = data(:,2:end);
data1 = 0.195*(data1 - 32768); 
% data1 = abs(data1);

% emg.filter = bandpass(emg.raw,[5 500],20000);
[b,a] = butter(5,450/(1000/2),'low');
data2 = filter(b,a,data1);

figure(1);
subplot(1,2,1); plot(time,data1(:,13)); set(gca,'xlim',[1.0 1.1]);
subplot(1,2,2); plot(time,data2(:,13)); set(gca,'xlim',[1.0 1.1]);

figure(2);
for ch = 17:32
    subplot(4,4,ch-16); plot(samples,data2(:,ch));
    set(gca,'xlim',[1000 1200]);
end

figure(3);
for ch = 1:16
    subplot(4,4,ch); plot(samples,data2(:,ch));
    set(gca,'xlim',[1000 1200]);
end