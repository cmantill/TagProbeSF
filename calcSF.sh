#!/bin/bash
object=$1
algo=$2
wp=$3
whichbit=$4
#For bit version, enter either 04 or 05

declare -a ptranges
#ptranges=("low" "lowmed" "med")
#ptranges=("low")
ptranges=("incl")

workdir=${object}"_"${algo}
mkdir ${workdir}

## loop over the pt bins
for ptrange in "${ptranges[@]}";
do
    ##make templates
    echo "make templates"
    echo "${whichbit}"
    
    # for bacon skim -> comment this if the templates are already done in coffea
    cmdpass=$(echo 'makeSFTemplates.C("'${object}'","'${algo}'","'${wp}'","'${ptrange}'","'${whichbit}'",true)')
    cmdfail=$(echo 'makeSFTemplates.C("'${object}'","'${algo}'","'${wp}'","'${ptrange}'","'${whichbit}'",false)')
    root -l -q ${cmdpass}
    root -l -q ${cmdfail}

    # pre-templates done

    ## add scale and smear
    inputname=${object}"_"${algo}"_"${wp}"_"${whichbit}"_"${ptrange}
    python makeSmearShift.py --ifile ${workdir}/${inputname}_pass_pre.root
    python makeSmearShift.py --ifile ${workdir}/${inputname}_fail_pre.root

    #mv ${workdir}/${inputname}_pass_pre.root ${workdir}/${inputname}_pass.root
    #mv ${workdir}/${inputname}_fail_pre.root ${workdir}/${inputname}_fail.root 

    ## make datacard
    echo "make datacard"
    echo " "
    cd ${workdir}
    inputname=${object}"_"${algo}"_"${wp}"_"${whichbit}"_"${ptrange}
    cp ../makeSFDatacard.C .
    cmdmakedatacard=$(echo 'makeSFDatacard.C("'${inputname}'")')
    root -l -q ${cmdmakedatacard} > sf.txt
    sed -n -i '3,$ p' sf.txt
    
    ## do the tag and probe
    echo "run the tag and probe"
    if [ ${object} == "T" ];
    then
	text2workspace.py -m 125 -P HiggsAnalysis.CombinedLimit.TagAndProbeExtended:tagAndProbe sf.txt --PO categories=catp2,catp1
    elif [ ${object} == "W" ];
    then	
	text2workspace.py -m 125 -P HiggsAnalysis.CombinedLimit.TagAndProbeExtended:tagAndProbe sf.txt --PO categories=catp2,catp1
    fi
    mv sf.root sf"_"${inputname}".root"
    echo "Do the MultiDimFit"
    combine -M MultiDimFit -m 125 sf"_"${inputname}.root --algo=singles --robustFit=1 --cminDefaultMinimizerTolerance 5.
    echo "Run the FitDiagnostics"    
    #combine -M FitDiagnostics -m 125 sf"_"${inputname}.root --saveShapes --saveWithUncertainties --robustFit=1 --cminDefaultMinimizerTolerance 5. -t -1 --setParameters scale=0.5
    combine -M FitDiagnostics -m 125 sf"_"${inputname}.root --saveShapes --saveWithUncertainties --robustFit=1 --cminDefaultMinimizerTolerance 5.
    mv fitDiagnostics.root sf"_"fitDiagnostics"_"${inputname}".root"
    mv sf.txt sf"_"datacard"_"${inputname}".txt"
    cd ../
    # for the bacon templates
    cmdmake=$(echo 'makePlots.C("'${workdir}'","'${inputname}'","'${wp}'","'${ptrange}'",'50.','110.','20',"mass")') 
    # for the coffea templates
    #cmdmake=$(echo 'makePlots.C("'${workdir}'","'${inputname}'","'${wp}'","'${ptrange}'",'50.','110.','15',"mass")') 
    root -l -q ${cmdmake}
done
