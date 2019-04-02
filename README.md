#  TagProbeSF

Code to calculate SF using templates and combine


(I) setup

-> setup combine from here:
https://github.com/nucleosynthesis/HiggsAnalysis-CombinedLimit/wiki/gettingstarted#for-end-users-that-dont-need-to-commit-or-do-any-development

-> get the SF code:
git clone ssh://git@gitlab.cern.ch:7999/gouskos/hrtsf.git

-> run:
cd TagProbeSF
mv TagAndProbeExtended.py $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/python/
scram b -j 4


(II) brief instructions

-> run the SF code:
./calcTopWSF.sh "object"
where object [for now] is "T" or "W".

It currently has two categories: matched cat2 (signal) and unmatched cat1 (all bkg).

The templates and the datacards are produced with makeSFTemplates.C and makeSFDatacard.C scripts. Simple examples for now.

Pre/Post fit plots are produced while running this step. The actual code is in makePlots.C. 

-> In makePlots.C one can find more useful functions: 

(a) read the scale factors from the root files produced by combine, 
(b) calculate working points, etc..

