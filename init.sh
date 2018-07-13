platform='unknown'
unamestr=`uname`

# If the --nolcg argument is present, then don't try to pick a gcc
# from the CVMFS installation of LCG
if [ "$1" == "--nolcg" ]; then
	NOLCG=1
fi

# If FCCEDM is set then the environment is setup
if [ -z "$FCCEDM" ]; then
	# If PODIO is not set, make a guess...
	if [ -z "$PODIO" ]; then
    	export PODIO=$PWD/install
    	echo "PODIO has been to $PODIO"
	fi
fi

# Try to bootstrap a sensible development environment if it seems one
# is not setup already
if [ -z "$NOLCG" ]; then
	if [[ "$unamestr" == 'Linux' ]]; then
    	platform='Linux'
    	if [[ -d /cvmfs/sft.cern.ch/lcg ]]; then
    		# Do we have an cvmfs g++ already? If we do it's probably ok.
    		if type -p g++ | grep cvmfs &> /dev/null; then
    			echo "Found g++ from CVMFS, not modifying your setup (see README.md)"
    		else
    			# Setup for either SLC6 or CentOS7...
    			distro=""
    			if [ -f /etc/redhat-release ]; then
	    			if grep "SLC release 6" /etc/redhat-release >& /dev/null; then
    					distro="slc6"
    				else grep "CentOS Linux release 7" /etc/redhat-release >& /dev/null;
    					distro="centos7"
    				fi
    			fi
    			if [ -n "$distro" ]; then
    				echo "Setting up LCG_93 for $distro"  
        			source /cvmfs/sft.cern.ch/lcg/views/LCG_93/x86_64-${distro}-gcc7-opt/setup.sh
			        export PATH=$PATH:/cvmfs/sft.cern.ch/lcg/contrib/llvm/3.9.0/x86_64-slc6-gcc49-opt/bin  # Why?
        		else
        			echo "No LCG setup supported for non SCL6/CentOS7 machines - make sure your environment setup is ok (see README.md)"
        		fi
			fi
		fi
    fi
else
	echo "Not attempting to setup LCG - make sure your environment setup is ok (see README.md)"
fi

# Now set environment variables, if it's not yet done
if [[ "$unamestr" == 'Linux' ]]; then
	echo $LD_LIBRARY_PATH | grep $PODIO/lib >& /dev/null
	if [ $? == "1" ]; then
		export LD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$LD_LIBRARY_PATH
	fi
elif [[ "$unamestr" == 'Darwin' ]]; then
    platform='Darwin'
	echo $DYLD_LIBRARY_PATH | grep $PODIO/lib >& /dev/null
	if [ $? == "1" ]; then
    	export DYLD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$DYLD_LIBRARY_PATH
    fi
fi

echo $PYTHONPATH | grep $PODIO/python >& /dev/null
if [ $? == "1" ]; then
	export PYTHONPATH=$PODIO/python:$PYTHONPATH
fi
