addpath('SOFA_API_MO/');

filename = 'Kemar_HRTF_sofa.sofa';
s1 = SOFAload(filename);

[dtf,ctf]=SOFAhrtf2dtf(s1);
SOFAsave('Kemar_HRTF_sofa_diffuseFieldEqualised_48000HzAKTools.sofa', dtf);