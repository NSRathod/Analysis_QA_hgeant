#!/bin/bash

# This is the generic jobscript to run jobs on GridEngine
#
# The script supports up to 7 parameters.
#
# The user specific part of the script is indicated below.



par1="no"
par2="no"
par3="no"
par4="no"
par5="no"
par6="no"
par7="no"

if [ $# -lt 1 ]
then
        echo "Usage : jobScript.sh par1 [par2] [par2] [par3] [par4] [par5] [par6] [par7]"
        sleep 3
        exit
fi

case "$#" in

1)  par1=$1
    ;;
2)  par1=$1
    par2=$2
    ;;
3)  par1=$1
    par2=$2
    par3=$3
    ;;
4)  par1=$1
    par2=$2
    par3=$3
    par4=$4
    ;;
5)  par1=$1
    par2=$2
    par3=$3
    par4=$4
    par5=$5
    ;;
6)  par1=$1
    par2=$2
    par3=$3
    par4=$4
    par5=$5
    par6=$6
    ;;
7)  par1=$1
    par2=$2
    par3=$3
    par4=$4
    par5=$5
    par6=$6
    par7=$7
    ;;
*) echo "Unsupported number of arguments" 
   echo "Usage : jobScript.sh par1 [par2] [par2] [par3] [par4] [par5] [par6] [par7]"
   exit
   ;;
esac

    echo "par1 = ${par1}"
    echo "par2 = ${par2}"
    echo "par3 = ${par3}"
    echo "par4 = ${par4}"
    echo "par5 = ${par5}"
    echo "par6 = ${par6}"
    echo "par7 = ${par7}"

    host=$(hostname)
    format='+%Y/%m/%d-%H:%M:%S'

    date $format
    echo ""               
    echo "--------------------------------"
    echo "RUNNING ON HOST : " $(hostname)
    echo "WORKING DIR     : " $(pwd)
    echo "USER is         : " $USER
    echo "JOBID           : $JOB_ID"
    echo "DISK USAGE /tmp :"
    df -h /tmp
    echo "--------------------------------"
    
    
    echo ""               
    echo "--------------------------------"
    echo "par1 = ${par1}"
    echo "par2 = ${par2}"
    echo "par3 = ${par3}"
    echo "par4 = ${par4}"
    echo "par5 = ${par5}"
    echo "par6 = ${par6}"
    echo "par7 = ${par7}"
    echo "--------------------------------"
    echo ""



    echo ""
    echo "--------------------------------"
    echo " DEBUG INFO"                     
    echo "==> Kernel version information :"
    uname -a                               
    echo "==> C compiler that built the kernel:"
    cat /proc/version                           
    echo "==> load on this node:"               
    mpstat -P ALL                               
    echo "==> actual compiler is"               
    gcc -v                                      
    echo "--------------------------------"     
    echo ""               








###################################################################
###################################################################
#   EDIT THIS PART TO EXECUTE YOUR JOB!



#----------------------
# evironment 
  echo "==> running environment script ${par1}"
. ${par1}
. ${par7}
#----------------------

  echo "==> ldd ${par2}"
  ldd $par2

  echo "==> execute program "

  filelist=$(echo $par3 | sed 's/,/ /g')

  for file in $filelist
  do
     echo "==> $par2 -f ${par5} -i $file -I $par6 -o $par4"
     #time $par2 -f ${par5} -i $file -I $par6 -o $par4 
     time $par2  $file  
     status=$?
     
     
     if [ $status -ne 0 ]
     then
        echo "JOB: $JOB_ID CRASHED ON HOST: $host WITH INPUTFILE $file"
        echo "JOB: $JOB_ID CRASHED ON HOST: $host WITH INPUTFILE $file" > ${par4}/crash/${file}_crash.txt
     fi

     
     
     
  done

#   END EDIT YOUR EXECUT JOB!
###################################################################
###################################################################

    





    echo ""               
    echo "--------------------------------"
    echo "Job with params "
    echo "par1 = ${par1}"  
    echo "par2 = ${par2}"  
    echo "par3 = ${par3}"  
    echo "par4 = ${par4}"  
    echo "par5 = ${par5}"  
    echo "par6 = ${par6}"  
    echo "par7 = ${par7}"  
    echo "finsished!"      
    echo "--------------------------------"
    echo ""
    

    
    echo ""               
    echo "--------------------------------"
    echo "MONITOR ENVIRONMENT:"
    echo "JOB EXE   :---------------------"
    qstat -j $JOB_ID
    echo "HOST LOAD :---------------------"
    qhost -h $host
    echo "OTHER JOBS ON HOST:-------------"
    qhost -h $host -j
    echo "DISK USAGE /tmp :---------------"
    df -h /tmp 
    echo "MEM/CPU USAGE :-----------------"
    qstat -j $JOB_ID | grep usage
    echo "--------------------------------"
   
    
    
    
    date $format


    sleep 2

