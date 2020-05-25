# Adjust environment setting to enable PODIO at runtime
#
# The PODIO environment variable must be set to use this
# script
if [ -z "$PODIO" ]; then
  echo "Please set the PODIO enviroment variable to the install location before sourcing this script."
  return
fi

unamestr=`uname`
if [[ "$unamestr" == 'Linux' ]]; then
	echo $LD_LIBRARY_PATH | grep $PODIO/lib >& /dev/null
	if [ $? == "1" ]; then
    # RedHat based put the libraries into lib64
    if [ -d $PODIO/lib64 ]; then
      export LD_LIBRARY_PATH=$PODIO/lib64:$LD_LIBRARY_PATH
    else
      export LD_LIBRARY_PATH=$PODIO/lib:$LD_LIBRARY_PATH
    fi
	fi
elif [[ "$unamestr" == 'Darwin' ]]; then
  # This currenty does not work on OS X as DYLD_LIBRARY_PATH is ignored
  # in recent OS X versions
	echo $DYLD_LIBRARY_PATH | grep $PODIO/lib >& /dev/null
	if [ $? == "1" ]; then
    	export DYLD_LIBRARY_PATH=$PODIO/lib:$DYLD_LIBRARY_PATH
    fi
fi
echo $PYTHONPATH | grep $PODIO/python >& /dev/null
if [ $? == "1" ]; then
	export PYTHONPATH=$PODIO/python:$PYTHONPATH
fi
