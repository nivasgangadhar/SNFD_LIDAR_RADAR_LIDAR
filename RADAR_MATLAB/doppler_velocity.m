c=3e8;
f=77e9;
lambda=c/f;
doppler_frequency_shift=[3e3,-4.5e3,11e3,-3e3];
relative_vel=lambda*doppler_frequency_shift/2;
disp(relative_vel);