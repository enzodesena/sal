load 'sine_original.txt';
load 'sine_doppler.txt';
load 'current_latency.txt';

FS = 44100;

t = linspace(0,length(sine_original)/FS, length(sine_original));
subplot(3,1,1);
plot(t',sine_original);
hold on;
plot(t', sine_doppler);
plot(diff(sine_doppler));
hold off;



[H_original] = freqz(sine_original, 1, 2^16, FS);
[H_doppler, f] = freqz(sine_doppler, 1, 2^16, FS);

subplot(3,1,2);
plot(f, 20*log10(abs(H_original)));
hold on;
plot(f, 20*log10(abs(H_doppler)));
hold off;

subplot(3,1,3);
plot(diff(current_latency(1:end-1)));

sine_freq = 1000;
v = 343; % sound speed
vs = 171.5; % source speed

doppler_shift = (v+vs)/v*sine_freq


sound(sine_original, FS);
pause
sound(sine_doppler, FS);

