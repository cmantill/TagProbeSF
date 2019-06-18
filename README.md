#  TagProbeSF

Code to calculate SF using template fit and combine (in lxplus)

1. setup combine

2. get code

```
git clone https://github.com/cmantill/TagProbeSF.git
```

3.  install tagandprobe
```
cd TagProbeSF
mv TagAndProbeExtended.py $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/python/
scram b -j 4
```

4. run the SF code:

```
bash calcSF_shift.sh T n2 hbb 04 1 1 0.05 1
T: WP
n2: algorithm
hbb: tag
04: version of bits
1: shift by (GeV)
1: scale * in datacard (if 0 deactivated)
0.05: smear by (%)
1: smear * in datacard (if 0 deactivated)
```
